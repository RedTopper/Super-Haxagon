#ifndef DRIVER_SCREEN_HPP
#define DRIVER_SCREEN_HPP

// SPDX-FileCopyrightText: 2025 AJ Walter
// SPDX-License-Identifier: GPL-3.0-or-later OR MIT

#include "Driver/Tools/Color.hpp"
#include "Driver/Tools/Vector.hpp"

#include <vector>
#include <memory>

namespace SuperHaxagon {
	class Screen {
	public:
		struct ScreenImpl;
		explicit Screen(std::unique_ptr<ScreenImpl> impl);
		Screen(Screen&) = delete;
		~Screen();

		// Draws a polygon to the screen with absolute coordinates.
		void drawPoly(const Color& color, const std::vector<Vec2f>& points) const;

		Vec2f getScreenDim() const;
		void screenBegin() const;
		void screenFinalize() const;
		void screenSwitch() const;
		void clear(const Color& color) const;

	private:
		std::unique_ptr<ScreenImpl> _impl;
	};
}

#endif //DRIVER_SCREEN_HPP
