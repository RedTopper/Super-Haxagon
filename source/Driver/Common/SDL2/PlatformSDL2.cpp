#include "Driver/Platform.hpp"

#include "Core/Structs.hpp"
#include "Core/Twist.hpp"
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
	std::unique_ptr<Font> createFont(SDL_Renderer& renderer, const std::string& path, int size);

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

	void setController(Buttons& buttons, int key, bool pressing) {
		switch (key) {
			case SDL_CONTROLLER_BUTTON_A:
				buttons.select = pressing; return;
			case SDL_CONTROLLER_BUTTON_B:
				buttons.back = pressing; return;
			case SDL_CONTROLLER_BUTTON_DPAD_LEFT:
			case SDL_CONTROLLER_BUTTON_LEFTSHOULDER:
				buttons.left = pressing; return;
			case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
			case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER:
			case SDL_CONTROLLER_BUTTON_Y:
				buttons.right = pressing; return;
			case SDL_CONTROLLER_BUTTON_START:
				buttons.quit = pressing; return;
			default: return;
		}
	}

	SDL_GameController *findController() {
		for (int i = 0; i < SDL_NumJoysticks(); i++) {
			if (SDL_IsGameController(i)) {
				std::cout << "Found a controller!" << std::endl;
				return SDL_GameControllerOpen(i);
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
			SDL_SetHint(SDL_HINT_WINDOWS_DPI_AWARENESS, "permonitor");
			//SDL_SetHint(SDL_HINT_VIDEODRIVER, "x11");

			if (SDL_InitSubSystem(SDL_INIT_VIDEO) < 0) {
				platform.message(Dbg::FATAL, "platform", "sdl could not init video!");
				return;
			}

			if (SDL_InitSubSystem(SDL_INIT_AUDIO) < 0) {
				platform.message(Dbg::WARN, "platform", "sdl could not init audio!");
			}

			if (SDL_InitSubSystem(SDL_INIT_GAMECONTROLLER) < 0) {
				platform.message(Dbg::WARN, "platform", "sdl could not init game controllers!");
			}

			Mix_Init(MIX_INIT_OGG);
			Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 4096);
			Mix_AllocateChannels(16);

			TTF_Init();

			SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
			SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 2);

			window = SDL_CreateWindow("Super Haxagon", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 0, 0, SDL_WINDOW_FULLSCREEN_DESKTOP | SDL_WINDOW_ALLOW_HIGHDPI);
			if (!window) {
				platform.message(Dbg::FATAL, "screen", "could not init sfml window!");
				return;
			}

			renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
			if (!renderer) {
				// fallback to software
				platform.message(Dbg::WARN, "screen", "falling back to software rendering");
				renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
				software = true;
				if (!renderer) {
					platform.message(Dbg::FATAL, "screen", "... and it failed!");
					return;
				}
			}

			SDL_ShowCursor(SDL_DISABLE);
			SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
			SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
			SDL_RenderClear(renderer);
			SDL_RenderPresent(renderer);

			SDL_StopTextInput();

			mkdir("./sdmc", 0755);

			time = std::chrono::high_resolution_clock::now();
			screen = createScreen(window, renderer);
			controller = findController();
			loaded = true;
		}

		bool loop() {
			if (software) {
				// We aren't locked to the framerate of a hardware device,
				// so attempt to make our own frame cap of 60fps
				auto now = std::chrono::high_resolution_clock::now();
				auto us = std::chrono::duration_cast<std::chrono::microseconds>(now - time).count();
				auto timeToSleep = 16666 - us;
				if (timeToSleep < 0) timeToSleep = 0;
				std::this_thread::sleep_for(std::chrono::microseconds (timeToSleep));
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
						break;
					case SDL_CONTROLLERDEVICEADDED:
						if (!controller) {
							controller = SDL_GameControllerOpen(ev.cdevice.which);
						}
						break;
					case SDL_CONTROLLERDEVICEREMOVED:
						if (controller && ev.cdevice.which == SDL_JoystickInstanceID(SDL_GameControllerGetJoystick(controller))) {
							SDL_GameControllerClose(controller);
							controller = findController();
						}
						break;
					case SDL_CONTROLLERBUTTONDOWN:
					case SDL_CONTROLLERBUTTONUP:
						if (controller && ev.cdevice.which == SDL_JoystickInstanceID(SDL_GameControllerGetJoystick(controller))) {
							setController(buttons, ev.cbutton.button, ev.cbutton.state == SDL_PRESSED);
						}
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
		bool software = false;
		SDL_Window* window = nullptr;
		SDL_Renderer* renderer = nullptr;
		SDL_GameController* controller = nullptr;
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

	std::unique_ptr<Font> Platform::loadFont(const int size) const {
		if (!_impl->loaded) return nullptr;
		return createFont(*_impl->renderer, getPath("/bump-it-up.ttf", Location::ROM), size);
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
}
