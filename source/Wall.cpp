#include "Wall.h"
#include "Pattern.h"

namespace SuperHaxagon {
	WallActive::WallActive(double distance, double height, int side) :
		distance(distance),
		height(height),
		side(side)
	{}

	Wall::Wall(short distance, short height, short side) :
		distance(distance),
		height(height),
		side(side)
	{}

	WallActive Wall::instantiate(const Pattern& pattern, double offsetDistance, int offsetSide) const {
		int newSide = side + offsetSide;
		newSide = newSide > pattern.getSides() ? newSide - pattern.getSides() : newSide;
		double newDistance = distance + offsetDistance;
		double newHeight = height;
		return {newDistance, newHeight, newSide};
	}
}
