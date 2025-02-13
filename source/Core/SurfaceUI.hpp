#ifndef SUPER_HAXAGON_SURFACE_UI_HPP
#define SUPER_HAXAGON_SURFACE_UI_HPP

#include "Structs.hpp"

#include <vector>

namespace SuperHaxagon {
	class Screen;
	class SurfaceUI {
	public:
		explicit SurfaceUI(Screen& screen);
		virtual ~SurfaceUI() = default;

		/**
		 * Draws a polygon directly to the screen on the range of [0, screen size].
		 * Top left is 0,0
		 */
		virtual void drawPolyUI(const Color& color, const std::vector<Vec2f>& points);

		/**
		 * Draws a rectangle at position with the size of size.
		 * Position is the top left.
		 */
		void drawRectUI(Color color, Vec2f position, Vec2f size);

		virtual Vec2f getScreenDim() const;
		float getScale() const;
		float getScreenDimMax() const;
		float getScreenDimMin() const;

	private:
		Screen& _screen;
	};
}

#endif //SUPER_HAXAGON_SURFACE_UI_HPP
