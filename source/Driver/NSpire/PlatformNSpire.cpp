#include "Driver/NSpire/PlatformNSpire.hpp"

#include "Driver/NSpire/FontNSpire.hpp"
#include "Driver/NSpire/AudioNSpire.hpp"

#include "Core/Twist.hpp"

#include <time.h>
#include <iostream>

namespace SuperHaxagon {
	PlatformNSpire::PlatformNSpire(const Dbg dbg) : Platform(dbg) {
		// Initialize nGL first
		nglInit();
		// Allocate the framebuffer
		COLOR *framebuffer = new COLOR[SCREEN_WIDTH * SCREEN_HEIGHT];
		nglSetBuffer(framebuffer);
	}

	std::string PlatformNSpire::getPath(const std::string& partial) {
		return std::string("./sdmc") + partial + std::string(".tns");
	}

	std::string PlatformNSpire::getPathRom(const std::string& partial) {
		return std::string("./romfs") + partial + std::string(".tns");
	}

	void PlatformNSpire::message(const Dbg dbg, const std::string& where, const std::string& message) {
		if (dbg == Dbg::INFO) {
			std::cout << "[win:info] " + where + ": " + message << std::endl;
		} else if (dbg == Dbg::WARN) {
			std::cout << "[win:warn] " + where + ": " + message << std::endl;
		} else if (dbg == Dbg::FATAL) {
			std::cerr << "[win:fatal] " + where + ": " + message << std::endl;
		}
	}

	std::unique_ptr<Twist> PlatformNSpire::getTwister() {
		auto* a = new std::seed_seq{time(nullptr)};
		return std::make_unique<Twist>(
				std::unique_ptr<std::seed_seq>(a)
		);
	}

	std::unique_ptr<Audio> PlatformNSpire::loadAudio(const std::string&, SuperHaxagon::Stream) {
		return std::unique_ptr<AudioNSpire>();
	}

	std::unique_ptr<Font> PlatformNSpire::loadFont(const std::string&, int) {
		return std::unique_ptr<FontNSpire>();
	}

	std::string PlatformNSpire::getButtonName(const Buttons& button) {
		if (button.back) return "ESC";
		if (button.select) return "ENTER";
		if (button.left) return "4";
		if (button.right) return "6";
		if (button.quit) return "HOME";
		return "?";
	}

	Buttons PlatformNSpire::getPressed() {
		return Buttons();
	}

	Point PlatformNSpire::getScreenDim() const {
		return {320, 240};
	}

	void PlatformNSpire::screenBegin() {

	}

	void PlatformNSpire::screenSwap() {

	}

	void PlatformNSpire::screenFinalize() {

	}

	void PlatformNSpire::drawPoly(const Color&, const std::vector<Point>&) {

	}
}
