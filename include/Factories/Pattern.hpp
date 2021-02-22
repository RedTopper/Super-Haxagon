#ifndef SUPER_HAXAGON_PATTERN_HPP
#define SUPER_HAXAGON_PATTERN_HPP

#include "Factories/Wall.hpp"

#include <vector>
#include <string>

namespace SuperHaxagon {
	class Twist;
	class Pattern {
	public:
		Pattern(std::vector<Wall>& walls, int sides);

		const std::vector<Wall>& getWalls() const {return _walls;}
		int getSides() const {return _sides;}

		float getFurthestWallDistance() const;
		float getClosestWallDistance() const;
		void advance(float speed);

	private:
		std::vector<Wall> _walls;
		int _sides;
	};

	class PatternFactory {
	public:
		static const char* PATTERN_HEADER;
		static const char* PATTERN_FOOTER;
		static constexpr int MIN_PATTERN_SIDES = 3;

		PatternFactory(std::istream& stream, Platform& platform);
		~PatternFactory();

		Pattern instantiate(Twist& rng, float distance) const;

		bool isLoaded() const {return _loaded;}
		int getSides() const {return _sides;}
		std::string getName() const {return _name;}

	private:
		std::vector<WallFactory> _walls;
		std::string _name;
		int _sides = 0;
		bool _loaded = false;
	};
}

#endif //SUPER_HAXAGON_PATTERN_HPP
