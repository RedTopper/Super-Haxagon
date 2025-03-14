#include "Driver/Platform.hpp"

#include "Core/Configuration.hpp"
#include "Driver/Common/SDL2/CreateSDL2.hpp"

#include <SDL.h>

#include <iostream>

namespace SuperHaxagon {
	void platformSpecificHints() {}

	SDL_Window* getWindow(const Platform&) {
		return SDL_CreateWindow(APP_NAME, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 0, 0, SDL_WINDOW_FULLSCREEN_DESKTOP | SDL_WINDOW_ALLOW_HIGHDPI);
	}

	ControllerSettings getDefaultControllerSettings() {
		// I am making the assumption that most PortMaster compatible handhelds use a Nintendo style layout,
		// and REPORT THOSE BUTTONS CORRECTLY (looking at you, knulli)
		return ControllerSettings::NINTENDO;
	}

	void Platform::message(const Dbg dbg, const std::string& where, const std::string& message) const {
		if (dbg == Dbg::INFO) {
			std::cout << "[portmaster:info] " + where + ": " + message << std::endl;
		} else if (dbg == Dbg::WARN) {
			std::cout << "[portmaster:warn] " + where + ": " + message << std::endl;
		} else if (dbg == Dbg::FATAL) {
			std::cerr << "[portmaster:fatal] " + where + ": " + message << std::endl;
		}
	}

	Supports Platform::supports() {
		return Supports::SHADOWS;
	}
}
