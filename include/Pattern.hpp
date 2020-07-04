#ifndef SUPER_HAXAGON_PATTERN_HPP
#define SUPER_HAXAGON_PATTERN_HPP

#include <vector>
#include <string>

#include "Wall.hpp"

namespace SuperHaxagon {
	class Twist;
	class Pattern {
	public:
		Pattern(std::vector<Wall> walls, int sides);

		const std::vector<Wall>& getWalls() const {return walls;}
		int getSides() const {return sides;}

		double getFurthestWallDistance() const;
		void advance(double speed);

	private:
		std::vector<Wall> walls;
		int sides;
	};

	class PatternFactory {
	public:
		Pattern instantiate(Twist& rng, double distance) const;

		const std::vector<WallFactory>& getWalls() const {return walls;}
		const std::string& getName() const {return name;}
		int getSides() const {return sides;}

	private:
		std::string name;
		std::vector<WallFactory> walls;
		int sides;
	};
}

#endif //SUPER_HAXAGON_PATTERN_HPP
