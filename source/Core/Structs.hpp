#ifndef SUPER_HAXAGON_STRUCTS_HPP
#define SUPER_HAXAGON_STRUCTS_HPP

#include "Vector.hpp"

#include <cstdint>
#include <fstream>
#include <string>

namespace SuperHaxagon {
	class Platform;

	struct Color {
		uint8_t r;
		uint8_t g;
		uint8_t b;
		uint8_t a;
	};

	enum class Movement {
		CAN_MOVE,
		CANNOT_MOVE_LEFT,
		CANNOT_MOVE_RIGHT,
		DEAD,
	};

	enum class LocColor {
		FG = 0,
		BG1,
		BG2,
		LAST // Unused, but used for iteration
	};

	enum class SoundEffect {
		BEGIN,
		HEXAGON,
		OVER,
		SELECT,
		LEVEL_UP,
		WONDERFUL,
	};

	static constexpr int COLOR_LOCATION_FIRST = static_cast<int>(LocColor::FG);
	static constexpr int COLOR_LOCATION_LAST = static_cast<int>(LocColor::LAST);

	// This converts the old "pixel based" levels to the new "screen space" coordinates
	static constexpr float SCALE_TO_SCREEN_SPACE = (2.0f/400.0f);

	static constexpr float ASPECT_MIN = (5.0f / 4.0f);
	static constexpr float ASPECT_DEFAULT = (400.0f / 240.0f);
	static constexpr float ASPECT_MAX = (21.0f / 9.0f);

	static constexpr float BASE_DISTANCE = 2.0f;
	static constexpr float HEX_LENGTH = 22.0f * SCALE_TO_SCREEN_SPACE;
	static constexpr float HEX_BORDER = 4.0f * SCALE_TO_SCREEN_SPACE;
	static constexpr float PLAYER_PADDING_BETWEEN_HEX = 5.0f * SCALE_TO_SCREEN_SPACE;
	static constexpr float PLAYER_TRI_HEIGHT = 5.0f * SCALE_TO_SCREEN_SPACE;
	static constexpr float PLAYER_TRI_WIDTH = 5.0f * SCALE_TO_SCREEN_SPACE;

	static const Color COLOR_SHADOW = {0, 0, 0, 0xC0};
	static const Color COLOR_TRANSPARENT = {0, 0, 0, 0xA0};
	static const Color COLOR_GREY = {0xA0, 0xA0, 0xA0, 0xFF};
	static const Color COLOR_WHITE =  {0xFF, 0xFF, 0xFF, 0xFF};
	static const Color COLOR_BLACK =  {0, 0, 0, 0xFF};

	static const Color PULSE_LOW = {0xFF, 0xFF, 0xFF, 0x7F};
	static const Color PULSE_HIGH = {0xFF, 0xFF, 0xFF, 0xFF};

	struct GameColors {
		Color fg = COLOR_BLACK;
		Color bg1 = COLOR_BLACK;
		Color bg2 = COLOR_BLACK;

		Color operator[](LocColor loc) const {
			switch (loc) {
				case LocColor::FG: return fg;
				case LocColor::BG1: return bg1;
				case LocColor::BG2: return bg2;
				default: return COLOR_BLACK;
			}
		}
	};

	/**
	 * Linear interpolation between two colors
	 */
	Color interpolateColor(const Color& one, const Color& two, float percent);

	/**
	 * Rotates a color n degrees
	 */
	Color rotateColor(const Color& in, float degrees);

	/**
	 * Linear interpolation between two floats
	 */
	float linear(float start, float end, float percent);

	/**
	 * Rotates a cartesian point around the origin
	 */
	Vec2f rotateAroundOrigin(const Vec2f& point, float rotation);

	/**
	 * Converts score into a string
	 *
	 * TTT:%% where  TTT is the current time in seconds, and %% is  the percent
	 * through the current second.
	 */
	std::string getTime(float score);

	/**
	 * Will pulse between 0.0 and 1.0 at the speed given (in tenths of a second).
	 * start is when the pulse should (have) start(ed).
	 */
	float getPulse(float frame, float range, float start);

	/**
	 * Gets the current level that the score represents (for example, point)
	 * if the user is less than 10 seconds into the game)
	 */
	const char* getScoreText(int score, bool reduced);

	/**
	 * Compares a fixed length string to an expected string in a file.
	 * (useful for checking both headers and footers)
	 */
	bool readCompare(std::istream& stream, const std::string& str);

	/**
	 * Reads an integer from a file advancing its internal pointer
	 */
	int32_t read32(std::istream& stream, int32_t min, int32_t max, Platform& platform, const std::string& noun);

	/**
	 * Reads a short from a file advancing its internal pointer
	 */
	int16_t read16(std::istream& stream);

	/**
	 * Read a float from a file advancing its internal pointer
	 */
	float readFloat(std::istream& stream);

	/**
	 * Reads a color from a file advancing its internal pointer
	 */
	Color readColor(std::istream& stream);

	/**
	 * Reads a string from the binary file
	 */
	std::string readString(std::istream& stream, Platform& platform, const std::string& noun);

	/**
	 * Writes a string with a length to a binary file
	 */
	void writeString(std::ostream& stream, const std::string& str);
}

#endif //SUPER_HAXAGON_STRUCTS_HPP
