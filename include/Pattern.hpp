#ifndef SUPER_HAXAGON_PATTERN_HPP
#define SUPER_HAXAGON_PATTERN_HPP

#include <vector>
#include <string>

#include "Wall.hpp"

namespace SuperHaxagon {
	class Twist;
	class PatternActive {
	public:
		PatternActive(std::vector<WallActive> walls, int sides);

		const std::vector<WallActive>& getWalls() const {return walls;}
		int getSides() const {return sides;}

		double getFurthestWallDistance() const;
		void advance(double speed);

	private:
		std::vector<WallActive> walls;
		int sides;
	};

	class Pattern {
	public:
		PatternActive instantiate(Twist& rng, double distance) const;

		const std::vector<Wall>& getWalls() const {return walls;}
		const std::string& getName() const {return name;}
		int getSides() const {return sides;}

	private:
		std::string name;
		std::vector<Wall> walls;
		int sides;
	};
}

#endif //SUPER_HAXAGON_PATTERN_HPP
