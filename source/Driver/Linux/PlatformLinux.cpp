#include "Driver/Platform.hpp"

#include "Core/Twist.hpp"
#include "Driver/Screen.hpp"
#include "Driver/SFML/CreateSFML.hpp"

#include <iostream>
#include <sys/stat.h>

namespace SuperHaxagon {
	void osSpecificWindowSetup(sf::Window&) {}
	void initializePlatform(std::string& sdmc, std::string& romfs, bool& platformBackslash) {
		if (std::getenv("container")) {
			const auto* data = std::getenv("XDG_DATA_HOME");
			sdmc = std::string(data) + "/sdmc";
			romfs = std::string("/app/romfs");
		} else {
			sdmc = "./sdmc";
			romfs = "./romfs";
		}

		platformBackslash = false;

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

	std::unique_ptr<Twist> Platform::getTwister() {
		std::random_device source;
		std::mt19937::result_type data[std::mt19937::state_size];
		generate(std::begin(data), std::end(data), ref(source));
		return std::make_unique<Twist>(
				std::make_unique<std::seed_seq>(std::begin(data), std::end(data))
		);
	}
}
