#include "Driver/Platform.hpp"

#include "Core/Twist.hpp"
#include "Driver/Screen.hpp"
#include "Driver/Common/SFML/CreateSFML.hpp"

#include <iostream>
#include <sys/stat.h>

namespace SuperHaxagon {
	void osSpecificWindowSetup(sf::Window&) {}
	void initializePlatform(std::string& sdmc, std::string& romfs, bool& platformBackslash) {
		sdmc = "./sdmc";
		romfs = "./romfs";

		platformBackslash = false;

		mkdir(sdmc.c_str(), 0755);
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

	std::unique_ptr<Twist> Platform::getTwister() {
		std::random_device source;
		std::mt19937::result_type data[std::mt19937::state_size];
		generate(std::begin(data), std::end(data), ref(source));
		return std::make_unique<Twist>(
				std::make_unique<std::seed_seq>(std::begin(data), std::end(data))
		);
	}
}
