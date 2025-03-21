#ifndef SUPER_HAXAGON_SCREEN_HPP
#define SUPER_HAXAGON_SCREEN_HPP

// SPDX-FileCopyrightText: 2025 AJ Walter, see driver/Common/README.md for MIT usage.
// SPDX-License-Identifier: GPL-3.0-or-later OR MIT

#include "Core/Vector.hpp"

#include <vector>
#include <memory>

namespace SuperHaxagon {
	struct Color;

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

#endif //SUPER_HAXAGON_SCREEN_HPP
