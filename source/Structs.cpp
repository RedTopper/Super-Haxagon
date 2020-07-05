#include <cmath>
#include <string>
#include <sstream>
#include <iomanip>

#include "Structs.hpp"

namespace SuperHaxagon {
	Color interpolateColor(const Color& one, const Color& two, double percent) {
		Color result{};
		result.r = (int)linear((double)one.r, (double)two.r, percent);
		result.g = (int)linear((double)one.g, (double)two.g, percent);
		result.b = (int)linear((double)one.b, (double)two.b, percent);
		result.a = (int)linear((double)one.a, (double)two.a, percent);
		return result;
	}

	double linear(double start, double end, double percent) {
		return (end - start) * percent + start;
	}

	Point calcPoint(const Point& focus, double rotation, double offset, double distance) {
		Point point = {0,0};
		point.x = lround(distance * cos(rotation + offset) + focus.x);
		point.y = lround(distance * sin(rotation + offset) + focus.y);
		return point;
	}

	std::string getTime(int score) {
		std::stringstream buffer;
		int scoreInt = (int)((double)score/60.0);
		int decimalPart = (int)(((double)score/60.0 - (double)scoreInt) * 100.0);
		buffer << std::fixed << std::setprecision(3) << scoreInt << std::setprecision(2) << decimalPart;
		return buffer.str();
	}

	double getPulse(int frame, int range, int start) {
		frame -= start;
		// Alternate algorithm:
		//double percent = sin((double)frame * <speed>) / 2.0 + 0.5;
		return fabs(((double)(frame % range * 2) - (double)range) / (double)range);
	}

	const char* getScoreText(int score) {
		if(score < 10 * 60) return "SPACE";
		if(score < 20 * 60) return "POINT";
		if(score < 30 * 60) return "LINE";
		if(score < 40 * 60) return "TRIANGLE";
		if(score < 50 * 60) return "SQUARE";
		if(score < 60 * 60) return "PENTAGON";
		return "HEXAGON";
	}

	std::runtime_error malformed(const std::string& where, const std::string& message) {
		return std::runtime_error("[malformed] '" + where + "': " + message);
	}

	bool readCompare(std::ifstream& file, const std::string& str) {
		char read[str.length() + 1];
		file.read(read, str.length());
		read[str.length()] = '\0';
		return read == str;
	}

	uint32_t read32(std::ifstream& file, uint32_t min, uint32_t max, const std::string& noun) {
		uint32_t num;
		file.read(reinterpret_cast<char*>(&num), sizeof(num));
		if (num < min) throw malformed("int", noun + " is too small!");
		if (num > max) throw malformed("int", noun + " is too large!");
		return num;
	}

	uint16_t read16(std::ifstream& file) {
		uint16_t num;
		file.read(reinterpret_cast<char*>(&num), sizeof(num));
		return num;
	}

	float readFloat(std::ifstream& file) {
		float num;
		file.read(reinterpret_cast<char*>(&num), sizeof(num));
		return num;
	}

	Color readColor(std::ifstream& file) {
		Color color{};
		file.read(reinterpret_cast<char*>(&color.r), sizeof(color.r));
		file.read(reinterpret_cast<char*>(&color.g), sizeof(color.g));
		file.read(reinterpret_cast<char*>(&color.b), sizeof(color.b));
		color.a = 0xFF;
		return color;
	}

	std::string readString(std::ifstream& file, const std::string& noun) {
		int length = read32(file, 1, 300, noun + " string");
		char read[length + 1];
		file.read(read, length);
		read[length] = '\0';
		return std::string(read);
	}
}