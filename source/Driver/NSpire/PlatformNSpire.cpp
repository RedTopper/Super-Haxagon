#include "Driver/NSpire/PlatformNSpire.hpp"

#include "Driver/NSpire/FontNSpire.hpp"
#include "Driver/NSpire/AudioNSpire.hpp"

#include "Core/Twist.hpp"

#include <time.h>
#include <iostream>
#include <libndls.h>
#include "gldrawarray.h"

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
		// The depth is arbitrary, but needed for triangle stacking.
		// If this is updated, the scale factor in drawPoly will
		// also need to be updated
		//
		// This is only an issue since nGL forces perspective even though
		// the game really should be isometric
		_z = 5000;
		glPushMatrix();
		glColor3f(0.4f, 0.7f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void PlatformNSpire::screenFinalize() {
		glPopMatrix();
		nglDisplay();
	}

	void PlatformNSpire::drawPoly(const Color& color, const std::vector<Point>& points) {
		COLOR c = ((color.r & 0b11111000) << 8) | ((color.g & 0b11111100) << 3) | (color.b >> 3);
		auto dim = getScreenDim();
		auto pos = std::make_unique<VECTOR3[]>(points.size());
		auto proc = std::make_unique<ProcessedPosition[]>(points.size());
		auto index = std::make_unique<IndexedVertex[]>((points.size() - 2) * 3);

		const auto SCALE = 20;

		for (size_t i = 0; i < points.size(); i++) {
			// Note the flipping of the y value with -SCALE!
			pos[i] = {(int)((points[i].x - dim.x/2) * SCALE), (int)((points[i].y - dim.y/2) * -SCALE), _z};
		}

		// This is a hack - The game calculates the triangles
		// "backwards" after flipping the screen upside down
		// using -SCALE and nGL backface culls them away. Instead
		// of being reasonable and fixing the logic elsewhere, we can
		// flip the index of two points to reverse the triangle.
		for (size_t i = 1; i < points.size() - 1; i++) {
			const auto pa = VERTEX(pos[0].x, pos[0].y, 0, 0, 0, 0);
			const auto pb = VERTEX(pos[i].x, pos[i].y, 0, 0, 0, 0);
			const auto pc = VERTEX(pos[i + 1].x, pos[i + 1].y, 0, 0, 0, 0);
			auto isBackface = nglIsBackface(&pa, &pb, &pc);

			index[i*3 - 3] = {0, 0, 0, c};
			index[i*3 - 2] = {i + (isBackface ? 0 : 1), 0, 0, c};
			index[i*3 - 1] = {i + (isBackface ? 1 : 0), 0, 0, c};
		}

		nglDrawArray(index.get(), (points.size() - 2) * 3, pos.get(), points.size(), proc.get(), GL_TRIANGLES);
		_z--;
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
