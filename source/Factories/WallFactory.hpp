#ifndef SUPER_HAXAGON_WALL_FACTORY_HPP
#define SUPER_HAXAGON_WALL_FACTORY_HPP

#include "Core/Structs.hpp"
#include "Objects/Wall.hpp"

#include <vector>

namespace SuperHaxagon {
	class WallFactory {
	public:
		static constexpr int MIN_WALL_HEIGHT = 4;

		WallFactory(std::istream& stream, int maxSides);

		Wall instantiate(float offsetDistance, int offsetSide, int sides) const;

	private:
		uint16_t _distance = 0;
		uint16_t _height = 0;
		uint16_t _side = 0;
	};
}

#endif //SUPER_HAXAGON_WALL_FACTORY_HPP
