#ifndef SUPER_HAXAGON_STRUCTS_HPP
#define SUPER_HAXAGON_STRUCTS_HPP

#include <cstdint>

namespace SuperHaxagon {
	struct Color {
		uint8_t r;
		uint8_t g;
		uint8_t b;
		uint8_t a;
	};

	struct Point {
		int x;
		int y;
	};

	enum class Movement {
		CAN_MOVE,
		CANNOT_MOVE_LEFT,
		CANNOT_MOVE_RIGHT,
		DEAD,
	};

	static constexpr double TAU = M_PI * 2;
	static const Color COLOR_SHADOW = {0, 0, 0, 0xC0};
	static const Color COLOR_TRANSPARENT = {0, 0, 0, 0xA0};
	static const Color COLOR_GREY = {0xA0, 0xA0, 0xA0, 0xFF};
	static const Color COLOR_WHITE =  {0xFF, 0xFF, 0xFF, 0xFF};
	static const Color COLOR_BLACK =  {0, 0, 0, 0xFF};


	/**
	 * Linear interpolation between two colors
	 */
	static Color interpolateColor(const Color& one, const Color& two, double percent);

	/**
	 * Linear interpolation between two doubles
	 */
	static double linear(double start, double end, double percent);

	/**
	 * Calculates a point (either for a wall or a general point) based on the running level's rotation, some offset
	 * so it renders correctly, some distance the point should be from the center, the side the point
	 * should be rendered on, based on a total number of sides the shape has.
	 */
	static Point calcPoint(const Point& focus, double rotation, double offset, double distance);

	/**
	 * Kind of like getScoreText(int score), this method converts a score
	 * into a string. The format of this method looks like such:
	 *
	 * TTT:%% where  TTT is the current time in seconds, and %% is  the percent
	 * through the current second.
	 *
	 * getBestTime(int score) is the same, but it says BEST: instead of TIME:
	 */
	static std::string getBestTime(int score);

	/**
	 * Gets the current level that the score represents (for example, point)
	 * if the user is less than 10 seconds into the game)
	 */
	static const char* getScoreText(int score);
}

#endif //SUPER_HAXAGON_STRUCTS_HPP
