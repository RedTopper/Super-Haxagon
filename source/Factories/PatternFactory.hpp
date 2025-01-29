#ifndef SUPER_HAXAGON_PATTERN_FACTORY_HPP
#define SUPER_HAXAGON_PATTERN_FACTORY_HPP

#include "Factories/WallFactory.hpp"
#include "Objects/Pattern.hpp"

#include <vector>
#include <string>

namespace SuperHaxagon {
	class Twist;
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

#endif //SUPER_HAXAGON_PATTERN_FACTORY_HPP
