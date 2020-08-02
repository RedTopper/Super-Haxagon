#include "Factories/Wall.hpp"

#include <cmath>

namespace SuperHaxagon {
	Wall::Wall(const double distance, const double height, const int side) :
		_distance(distance),
		_height(height),
		_side(side)
	{}

	void Wall::advance(const double speed) {
		_distance -= speed;
	}

	Movement Wall::collision(const double cursorHeight, const double cursorPos, const double cursorStep, const int sides) const {

		// Check if we are between the wall vertically
		if(cursorHeight < _distance || cursorHeight > _distance + _height) {
			return Movement::CAN_MOVE;
		}

		const auto leftRotStep = cursorPos + cursorStep;
		const auto rightRotStep = cursorPos - cursorStep;

		// If the cursor wrapped and the range we need to calculate overflows beyond TAU we also need to check the other equivalent regions:
		// exactly one TAU ago and the next TAU.
		// This is particularly useful when the cursor's next step is beyond a TAU or below zero, OR a wall resides along "the seam"
		const auto leftSideRads = (_side + 1.0) * TAU/sides;
		const auto leftSideRadsNextTau = leftSideRads + TAU;
		const auto leftSideRadsLastTau = leftSideRads - TAU;
		const auto rightSideRads = _side * TAU/sides;
		const auto rightSideRadsNextTau = rightSideRads + TAU;
		const auto rightSideRadsLastTau = rightSideRads - TAU;

		//Check if we are between the wall horizontally.
		if(cursorPos >= rightSideRads && cursorPos <= leftSideRads) {
			return Movement::DEAD;
		}

		if((leftRotStep > rightSideRads && leftRotStep < leftSideRads) ||
		   (leftRotStep > rightSideRadsNextTau && leftRotStep < leftSideRadsNextTau) ||
		   (leftRotStep > rightSideRadsLastTau && leftRotStep < leftSideRadsLastTau))  {
			return Movement::CANNOT_MOVE_LEFT;
		}

		if((rightRotStep < leftSideRads && rightRotStep > rightSideRads) ||
		   (rightRotStep < leftSideRadsNextTau && rightRotStep > rightSideRadsNextTau) ||
		   (rightRotStep < leftSideRadsLastTau && rightRotStep > rightSideRadsLastTau)) {
			return Movement::CANNOT_MOVE_RIGHT;
		}

		return Movement::CAN_MOVE;
	}

	std::vector<Point> Wall::calcPoints(const Point& focus, const double rotation, const double sides, const double offset, const double scale) const {
		
		auto tHeight = _height;
		auto tDistance = _distance + offset;
		if(tDistance < SCALE_HEX_LENGTH) {//so the distance is never negative as it enters.
			tHeight -= SCALE_HEX_LENGTH - tDistance;
			tDistance = SCALE_HEX_LENGTH; //Should never be 0!!!
		}

		tDistance *= scale;
		tHeight *= scale;
		std::vector<Point> quad{
			calcPoint(focus, rotation, -WALL_OVERFLOW, tDistance, sides, _side),
			calcPoint(focus, rotation, -WALL_OVERFLOW, tDistance + tHeight, sides, _side),
			calcPoint(focus, rotation, WALL_OVERFLOW, tDistance + tHeight, sides, _side + 1),
			calcPoint(focus, rotation, WALL_OVERFLOW, tDistance, sides, _side + 1)
		};

		return quad;
	}

	Point Wall::calcPoint(const Point& focus, const double rotation, const double overflow, const double distance, const double sides, const int side) {
		Point point = {0,0};
		auto width = side * TAU/sides + overflow;
		if(width > TAU + WALL_OVERFLOW) width = TAU + WALL_OVERFLOW;
		point.x = distance * std::cos(rotation + width) + focus.x;
		point.y = distance * std::sin(rotation + width + PI) + focus.y;
		return point;
	}


	WallFactory::WallFactory(std::ifstream& file, const int maxSides) {
		_distance = read16(file);
		_height = read16(file);
		_side = read16(file);

		if(_height < MIN_WALL_HEIGHT) _height = MIN_WALL_HEIGHT;
		if(_side >= maxSides) _side = maxSides - 1;
	}

	Wall WallFactory::instantiate(const double offsetDistance, const int offsetSide, const int sides) const {
		auto newSide = _side + offsetSide;
		newSide = newSide >= sides ? newSide - sides : newSide;
		const auto newDistance = _distance + offsetDistance;
		const double newHeight = _height;
		return {newDistance, newHeight, newSide};
	}
}
