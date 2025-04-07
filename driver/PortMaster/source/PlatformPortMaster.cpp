// SPDX-FileCopyrightText: 2025 AJ Walter
// SPDX-License-Identifier: GPL-3.0-or-later OR MIT

#include "CreateSDL2.hpp"

#include "Driver/Tools/Configuration.hpp"
#include "Driver/Platform.hpp"

#include <SDL.h>

#include <iostream>
#include <sys/stat.h>

namespace SuperHaxagon {
	void initializePlatform(std::string& sdmc, std::string& romfs, bool& backslash, ControllerSettings& controllerSettings) {
		sdmc = "./sdmc";
		romfs = "./romfs";
		backslash = false;

		// I am making the assumption that most PortMaster compatible handhelds use a Nintendo style layout,
		// and REPORT THOSE BUTTONS CORRECTLY (looking at you, knulli)
		controllerSettings = ControllerSettings::NINTENDO;

		mkdir(sdmc.c_str(), 0755);
	}

	SDL_Window* getWindow(const Platform&) {
		return SDL_CreateWindow(APP_NAME, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 0, 0, SDL_WINDOW_FULLSCREEN_DESKTOP | SDL_WINDOW_ALLOW_HIGHDPI);
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
