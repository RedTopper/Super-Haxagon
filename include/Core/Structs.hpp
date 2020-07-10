#ifndef SUPER_HAXAGON_STRUCTS_HPP
#define SUPER_HAXAGON_STRUCTS_HPP

#include <cstdint>
#include <stdexcept>

namespace SuperHaxagon {
	struct Color {
		uint8_t r;
		uint8_t g;
		uint8_t b;
		uint8_t a;
	};

	struct Point {
		double x;
		double y;
	};

	enum class Movement {
		CAN_MOVE,
		CANNOT_MOVE_LEFT,
		CANNOT_MOVE_RIGHT,
		DEAD,
	};

	static constexpr double TAU = 3.14159265358979323846 * 2;

	static constexpr double SCALE_BASE_DISTANCE = 300.0;
	static constexpr double SCALE_HEX_LENGTH = 20.0;
	static constexpr double SCALE_HEX_BORDER = 4.0;
	static constexpr double SCALE_HUMAN_PADDING = 5.0;
	static constexpr double SCALE_HUMAN_HEIGHT = 5.0;
	static constexpr double SCALE_PULSE_MAX = 20.0;
	static constexpr double HUMAN_WIDTH_RAD = TAU/30.0;

	static const Color COLOR_SHADOW = {0, 0, 0, 0xC0};
	static const Color COLOR_TRANSPARENT = {0, 0, 0, 0xA0};
	static const Color COLOR_GREY = {0xA0, 0xA0, 0xA0, 0xFF};
	static const Color COLOR_WHITE =  {0xFF, 0xFF, 0xFF, 0xFF};
	static const Color COLOR_BLACK =  {0, 0, 0, 0xFF};
	static const Color COLOR_RED =  {0xFF, 0x60, 0x60, 0xFF};

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
	std::string getTime(double score);

	/**
	 * Will pulse between 0.0 and 1.0 at the speed given (in tenths of a second).
	 * start is when the pulse should (have) start(ed).
	 */
	double getPulse(double frame, double range, double start);

	/**
	 * Gets the current level that the score represents (for example, point)
	 * if the user is less than 10 seconds into the game)
	 */
	const char* getScoreText(int score, bool reduced);

	/**
	 * Creates an exception with
	 */
	std::runtime_error malformed(const std::string& where, const std::string& message);

	/**
	 * Compares a fixed length string to an expected string in a file.
	 * (useful for checking both headers and footers)
	 */
	bool readCompare(std::ifstream& file, const std::string& str);

	/**
	 * Reads an integer from a file advancing its internal pointer
	 */
	uint32_t read32(std::ifstream& file, uint32_t min, uint32_t max, const std::string& noun);

	/**
	 * Reads a short from a file advancing its internal pointer
	 */
	uint16_t read16(std::ifstream& file);

	/**
	 * Read a float from a file advancing its internal pointer
	 */
	float readFloat(std::ifstream& file);

	/**
	 * Reads a color from a file advancing its internal pointer
	 */
	Color readColor(std::ifstream& file);

	/**
	 * Reads a string from the binary file
	 */
	std::string readString(std::ifstream& file, const std::string& noun);

	/**
	 * Writes a string with a length to a binary file
	 */
	void writeString(std::ofstream& file, const std::string& str);
}

#endif //SUPER_HAXAGON_STRUCTS_HPP
