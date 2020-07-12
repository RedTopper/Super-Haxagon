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

		const std::vector<std::unique_ptr<Wall>>& getWalls() const {return _walls;}
		int getSides() const {return _sides;}

		double getFurthestWallDistance() const;
		void advance(double speed);

	private:
		std::vector<std::unique_ptr<Wall>> _walls;
		int _sides;
	};

	class PatternFactory {
	public:
		static const char* PATTERN_HEADER;
		static const char* PATTERN_FOOTER;
		static constexpr int MIN_PATTERN_SIDES = 3;

		PatternFactory(std::ifstream& file, Platform& platform);
		~PatternFactory();

		std::unique_ptr<Pattern> instantiate(Twist& rng, double distance) const;

		bool isLoaded() const {return _loaded;}

		std::string getName() const {return _name;}

	private:
		std::vector<std::unique_ptr<WallFactory>> _walls;
		std::string _name  = "";
		int _sides = 0;
		bool _loaded = false;
	};
}

#endif //SUPER_HAXAGON_PATTERN_HPP
