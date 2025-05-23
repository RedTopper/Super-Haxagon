// SPDX-FileCopyrightText: 2020 AJ Walter
// SPDX-License-Identifier: GPL-3.0-or-later OR MIT

#include "CreateSFML.hpp"

#include "Driver/Platform.hpp"
#include "Driver/Screen.hpp"

#include <filesystem>
#include <iostream>
#include <sys/stat.h>

namespace SuperHaxagon {
	void osSpecificWindowSetup(sf::Window&) {}
	void initializePlatform(std::string& sdmc, std::string& romfs, bool& backslash) {
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

		mkdir(sdmc.c_str(), 0755);
	}

	void Platform::message(const Dbg dbg, const std::string& where, const std::string& message) const {
		if (dbg == Dbg::INFO) {
			std::cout << "[linux:info] " + where + ": " + message << std::endl;
		} else if (dbg == Dbg::WARN) {
			std::cout << "[linux:warn] " + where + ": " + message << std::endl;
		} else if (dbg == Dbg::FATAL) {
			std::cerr << "[linux:fatal] " + where + ": " + message << std::endl;
			// Linux users don't need any message boxes like windows because they
			// spend their entire life in the console anyway.
		}
	}
}
