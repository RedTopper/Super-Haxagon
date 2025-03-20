// SPDX-FileCopyrightText: 2021 AJ Walter
// SPDX-License-Identifier: GPL-3.0-or-later

#include "Factories/WallFactory.hpp"

namespace SuperHaxagon {
	WallFactory::WallFactory(std::istream& stream, const int maxSides) {
		// To keep compatability with old levels
		_distance = static_cast<float>(read16(stream)) * SCALE_TO_SCREEN_SPACE;
		_height = static_cast<float>(read16(stream)) * SCALE_TO_SCREEN_SPACE;
		_side = read16(stream);

		if(_height < MIN_WALL_HEIGHT) _height = MIN_WALL_HEIGHT;
		if(_side >= maxSides) _side = static_cast<uint16_t>(maxSides) - 1;
	}

	Wall WallFactory::instantiate(const float offsetDistance, const int offsetSide, const int sides) const {
		auto newSide = _side + offsetSide;
		newSide = newSide >= sides ? newSide - sides : newSide;
		const auto newDistance = static_cast<float>(_distance) + offsetDistance;
		const float newHeight = _height;
		return {newDistance, newHeight, newSide};
	}
}
