#ifndef SUPER_HAXAGON_WALL_HPP
#define SUPER_HAXAGON_WALL_HPP

#include <array>

#include "Core/Structs.hpp"

namespace SuperHaxagon {
	class Wall {
	public:

		//Overflow so you don't get glitched lines between hexagons.
		//This is really just some arbitrary number so yeah...
		static constexpr double WALL_OVERFLOW = TAU/1200.0;

		Wall(double distance, double height, int side);

		void advance(double speed);
		Movement collision(double cursorHeight, double cursorPos, double cursorStep, int sides) const;
		std::array<Point, 4> calcPoints(const Point& focus, double rotation, double sides, double offset, double scale) const;
		static Point calcPoint(const Point& focus, double rotation, double overflow, double distance, double sides, int side);

		double getDistance() const {return _distance;}
		double getHeight() const {return _height;}
		int getSide() const {return _side;}

	private:
		double _distance;
		double _height;
		int _side;
	};

	class WallFactory {
	public:
		static constexpr int MIN_WALL_HEIGHT = 4;

		WallFactory(std::ifstream& file, int maxSides);

		Wall instantiate(double offsetDistance, int offsetSide, int sides) const;

	private:
		uint16_t _distance = 0;
		uint16_t _height = 0;
		uint16_t _side = 0;
	};
}

#endif //SUPER_HAXAGON_WALL_HPP
