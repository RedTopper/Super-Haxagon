// SPDX-FileCopyrightText: 2025 AJ Walter and adc-ax, see driver/SFML/README.md for MIT usage.
// SPDX-License-Identifier: GPL-3.0-or-later OR MIT

#include "CreateSFML.hpp"

#include "Driver/Platform.hpp"

#include <CoreFoundation/CFBundle.h>

#include <iostream>
#include <sys/stat.h>

namespace SuperHaxagon {
	void osSpecificWindowSetup(sf::Window&) {}
	void initializePlatform(std::string& sdmc, std::string& romfs, bool& platformBackslash) {

		platformBackslash = false;
		CFURLRef appUrlRef = CFBundleCopyResourceURL(CFBundleGetMainBundle(), CFSTR("levels"), CFSTR("haxagon"), nullptr);

		sdmc = "./sdmc";
		romfs = "./romfs";

		if (const char* homeDir = getenv("HOME")) {
			sdmc = std::string(homeDir) + "/Library/Application Support/net.awalter.SuperHaxagon";
		}

		mkdir(sdmc.c_str(), 0755);

		if (appUrlRef) {
			const CFStringRef filePathRef = CFURLCopyPath(appUrlRef);
			const char* filePath = CFStringGetCStringPtr(filePathRef, kCFStringEncodingUTF8);
			const auto path =  std::filesystem::path(std::string(filePath));

			romfs = path.parent_path().string();
			CFRelease(filePathRef);
			CFRelease(appUrlRef);
		}
	}

	void Platform::message(const Dbg dbg, const std::string& where, const std::string& message) const {
		if (dbg == Dbg::INFO) {
			std::cout << "[macOS:info] " + where + ": " + message << std::endl;
		} else if (dbg == Dbg::WARN) {
			std::cout << "[macOS:warn] " + where + ": " + message << std::endl;
		} else if (dbg == Dbg::FATAL) {
			std::cerr << "[macOS:fatal] " + where + ": " + message << std::endl;
		}
	}
}
