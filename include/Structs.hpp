#ifndef SUPER_HAXAGON_STRUCTS_HPP
#define SUPER_HAXAGON_STRUCTS_HPP

#include <cmath>
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

	static const Color PULSE_LOW = {0xFF, 0xFF, 0xFF, 0x7F};
	static const Color PULSE_HIGH = {0xFF, 0xFF, 0xFF, 0xFF};



	/**
	 * Linear interpolation between two colors
	 */
	Color interpolateColor(const Color& one, const Color& two, double percent);

	/**
	 * Linear interpolation between two doubles
	 */
	double linear(double start, double end, double percent);

	/**
	 * Calculates a point (either for a wall or a general point) based on the running level's rotation, some offset
	 * so it renders correctly, some distance the point should be from the center, the side the point
	 * should be rendered on, based on a total number of sides the shape has.
	 */
	Point calcPoint(const Point& focus, double rotation, double offset, double distance);

	/**
	 * Converts score into a string
	 *
	 * TTT:%% where  TTT is the current time in seconds, and %% is  the percent
	 * through the current second.
	 */
	std::string getTime(int score);

	/**
	 * Will pulse between 0.0 and 1.0 at the speed given (in tenths of a second).
	 * start is when the pulse should (have) start(ed).
	 */
	double getPulse(int frame, int range, int start);

	/**
	 * Gets the current level that the score represents (for example, point)
	 * if the user is less than 10 seconds into the game)
	 */
	const char* getScoreText(int score);

	/**
	 * Compares a fixed length string to an expected string in a file.
	 * (useful for checking both headers and footers)
	 */
	bool readCompare(std::ifstream& file, const std::string& str);

	/**
	 * Reads an integer from a file advancing its internal pointer
	 */
	int readSize(std::ifstream& file);
}

#endif //SUPER_HAXAGON_STRUCTS_HPP
