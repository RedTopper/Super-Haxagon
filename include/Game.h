#ifndef SUPER_HAXAGON_GAME_H
#define SUPER_HAXAGON_GAME_H

#include <cmath>
#include <vector>
#include <memory>

#include "Driver/Platform.h"
#include "State.h"
#include "Level.h"
#include "Structs.h"

namespace SuperHaxagon {
	class Game {
	public:
		static constexpr double TAU = M_PI * 2;
		static const Color COLOR_SHADOW = {0, 0, 0, 0xC0};

		int run(Platform& platform);
		const std::vector<std::unique_ptr<Level>>& getLevels() const { return levels; }

		/**
		 * Draws the background of the screen (the radiating colors part)
		 */
		static void drawBackground(const Platform& platform, const Color& color1, const Color& color2, const Point& focus, double height, double rotation, double sides);

		/**
		 * Draws a regular polygon at some point focus. Useful for generating
		 * the regular polygon in the center of the screen.
		 */
		static void drawRegular(const Platform& platform, const Color& color, const Point& focus, int height, double rotation, double sides);
		static double linear(double start, double end, double percent);
		static Color interpolateColor(const Color& one, const Color& two, double percent);

	private:
		std::unique_ptr<State> state;
		std::vector<std::unique_ptr<Level>> levels;
	};
}

#endif //SUPER_HAXAGON_GAME_H
