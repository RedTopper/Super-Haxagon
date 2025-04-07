// SPDX-FileCopyrightText: 2025 AJ Walter
// SPDX-License-Identifier: GPL-3.0-or-later OR MIT

#include "Driver/Platform.hpp"

#include "CreateSDL2.hpp"
#include "Driver/Tools/Configuration.hpp"
#include "Driver/Tools/Random.hpp"
#include "Driver/Font.hpp"
#include "Driver/Music.hpp"
#include "Driver/Screen.hpp"
#include "Driver/Sound.hpp"

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>

#include <iostream>
#include <chrono>
#include <thread>
#include <sys/stat.h>

namespace SuperHaxagon {
	std::unique_ptr<Music> createMusic(const std::string& path);
	std::unique_ptr<Screen> createScreen(SDL_Window* window, SDL_Renderer* renderer);
	std::unique_ptr<Sound> createSound(const std::string& path);
	std::unique_ptr<Font> createFont(const Platform& platform, SDL_Renderer* renderer, SDL_Surface* surface, const std::string& path, int size);

	SDL_GameController* findController(const Platform& platform) {
		for (int i = 0; i < SDL_NumJoysticks(); i++) {
			if (SDL_IsGameController(i)) {
				platform.message(Dbg::INFO, "findController", "found controller: " + std::to_string(i));
				return SDL_GameControllerOpen(i);
			} else {
				platform.message(Dbg::INFO, "findController", "controller " + std::to_string(i) + " incompatible");
			}
		}

		return nullptr;
	}

	struct Platform::PlatformImpl {
		PlatformImpl() : screen(createScreen(nullptr, nullptr)) {}

		~PlatformImpl() {
			SDL_DestroyRenderer(renderer);
			SDL_DestroyWindow(window);
			Mix_Quit();
			SDL_Quit();
		}

		void sdlInit(const Platform& platform) {
			initializePlatform(sdmc, romfs, backslash, controllerSettings);

			if (SDL_InitSubSystem(SDL_INIT_VIDEO) < 0) {
				platform.message(Dbg::FATAL, "sdlInit", "sdl could not init video!");
				return;
			}

			if (SDL_InitSubSystem(SDL_INIT_AUDIO) < 0) {
				platform.message(Dbg::WARN, "sdlInit", "sdl could not init audio!");
			}

			if (SDL_InitSubSystem(SDL_INIT_GAMECONTROLLER) < 0) {
				platform.message(Dbg::WARN, "sdlInit", "sdl could not init game controllers!");
			}

			Mix_Init(MIX_INIT_OGG);
			Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 4096);
			Mix_AllocateChannels(16);

			TTF_Init();

			SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
			SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 2);

			window = getWindow(platform);
			if (!window) {
				platform.message(Dbg::FATAL, "sdlInit", "could not init sfml window!");
				return;
			}

			renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
			if (!renderer) {
				// fallback to software
				platform.message(Dbg::WARN, "sdlInit", "falling back to software rendering");
				renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
				software = true;
				if (!renderer) {
					platform.message(Dbg::FATAL, "sdlInit", "... and it failed!");
					return;
				}
			}

			SDL_ShowCursor(SDL_DISABLE);
			SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
			SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
			SDL_RenderClear(renderer);
			SDL_RenderPresent(renderer);

			SDL_StopTextInput();

			// Add version information to title
			SDL_SetWindowTitle(window, (std::string(APP_NAME " (") + VERSION + ")").c_str());

			time = std::chrono::high_resolution_clock::now();
			screen = createScreen(window, renderer);
			controller = findController(platform);

			// This environment variable gets set in the SuperHaxagon.sh script (or manually by the user)
			if (const auto* env = std::getenv("CONTROLLER")) {
				const std::string layout = env;

				// Fix for knulli using weird keys.
				if (layout == "knulli") controllerSettings = ControllerSettings::SWAPPED_FACE_BUTTONS;

				// Fix for if you want the face buttons of an xbox controller to map to the prompts.
				// Any CFW using XBox style controls should be listed below
				if (layout == "xbox") controllerSettings = ControllerSettings::XBOX;

				// Fix for if you want the face buttons of an xbox controller to map to the prompts,
				// but you also want "B" (east) to be your select button.
				// This is a user preference.
				if (layout == "xbox_b") controllerSettings = ControllerSettings::XBOX_B_SELECT;
			}

			switch (controllerSettings) {
				default:
				case ControllerSettings::NINTENDO:
					platform.message(Dbg::INFO, "sdlInit",  "nintendo controller layout"); break;
				case ControllerSettings::XBOX:
					platform.message(Dbg::INFO, "sdlInit",  "xbox controller layout"); break;
				case ControllerSettings::XBOX_B_SELECT:
					platform.message(Dbg::INFO, "sdlInit",  "xbox controller layout with b as select"); break;
				case ControllerSettings::SWAPPED_FACE_BUTTONS:
					platform.message(Dbg::INFO, "sdlInit",  "controller knulli mode"); break;
			}

			const auto* driver = SDL_GetCurrentVideoDriver();
			if (driver) platform.message(Dbg::INFO, "sdlInit", "using driver " + std::string(driver));

			loaded = true;

			if (controller) showKbdControls = false;
		}

		bool loop(const Platform& platform) {
			if (!loaded) return false;

			if (software) {
				// We aren't locked to the framerate of a hardware device,
				// so attempt to make our own frame cap of 60fps
				auto now = std::chrono::high_resolution_clock::now();
				auto us = std::chrono::duration_cast<std::chrono::microseconds>(now - time).count();
				auto timeToSleep = 16666 - us;
				if (timeToSleep < 0) timeToSleep = 0;
				std::this_thread::sleep_for(std::chrono::microseconds(timeToSleep));
			}

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
						showKbdControls = true;
						break;
					case SDL_CONTROLLERDEVICEADDED:
						if (!controller) {
							platform.message(Dbg::INFO, "loop", "controller connected");
							controller = SDL_GameControllerOpen(ev.cdevice.which);
							showKbdControls = false;
						}
						break;
					case SDL_CONTROLLERDEVICEREMOVED:
						if (controller && ev.cdevice.which == SDL_JoystickInstanceID(SDL_GameControllerGetJoystick(controller))) {
							platform.message(Dbg::INFO, "loop", "controller disconnected");
							SDL_GameControllerClose(controller);
							controller = findController(platform);
							showKbdControls = true;
						}
						break;
					case SDL_CONTROLLERBUTTONDOWN:
					case SDL_CONTROLLERBUTTONUP:
						if (controller && ev.cdevice.which == SDL_JoystickInstanceID(SDL_GameControllerGetJoystick(controller))) {
							setController(buttons, ev.cbutton.button, ev.cbutton.state == SDL_PRESSED, controllerSettings);
							showKbdControls = false;
						}
						break;
				}
			}

			return true;
		}

		std::string getPath(const std::string& partial, const Location location) const {
			auto path = partial;

			if (backslash) std::replace(path.begin(), path.end(), '/', '\\');

			switch (location) {
				case Location::ROM:
					return romfs + path;
				case Location::USER:
					return sdmc + path;
			}

			return "";
		}

		std::chrono::system_clock::time_point time;
		std::unique_ptr<Screen> screen;
		Buttons buttons{};

		float frameTime = 0.0f;
		bool loaded = false;
		bool showKbdControls = true;
		bool software = false;
		bool backslash = false;
		ControllerSettings controllerSettings = ControllerSettings::NINTENDO;
		SDL_Window* window = nullptr;
		SDL_Renderer* renderer = nullptr;
		SDL_GameController* controller = nullptr;
		std::string sdmc;
		std::string romfs;
	};

	Platform::Platform() : _impl(std::make_unique<PlatformImpl>()) {
		_impl->sdlInit(*this);
	}

	Platform::~Platform() = default;

	bool Platform::loop() {
		return _impl->loop(*this);
	}

	float Platform::getDilation() const {
		return _impl->frameTime / (1.0f/60.0f);
	}

	std::string Platform::getPath(const std::string& partial, const Location location) const {
		return _impl->getPath(partial, location);
	}

	std::unique_ptr<Font> Platform::loadFont(const int size) const {
		if (!_impl->loaded) return nullptr;
		return createFont(*this, _impl->renderer, nullptr, getPath("/bump-it-up.ttf", Location::ROM), size);
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

	std::string Platform::getButtonName(ButtonName buttonName) {
		return ::SuperHaxagon::getButtonName(buttonName, _impl->showKbdControls, _impl->controllerSettings);
	}

	Buttons Platform::getPressed() const {
		return _impl->buttons;
	}

	void Platform::shutdown() {}
}
