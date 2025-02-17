#include "Driver/Platform.hpp"

#include "Core/Structs.hpp"
#include "Core/Twist.hpp"
#include "Driver/Font.hpp"
#include "Driver/Music.hpp"
#include "Driver/Screen.hpp"
#include "Driver/Sound.hpp"

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

#include <ctime>
#include <memory>
#include <iostream>
#include <chrono>

namespace SuperHaxagon {
	std::unique_ptr<Music> createMusic();
	std::unique_ptr<Screen> createScreen(const Platform& platform);
	std::unique_ptr<Sound> createSound();
	std::unique_ptr<Font> createFont();

	void setKey(Buttons& buttons, int key, bool pressing) {
		switch (key) {
			case SDLK_ESCAPE:
				buttons.back = pressing; return;
			case SDLK_DELETE:
				buttons.quit = pressing; return;
			case SDLK_RETURN:
				buttons.select = pressing; return;
			case SDLK_LEFT:
			case SDLK_a:
				buttons.left = pressing; return;
			case SDLK_RIGHT:
			case SDLK_d:
				buttons.right = pressing; return;
			default:
				return;
		}
	}

	struct Platform::PlatformImpl {
		PlatformImpl() {
			if (SDL_InitSubSystem(SDL_INIT_VIDEO) < 0) {
				message(Dbg::FATAL, "platform", "sdl could not init video!");
				return;
			}

			if (SDL_InitSubSystem(SDL_INIT_AUDIO) < 0) {
				message(Dbg::FATAL, "platform", "sdl could not init audio!");
				return;
			}

			Mix_Init(MIX_INIT_OGG);
			Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 4096);
			Mix_AllocateChannels(16);

			SDL_StopTextInput();

			time = std::chrono::high_resolution_clock::now();
		}

		~PlatformImpl() {
			Mix_Quit();
			SDL_Quit();
		}

		bool loop() {
			auto now = std::chrono::high_resolution_clock::now();
			auto us = std::chrono::duration_cast<std::chrono::microseconds>(now - time).count();
			frameTime = static_cast<float>(us) /  1000000.0f;
			time = now;

			SDL_Event ev;
			while (SDL_PollEvent(&ev)) {
				if (ev.type == SDL_QUIT) {
					return false;
				}

				if (ev.type == SDL_KEYDOWN || ev.type == SDL_KEYUP) {
					bool press = ev.key.state == SDL_PRESSED;
					message(Dbg::INFO, "keyboard", press ? "KeyDown" : "KeyUp");
					setKey(buttons, ev.key.keysym.sym, press);
				}
			}

			return true;
		}

		void message(const Dbg dbg, const std::string& where, const std::string& message) const {
			if (dbg == Dbg::INFO) {
				std::cout << "[sdl2:info] " + where + ": " + message << std::endl;
			} else if (dbg == Dbg::WARN) {
				std::cout << "[sdl2:warn] " + where + ": " + message << std::endl;
			} else if (dbg == Dbg::FATAL) {
				std::cerr << "[sdl2:fatal] " + where + ": " + message << std::endl;
			}
		}

		std::chrono::system_clock::time_point time;
		std::unique_ptr<Screen> screen;
		float frameTime = 0.0f;
		Buttons buttons{};
	};

	Platform::Platform() : _impl(std::make_unique<PlatformImpl>()) {
		// create screen once _impl is valid
		_impl->screen = createScreen(*this);
	}

	Platform::~Platform() {
		// de-init screen before anything else
		_impl->screen = nullptr;
	}

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

	std::unique_ptr<Font> Platform::loadFont(const int) const {
		return nullptr;
	}

	std::unique_ptr<Sound> Platform::loadSound(const std::string&) const {
		return nullptr;
	}

	std::unique_ptr<Music> Platform::loadMusic(const std::string& base, const Location location) const {
		return nullptr;
	}

	Screen& Platform::getScreen() {
		return *_impl->screen;
	}

	std::string Platform::getButtonName(const Buttons& button) {
		if (button.back) return "ESC";
		if (button.select) return "ENTER";
		if (button.left) return "LEFT";
		if (button.right) return "RIGHT";
		if (button.quit) return "DELETE";
		return "?";
	}

	Buttons Platform::getPressed() const {
		return _impl->buttons;
	}

	void Platform::shutdown() {}

	Supports Platform::supports() {
		return Supports::SHADOWS;
	}

	void Platform::message(const Dbg dbg, const std::string& where, const std::string& message) const {
		_impl->message(dbg, where, message);
	}

	std::unique_ptr<Twist> Platform::getTwister() {
		auto* a = new std::seed_seq{time(nullptr)};
		return std::make_unique<Twist>(
				std::unique_ptr<std::seed_seq>(a)
		);
	}
}
