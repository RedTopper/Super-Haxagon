#include <cmath>
#include <string>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <iostream>

#include "Core/Structs.hpp"

namespace SuperHaxagon {
	Color interpolateColor(const Color& one, const Color& two, const double percent) {
		Color result{};
		result.r = static_cast<uint8_t>(linear(one.r, two.r, percent));
		result.g = static_cast<uint8_t>(linear(one.g, two.g, percent));
		result.b = static_cast<uint8_t>(linear(one.b, two.b, percent));
		result.a = static_cast<uint8_t>(linear(one.a, two.a, percent));
		return result;
	}

	double linear(const double start, const double end, const double percent) {
		return (end - start) * percent + start;
	}

	Point rotateAroundOrigin(const Point& point, double rotation) {
		const auto s = sin(rotation);
		const auto c = cos(rotation);
		Point ret{point.x * c - point.y * s,  point.x * s + point.y * c};
		return ret;
	}

	std::string getTime(const double score) {
		std::stringstream buffer;
		const auto scoreInt = static_cast<int>(score / 60.0);
		const auto decimalPart = static_cast<int>((score / 60.0 - scoreInt) * 100.0);
		buffer << std::fixed << std::setfill('0')
			<< std::setprecision(3) << std::setw(3)
			<< scoreInt
			<< ":"
			<< std::setprecision(2) << std::setw(2)
			<< decimalPart;
		return buffer.str();
	}

	double getPulse(double frame, const double range, const double start) {
		frame -= start;
		// Alternate algorithm:
		//double percent = sin((double)frame * <speed>) / 2.0 + 0.5;
		return fabs(((frame - floor(frame / (range * 2)) * range * 2) - range) / range);
	}

	const char* getScoreText(const int score, const bool reduced) {
		if (reduced) {
			if (score < 10 * 60) return "SPACE";
			if (score < 20 * 60) return "POINT";
			if (score < 30 * 60) return "LINE";
			if (score < 40 * 60) return "TRI";
			if (score < 50 * 60) return "QUAD";
			if (score < 60 * 60) return "PENTA";
			return "HEXA";
		}
		
		if (score < 10 * 60) return "SPACE";
		if (score < 20 * 60) return "POINT";
		if (score < 30 * 60) return "LINE";
		if (score < 40 * 60) return "TRIANGLE";
		if (score < 50 * 60) return "SQUARE";
		if (score < 60 * 60) return "PENTAGON";
		return "HEXAGON";
	}

	void warn(const std::string& where, const std::string& message) {
		std::cerr << ("[warn] '" + where + "': " + message) << std::endl;
	}

	bool readCompare(std::ifstream& file, const std::string& str) {
		const auto read = std::make_unique<char[]>(str.length() + 1);
		file.read(read.get(), str.length());
		read[str.length()] = '\0';
		return read.get() == str;
	}

	uint32_t read32(std::ifstream& file, const uint32_t min, const uint32_t max, const std::string& noun) {
		uint32_t num;
		file.read(reinterpret_cast<char*>(&num), sizeof(num));

		if (num < min) {
			num = min;
			warn("int", noun + " is too small, but continuing anyway.");
		}

		if (num > max) {
			num = max;
			warn("int", noun + " is too large, but continuing anyway.");
		}

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
		const size_t length = read32(file, 1, 300, noun + " string");
		const auto read = std::make_unique<char[]>(length + 1);
		file.read(read.get(), length);
		read[length] = '\0';
		return std::string(read.get());
	}

	void writeString(std::ofstream& file, const std::string& str) {
		auto len = static_cast<uint32_t>(str.length());
		file.write(reinterpret_cast<char*>(&len), sizeof(len));
		file.write(str.c_str(), str.length());
	}
}