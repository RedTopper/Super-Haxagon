// SPDX-FileCopyrightText: 2025 AJ Walter
// SPDX-License-Identifier: GPL-3.0-or-later OR MIT

#include "CreateSDL2.hpp"

#include "Driver/Tools/Configuration.hpp"
#include "Driver/Platform.hpp"

#include <SDL.h>

#include <iostream>

namespace SuperHaxagon {
	void platformSpecificHints() {
		SDL_SetHint(SDL_HINT_WINDOWS_DPI_AWARENESS, "permonitor");
		SDL_SetHint(SDL_HINT_VIDEODRIVER, "wayland");
	}

	SDL_Window* getWindow(const Platform& platform) {
		constexpr int dsWidth = 400;
		constexpr int dsHeight = 240;
		int displayWidth;
		int displayHeight;
		int scale = 1;

		SDL_DisplayMode mode;
		if (!SDL_GetCurrentDisplayMode(0, &mode)) {
			auto scaleW = (mode.w - 1) / dsWidth;
			auto scaleH = (mode.h - 1) / dsHeight;
			scale = std::max(1, std::min(scaleW, scaleH));
		} else {
			platform.message(Dbg::WARN, "window", "failed to get desktop resolution");
		}

		displayWidth = dsWidth * scale;
		displayHeight = dsHeight * scale;

		return SDL_CreateWindow(APP_NAME, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, displayWidth, displayHeight, SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
	}

	ControllerSettings getDefaultControllerSettings() {
		// I'm making the assumption that most Desktop players are using a standard XBox controller,
		// and prefer that "A" means select things.
		return ControllerSettings::XBOX;
	}

	void Platform::message(const Dbg dbg, const std::string& where, const std::string& message) const {
		if (dbg == Dbg::INFO) {
			std::cout << "[linuxSDL2:info] " + where + ": " + message << std::endl;
		} else if (dbg == Dbg::WARN) {
			std::cout << "[linuxSDL2:warn] " + where + ": " + message << std::endl;
		} else if (dbg == Dbg::FATAL) {
			std::cerr << "[linuxSDL2:fatal] " + where + ": " + message << std::endl;
		}
	}

	Supports Platform::supports() {
		return Supports::SHADOWS;
	}
}
