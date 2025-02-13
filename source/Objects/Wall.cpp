#include "Objects/Wall.hpp"

#include <cmath>

namespace SuperHaxagon {
	Wall::Wall(const float distance, const float height, const int side) :
		_distance(distance),
		_height(height),
		_side(side)
	{}

	void Wall::advance(const float speed) {
		_distance -= speed;
	}

	Movement Wall::collision(const float cursorHeight, const float cursorPos, const float cursorStep, const int sides) const {

		// Check if we are between the wall vertically
		if(cursorHeight < _distance || cursorHeight > _distance + _height) {
			return Movement::CAN_MOVE;
		}

		const auto leftRotStep = cursorPos - cursorStep;
		const auto rightRotStep = cursorPos + cursorStep;

		// If the cursor wrapped and the range we need to calculate overflows beyond TAU we also need to check the other equivalent regions:
		// exactly one TAU ago and the next TAU.
		// This is particularly useful when the cursor's next step is beyond a TAU or below zero, OR a wall resides along "the seam"
		const auto leftSideRads = (static_cast<float>(_side) + 1.0f) * TAU/ static_cast<float>(sides);
		const auto leftSideRadsNextTau = leftSideRads + TAU;
		const auto leftSideRadsLastTau = leftSideRads - TAU;
		const auto rightSideRads = static_cast<float>(_side) * TAU/ static_cast<float>(sides);
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

	std::vector<Vec2f> Wall::calcPoints(const float sides, const float offset) const {
		auto tHeight = _height;
		auto tDistance = _distance + offset;
		if(tDistance < SCALE_HEX_LENGTH) {//so the distance is never negative as it enters.
			tHeight -= SCALE_HEX_LENGTH - tDistance;
			tDistance = SCALE_HEX_LENGTH; //Should never be 0!!!
		}

		std::vector<Vec2f> quad{
			calcPoint(-WALL_OVERFLOW, tDistance, sides, _side),
			calcPoint(-WALL_OVERFLOW, tDistance + tHeight, sides, _side),
			calcPoint(WALL_OVERFLOW, tDistance + tHeight, sides, _side + 1),
			calcPoint(WALL_OVERFLOW, tDistance, sides, _side + 1)
		};

		return quad;
	}

	Vec2f Wall::calcPoint(const float overflow, const float distance, const float sides, const int side) {
		Vec2f point = {0,0};
		auto width = static_cast<float>(side) * TAU/sides + overflow;
		if(width > TAU + WALL_OVERFLOW) width = TAU + WALL_OVERFLOW;
		point.x = distance * std::cos(width);
		point.y = distance * std::sin(width + PI);
		return point;
	}
}
