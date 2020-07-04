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

	bool readCompare(std::ifstream& file, std::string str) {

	}
}