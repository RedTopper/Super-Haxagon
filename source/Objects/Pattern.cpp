// SPDX-FileCopyrightText: 2021 AJ Walter
// SPDX-License-Identifier: GPL-3.0

#include "Objects/Pattern.hpp"

#include <algorithm>

namespace SuperHaxagon {
	Pattern::Pattern(std::vector<Wall>& walls, const int sides) : _walls(std::move(walls)), _sides(sides) {}

	float Pattern::getFurthestWallDistance() const {
		const auto furthest = std::max_element(_walls.begin(), _walls.end(), [](const auto& a, const auto& b) {
			// true if first is less than second
			return a.getDistance() + a.getHeight() < b.getDistance() + b.getHeight();
		});

		return furthest->getDistance() + furthest->getHeight();
	}

	float Pattern::getClosestWallDistance() const {
		return std::min_element(_walls.begin(), _walls.end(), [](const auto& a, const auto& b) {
			// true if a is less than b
			return a.getDistance() < b.getDistance();
		})->getDistance();
	}

	void Pattern::advance(const float speed) {
		for(auto& wall : _walls) {
			wall.advance(speed);
		}
	}
}
