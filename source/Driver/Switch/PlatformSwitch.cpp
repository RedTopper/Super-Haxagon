#include <switch.h>
#include <sys/stat.h>
#include <iostream>

#include "Driver/Switch/PlatformSwitch.hpp"

namespace SuperHaxagon {
	PlatformSwitch::PlatformSwitch(const Dbg dbg): PlatformSFML(dbg, sf::VideoMode::getFullscreenModes()[0]) {
		romfsInit();
		mkdir("sdmc:/haxagon", 0777);
		_console = std::ofstream("sdmc:/haxagon/out.log");
	}

	std::string PlatformSwitch::getPath(const std::string& partial) {
		return "sdmc:/haxagon" + partial;
	}

	std::string PlatformSwitch::getPathRom(const std::string& partial) {
		return "romfs:" + partial;
	}

	bool PlatformSwitch::loop() {
		return appletMainLoop() && PlatformSFML::loop();
	}
	
	std::unique_ptr<Twist> PlatformSwitch::getTwister() {
		// ALSO a shitty way to do this but it's the best I got.
		const auto a = new std::seed_seq{ svcGetSystemTick() };
		return std::make_unique<Twist>(
			std::unique_ptr<std::seed_seq>(a)
		);
	}
	
	void PlatformSwitch::show() {
		auto display = false;
		for (const auto& message : _messages) {
			if (message.first == Dbg::FATAL) {
				display = true;
			}
		}

		if (display) {
			// Need to create console to show user the error
			consoleInit(nullptr);
			std::cout << "Fatal error! START to quit." << std::endl;
			std::cout << "Last messages:" << std::endl << std::endl;
			for (const auto& message : _messages) {
				std::cout << message.second << std::endl;
			}

			while (appletMainLoop()) {
				consoleUpdate(nullptr);
				hidScanInput();
				const auto kDown = hidKeysDown(CONTROLLER_P1_AUTO);
				if (kDown & KEY_PLUS) break;
			}
		}
	}

	void PlatformSwitch::message(const Dbg dbg, const std::string& where, const std::string& message) {
		std::string format;
		if (dbg == Dbg::INFO) {
			format = "[switch:info] ";
		}
		else if (dbg == Dbg::WARN) {
			format = "[switch:warn] ";
		}
		else if (dbg == Dbg::FATAL) {
			format = "[switch:fatal] ";
		}

		format += where + ": " + message;

		if (_dbg != Dbg::FATAL) {
			// If we are in non FATAL mode, write to a file
			_console << format << std::endl;
		}

		_messages.emplace_back(dbg, format);
		if (_messages.size() > 32) _messages.pop_front();
	}
}
