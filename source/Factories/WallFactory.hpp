#ifndef SUPER_HAXAGON_WALL_FACTORY_HPP
#define SUPER_HAXAGON_WALL_FACTORY_HPP

// SPDX-FileCopyrightText: 2021 AJ Walter
// SPDX-License-Identifier: GPL-3.0-or-later

#include "Core/Structs.hpp"
#include "Objects/Wall.hpp"

#include <vector>

namespace SuperHaxagon {
	class WallFactory {
	public:
		static constexpr float MIN_WALL_HEIGHT = 0.01f;

		WallFactory(std::istream& stream, int maxSides);

		Wall instantiate(float offsetDistance, int offsetSide, int sides) const;

	private:
		float _distance = 0;
		float _height = 0;
		uint16_t _side = 0;
	};
}

#endif //SUPER_HAXAGON_WALL_FACTORY_HPP
