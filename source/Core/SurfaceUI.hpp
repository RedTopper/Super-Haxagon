#ifndef SUPER_HAXAGON_SURFACE_UI_HPP
#define SUPER_HAXAGON_SURFACE_UI_HPP

// SPDX-FileCopyrightText: 2025 AJ Walter
// SPDX-License-Identifier: GPL-3.0-or-later

#include "Structs.hpp"

#include <vector>

namespace SuperHaxagon {
	class Screen;
	class SurfaceUI {
	public:
		explicit SurfaceUI(Screen& screen);

		~SurfaceUI() = default;

		/**
		 * Draws a polygon directly to the screen on the range of [0, screen size].
		 * Top left is 0,0
		 */
		void drawPolyUI(const Color& color, const std::vector<Vec2f>& points);

		// Convenience functions that call into the function above
		void drawPolyUITopLeft(const Color& color, const Vec2f& dim);
		void drawPolyUIBottomLeft(const Color& color, const Vec2f& dim);
		void drawPolyUITopRight(const Color& color, const Vec2f& dim);
		void drawPolyUIBottomRight(const Color& color, const Vec2f& dim);

		/**
		 * Draws a rectangle at position with the size of size.
		 * Position is the top left.
		 */
		void drawRectUI(Color color, Vec2f position, Vec2f size);

		Vec2f getScreenDim() const;
		float getScale() const;
		float getScreenDimMax() const;
		float getScreenDimMin() const;

	private:
		Screen& _screen;
	};
}

#endif //SUPER_HAXAGON_SURFACE_UI_HPP
