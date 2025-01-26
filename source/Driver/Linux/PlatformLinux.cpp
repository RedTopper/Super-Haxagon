#include "Driver/Linux/PlatformLinux.hpp"

#include "Core/Twist.hpp"

#include <iostream>
#include <sys/stat.h>

namespace SuperHaxagon {
	PlatformLinux::PlatformLinux(const Dbg dbg) : PlatformSFML(dbg, sf::VideoMode(
		static_cast<int>(sf::VideoMode::getDesktopMode().width * 0.75),
		static_cast<int>(sf::VideoMode::getDesktopMode().height * 0.75)
	)) {

		if (std::getenv("container")) {
			const auto* data = std::getenv("XDG_DATA_HOME");
			_sdmc = std::string(data) + "/sdmc";
			_romfs = std::string("/app/romfs");
		} else {
			_sdmc = "./sdmc";
			_romfs = "./romfs";
		}

		mkdir(_sdmc.c_str(), 0755);
	}

	std::string PlatformLinux::getPath(const std::string& partial, const Location location) {
		switch (location) {
		case Location::ROM:
			return _romfs + partial;
		case Location::USER:
			return _sdmc + partial;
		}

		return "";
	}

	void PlatformLinux::message(const Dbg dbg, const std::string& where, const std::string& message) {
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

	std::unique_ptr<Twist> PlatformLinux::getTwister() {
		std::random_device source;
		std::mt19937::result_type data[std::mt19937::state_size];
		generate(std::begin(data), std::end(data), ref(source));
		return std::make_unique<Twist>(
				std::make_unique<std::seed_seq>(std::begin(data), std::end(data))
		);
	}
}
