// SPDX-FileCopyrightText: 2025 AJ Walter, see driver/Example/README.md for MIT usage.
// SPDX-License-Identifier: GPL-3.0-or-later OR MIT

#include "Driver/Screen.hpp"

#include "Core/Structs.hpp"

namespace SuperHaxagon {
	struct Screen::ScreenImpl {
		ScreenImpl() {
			// This is where your driver implements drawing routines.
			// SFML grabs the window from the Platform via a constructor,
			// but your device may require its own rendering library.
		}
	};

	Screen::Screen(std::unique_ptr<ScreenImpl> data) : _impl(std::move(data)) {}

	Screen::~Screen() = default;

	Vec2f Screen::getScreenDim() const {
		// If your platform only has one screen resolution,
		// you might as well hard-code it.
		return {640.0f, 480.0f};
	}

	void Screen::screenBegin() const {}

	void Screen::screenSwitch() const {}

	void Screen::screenFinalize() const {
		// Do anything to swap the render buffer to the screen here
	}

	void Screen::drawPoly(const Color& color, const std::vector<Vec2f>& points) const {
		// The game will pass in screen space coordinates to this function.
	}

	void Screen::clear(const Color& color) const {}

	Screen createScreen() {
		return Screen{std::make_unique<Screen::ScreenImpl>()};
	}
}
