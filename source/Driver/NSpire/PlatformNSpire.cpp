#include "Driver/NSpire/PlatformNSpire.hpp"

#include "Driver/NSpire/FontNSpire.hpp"
#include "Driver/NSpire/AudioNSpire.hpp"

#include "Core/Twist.hpp"

#include <time.h>
#include <iostream>
#include <libndls.h>

namespace SuperHaxagon {
	PlatformNSpire::PlatformNSpire(const Dbg dbg) : Platform(dbg) {
		nglInit();
		_framebuffer = std::make_unique<COLOR[]>(SCREEN_WIDTH * SCREEN_HEIGHT);
		nglSetBuffer(_framebuffer.get());
	}

	std::string PlatformNSpire::getPath(const std::string& partial) {
		return getPathRom(partial);
	}

	std::string PlatformNSpire::getPathRom(const std::string& partial) {
		return std::string("ndless") + partial + std::string(".tns");
	}

	std::unique_ptr<Audio> PlatformNSpire::loadAudio(const std::string&, SuperHaxagon::Stream) {
		return std::make_unique<AudioNSpire>();
	}

	std::unique_ptr<Font> PlatformNSpire::loadFont(const std::string&, int size) {
		return std::make_unique<FontNSpire>(size);
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
		Buttons buttons{};
		buttons.select = isKeyPressed(KEY_NSPIRE_ENTER) > 0;
		buttons.back = isKeyPressed(KEY_NSPIRE_ESC) > 0;
		buttons.quit = isKeyPressed(KEY_NSPIRE_HOME) > 0;
		buttons.left = isKeyPressed(KEY_NSPIRE_4) > 0;
		buttons.right = isKeyPressed(KEY_NSPIRE_6)  > 0;
		return buttons;
	}

	Point PlatformNSpire::getScreenDim() const {
		return {320, 240};
	}

	void PlatformNSpire::screenBegin() {
		glPushMatrix();
		glColor3f(0.4f, 0.7f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glTranslatef(0, 0, 1000);
		_angle += 1;
		nglRotateY(_angle.normaliseAngle());
		glBegin(GL_TRIANGLES);
		nglAddVertices(_angle < GLFix(90) || _angle > GLFix(270) ? triangle : (triangle + 3), 3);
		glEnd();
	}

	void PlatformNSpire::screenFinalize() {
		glPopMatrix();
		nglDisplay();
	}

	void PlatformNSpire::drawPoly(const Color&, const std::vector<Point>&) {

	}

	void PlatformNSpire::shutdown() {
		nglUninit();
	}

	void PlatformNSpire::message(const Dbg dbg, const std::string& where, const std::string& message) {
		if (dbg == Dbg::INFO) {
			std::cout << "[ndless:info] " + where + ": " + message << std::endl;
		} else if (dbg == Dbg::WARN) {
			std::cout << "[ndless:warn] " + where + ": " + message << std::endl;
		} else if (dbg == Dbg::FATAL) {
			std::cerr << "[ndless:fatal] " + where + ": " + message << std::endl;
		}
	}

	std::unique_ptr<Twist> PlatformNSpire::getTwister() {
		auto* a = new std::seed_seq{time(nullptr)};
		return std::make_unique<Twist>(
				std::unique_ptr<std::seed_seq>(a)
		);
	}

	bool PlatformNSpire::loop() {
		return true;
	}
}
