// SPDX-FileCopyrightText: 2025 AJ Walter
// SPDX-License-Identifier: GPL-3.0-or-later OR MIT

#include "Driver/Platform.hpp"

#include "Driver/Tools/Random.hpp"
#include "Driver/Font.hpp"
#include "Driver/Music.hpp"
#include "Driver/Screen.hpp"
#include "Driver/Sound.hpp"

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>

#include <iostream>
#include <chrono>
#include <thread>
#include <sys/stat.h>

namespace SuperHaxagon {
	std::unique_ptr<Music> createMusic(const std::string& path);
	std::unique_ptr<Screen> createScreen(SDL_Window* window, SDL_Renderer* renderer, SDL_Surface* surface);
	std::unique_ptr<Sound> createSound(const std::string& path);
	std::unique_ptr<Font> createFont(const Platform& platform, SDL_Renderer* renderer, SDL_Surface* surface, const std::string& path, int size);

	void setKey(Buttons& buttons, int key, bool pressing) {
		switch (key) {
			case SDLK_SPACE: // A
				buttons.select = pressing; return;
			case SDLK_LCTRL: // B
				buttons.back = pressing; return;
			case SDLK_LEFT: // DPAD LEFT
			case SDLK_LALT: // Y
			case SDLK_e: // L1
			case SDLK_TAB: // L2
				buttons.left = pressing; return;
			case SDLK_RIGHT: // DPAD RIGHT
			case SDLK_LSHIFT: // X
			case SDLK_t: // R1
			case SDLK_BACKSPACE: // R2
				buttons.right = pressing; return;
			case SDLK_RETURN: // START
				buttons.quit = pressing; return;
			default:
				return;
		}
	}

	std::string getButtonName(ButtonName buttonName) {
		switch (buttonName) {
			case ButtonName::SELECT: return "A";
			case ButtonName::BACK: return "B";
			case ButtonName::LEFT: return "L1 | L2 | PAD L | Y";
			case ButtonName::RIGHT: return "R1 | R2 | PAD R | X";
			case ButtonName::QUIT: return "START | MENU";
			default: return "?";
		}
	}

	struct Platform::PlatformImpl {
		PlatformImpl() : screen(createScreen(nullptr, nullptr, nullptr)) {}

		~PlatformImpl() {
			SDL_FreeSurface(surface);
			SDL_DestroyRenderer(renderer);
			SDL_DestroyWindow(window);
			Mix_Quit();
			SDL_Quit();
		}

		void sdlInit(const Platform& platform) {
			SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS);

			window = SDL_CreateWindow("main", 0, 0, 640, 480, SDL_WINDOW_SHOWN);
			if (window) platform.message(Dbg::INFO, "init", "created window");

			renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
			if (renderer) platform.message(Dbg::INFO, "init", "created renderer");

			surface = SDL_CreateRGBSurface(0, 640, 480, 32, 0, 0, 0, 0);
			if (surface) platform.message(Dbg::INFO, "init", "created surface");

			if (!window || !renderer || !surface) {
				// uh oh
				platform.message(Dbg::FATAL, "init", "sdl2 init failed");
				return;
			}

			Mix_Init(MIX_INIT_OGG);
			Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 4096);
			Mix_AllocateChannels(8);

			TTF_Init();

			SDL_StopTextInput();

			mkdir("./sdmc", 0755);

			time = std::chrono::high_resolution_clock::now();
			screen = createScreen(window, renderer, surface);
			loaded = true;
		}

		bool loop() {
			if (!loaded) return false;

			auto now = std::chrono::high_resolution_clock::now();
			auto us = std::chrono::duration_cast<std::chrono::microseconds>(now - time).count();
			frameTime = static_cast<float>(us) /  1000000.0f;
			time = now;

			SDL_Event ev;
			while (SDL_PollEvent(&ev)) {
				switch(ev.type) {
					case SDL_QUIT:
						return false;
					case SDL_KEYDOWN:
					case SDL_KEYUP:
						setKey(buttons, ev.key.keysym.sym, ev.key.state == SDL_PRESSED);
						break;
				}
			}

			return true;
		}

		std::chrono::system_clock::time_point time;
		std::unique_ptr<Screen> screen;
		Buttons buttons{};

		float frameTime = 0.0f;
		bool loaded = false;
		SDL_Window* window = nullptr;
		SDL_Renderer* renderer = nullptr;
		SDL_Surface* surface = nullptr;
		SDL_GLContext context = {0};
	};

	Platform::Platform() : _impl(std::make_unique<PlatformImpl>()) {
		_impl->sdlInit(*this);
	}

	Platform::~Platform() = default;

	bool Platform::loop() {
		return _impl->loop();
	}

	float Platform::getDilation() const {
		return _impl->frameTime / (1.0f/60.0f);
	}

	std::string Platform::getPath(const std::string& partial, const Location location) const {
		switch (location) {
			default:
			case Location::ROM:
				return "./romfs" + partial;
			case Location::USER:
				return "./sdmc" + partial;
		}
	}

	std::unique_ptr<std::istream> Platform::openFile(const std::string& partial, const Location location) const {
		return std::make_unique<std::ifstream>(getPath(partial, location), std::ios::in | std::ios::binary);
	}

	std::unique_ptr<Font> Platform::loadFont(const int size) const {
		if (!_impl->loaded) return nullptr;
		return createFont(*this, nullptr, _impl->surface, getPath("/bump-it-up.ttf", Location::ROM), size);
	}

	std::unique_ptr<Sound> Platform::loadSound(const std::string& base) const {
		return createSound(getPath(base, Location::ROM) + ".wav");
	}

	std::unique_ptr<Music> Platform::loadMusic(const std::string& base, const Location location) const {
		return createMusic(getPath(base, location) + ".ogg");
	}

	Screen& Platform::getScreen() {
		return *_impl->screen;
	}

	std::vector<std::pair<Location, std::string>> Platform::loadUserLevels() {
		std::vector<std::pair<Location, std::string>> levels;
		auto file = openFile("/levels.haxagon", Location::USER);
		if (file->good()) {
			levels.emplace_back(Location::USER, "/levels.haxagon");
		}

		return levels;
	}

	std::string Platform::getButtonName(ButtonName buttonName) {
		return ::SuperHaxagon::getButtonName(buttonName);
	}

	Buttons Platform::getPressed() const {
		return _impl->buttons;
	}

	void Platform::shutdown() {}

	void Platform::message(const Dbg dbg, const std::string& where, const std::string& message) const {
		if (dbg == Dbg::INFO) {
			std::cout << "[mmiyoo:info] " + where + ": " + message << std::endl;
		} else if (dbg == Dbg::WARN) {
			std::cout << "[mmiyoo:warn] " + where + ": " + message << std::endl;
		} else if (dbg == Dbg::FATAL) {
			std::cerr << "[mmiyoo:fatal] " + where + ": " + message << std::endl;
		}
	}

	// Baby, we're using software rasterization, there's no way in hell shadows are going to be performant!
	Supports Platform::supports() {
		return Supports::NOTHING;
	}
}
