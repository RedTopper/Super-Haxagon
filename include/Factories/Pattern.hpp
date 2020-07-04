#ifndef SUPER_HAXAGON_PATTERN_HPP
#define SUPER_HAXAGON_PATTERN_HPP

#include <vector>
#include <string>
#include <memory>

namespace SuperHaxagon {
	class Twist;
	class Wall;
	class WallFactory;

	class Pattern {
	public:
		Pattern(std::vector<std::unique_ptr<Wall>> walls, int sides);

		const std::vector<std::unique_ptr<Wall>>& getWalls() const {return walls;}
		int getSides() const {return sides;}

		double getFurthestWallDistance() const;
		void advance(double speed);

	private:
		std::vector<std::unique_ptr<Wall>> walls;
		int sides;
	};

	class PatternFactory {
	public:
		explicit PatternFactory(std::ifstream& file);
		~PatternFactory();

		std::unique_ptr<Pattern> instantiate(Twist& rng, double distance) const;

		std::string getName() const {return name;}

	private:
		std::vector<std::unique_ptr<WallFactory>> walls;
		std::string name  = "";
		int sides = 0;
	};
}

#endif //SUPER_HAXAGON_PATTERN_HPP
