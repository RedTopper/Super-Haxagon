// SPDX-FileCopyrightText: 2025 AJ Walter
// SPDX-License-Identifier: GPL-3.0-or-later OR MIT

#include "CreateSDL2.hpp"

#include "Driver/Tools/Configuration.hpp"
#include "Driver/Platform.hpp"

#include <SDL.h>

#include <iostream>
#include <filesystem>
#include <sys/stat.h>

namespace SuperHaxagon {
	void initializePlatform(std::string& sdmc, std::string& romfs, bool& backslash, ControllerSettings& controllerSettings) {
		SDL_SetHint(SDL_HINT_WINDOWS_DPI_AWARENESS, "permonitor");
		SDL_SetHint(SDL_HINT_VIDEODRIVER, "wayland,x11");

		if (std::getenv("container") && std::filesystem::exists("/app")) {
			// If we are in a container and /app exists, we're very likely
			// in the flatpak version.
			const auto* data = std::getenv("XDG_DATA_HOME");
			sdmc = std::string(data) + "/sdmc";
			romfs = std::string("/app/romfs");
		} else {
			sdmc = "./sdmc";
			romfs = "./romfs";
		}

		// Linux does not use backslashes for path separation.
		backslash = false;

		// I'm making the assumption that most Desktop players are using a standard XBox controller,
		// and prefer that "A" means select things.
		controllerSettings = ControllerSettings::XBOX;

		mkdir(sdmc.c_str(), 0755);
	}

	SDL_Window* getWindow(const Platform& platform) {
		const char* envFullscreen = std::getenv("FULLSCREEN");
		const char* envDesktopSession = std::getenv("DESKTOP_SESSION");
		const char* envSteamDeck = std::getenv("SteamDeck");

		bool fsForceOn = envFullscreen && (envFullscreen == std::string("1"));
		bool fsForceOff = envFullscreen && (envFullscreen == std::string("0"));
		bool steamos =
			(envDesktopSession && std::string(envDesktopSession) == "plasma-steamos-oneshot") ||
			(envDesktopSession && std::string(envDesktopSession) == "gamescope-wayland") ||
			(envSteamDeck && std::string(envSteamDeck) == "1");

		if (fsForceOn || (steamos && !fsForceOff)) {
			if (fsForceOn) platform.message(Dbg::INFO, "getWindow", "fullscreen forced on");

			if (steamos) {
				platform.message(Dbg::INFO, "getWindow", "steam deck detected, using fullscreen");
				platform.message(Dbg::INFO, "getWindow", "force off with FULLSCREEN=0 env var");
			}

			// Run in fullscreen if we forced it, otherwise only steamos should run in fullscreen (unless we force it off)
			return SDL_CreateWindow(APP_NAME, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1280, 720, SDL_WINDOW_FULLSCREEN_DESKTOP | SDL_WINDOW_ALLOW_HIGHDPI);
		}

		if (fsForceOff) platform.message(Dbg::INFO, "getWindow", "fullscreen forced off");

		constexpr int n3dsWidth = 400;
		constexpr int n3dsHeight = 240;
		int displayWidth;
		int displayHeight;
		int scale = 1;

		SDL_DisplayMode mode;
		if (!SDL_GetCurrentDisplayMode(0, &mode)) {
			auto scaleW = (mode.w - 1) / n3dsWidth;
			auto scaleH = (mode.h - 1) / n3dsHeight;
			scale = std::max(1, std::min(scaleW, scaleH));
		} else {
			platform.message(Dbg::WARN, "window", "failed to get desktop resolution");
		}

		displayWidth = n3dsWidth * scale;
		displayHeight = n3dsHeight * scale;

		return SDL_CreateWindow(APP_NAME, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, displayWidth, displayHeight, SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
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
