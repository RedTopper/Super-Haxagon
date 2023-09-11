#include "Driver/macOS/PlatformMacOS.hpp"

#include "Core/Twist.hpp"

#include <iostream>
#include <sys/stat.h>

namespace SuperHaxagon {
	PlatformMacOS::PlatformMacOS(const Dbg dbg) : PlatformSFML(dbg, sf::VideoMode(
		static_cast<int>(sf::VideoMode::getDesktopMode().width * 0.75),
		static_cast<int>(sf::VideoMode::getDesktopMode().height * 0.75)
	)) {
		mkdir("./sdmc", 0755);
	}

	std::string PlatformMacOS::getPath(const std::string& partial, const Location location) {
		switch (location) {
		case Location::ROM:
			return std::string("./romfs") + partial;
		case Location::USER:
			return std::string("./sdmc") + partial;
		}

		return "";
	}

	void PlatformMacOS::message(const Dbg dbg, const std::string& where, const std::string& message) {
		if (dbg == Dbg::INFO) {
			std::cout << "[macOS:info] " + where + ": " + message << std::endl;
		} else if (dbg == Dbg::WARN) {
			std::cout << "[macOS:warn] " + where + ": " + message << std::endl;
		} else if (dbg == Dbg::FATAL) {
			std::cerr << "[macOS:fatal] " + where + ": " + message << std::endl;
		}
	}

	std::unique_ptr<Twist> PlatformMacOS::getTwister() {
		std::random_device source;
		std::mt19937::result_type data[std::mt19937::state_size];
		generate(std::begin(data), std::end(data), ref(source));
		return std::make_unique<Twist>(
				std::make_unique<std::seed_seq>(std::begin(data), std::end(data))
		);
	}
}
