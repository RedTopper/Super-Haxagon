#include <cmath>

#include "Factories/Wall.hpp"

namespace SuperHaxagon {
	Wall::Wall(double distance, double height, int side) :
		distance(distance),
		height(height),
		side(side)
	{}

	void Wall::advance(double speed) {
		distance -= speed;
	}

	Movement Wall::collision(double cursorHeight, double cursorPos, double cursorStep, int sides) const {

		// Check if we are between the wall vertically
		if(cursorHeight < distance || cursorHeight > distance + height) {
			return Movement::CAN_MOVE;
		}

		double leftRotStep = cursorPos + cursorStep;
		double rightRotStep = cursorPos - cursorStep;

		// If the cursor wrapped and the range we need to calculate overflows beyond TAU we also need to check the other equivalent regions:
		// exactly one TAU ago and the next TAU.
		// This is particularly useful when the cursor's next step is beyond a TAU or below zero, OR a wall resides along "the seam"
		double leftSideRads = ((double)(side) + 1.0) * TAU/(double)(sides);
		double leftSideRadsNextTau = leftSideRads + TAU;
		double leftSideRadsLastTau = leftSideRads - TAU;
		double rightSideRads = (double)(side) * TAU/(double)(sides);
		double rightSideRadsNextTau = rightSideRads + TAU;
		double rightSideRadsLastTau = rightSideRads - TAU;

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

	std::array<Point, 4> Wall::calcPoints(const Point& focus, double rotation, double sides, double offset, double scale) const {
		std::array<Point, 4> quad{};
		double tHeight = height;
		double tDistance = distance + offset;
		if(tDistance < SCALE_HEX_LENGTH - 2.0) {//so the distance is never negative as it enters.
			tHeight -= SCALE_HEX_LENGTH - 2.0 - tDistance;
			tDistance = SCALE_HEX_LENGTH - 2.0; //Should never be 0!!!
		}

		tDistance *= scale;
		tHeight *= scale;
		quad[0] = calcPoint(focus, rotation, WALL_OVERFLOW, tDistance, sides, side + 1);
		quad[1] = calcPoint(focus, rotation, WALL_OVERFLOW, tDistance + tHeight, sides, side + 1);
		quad[2] = calcPoint(focus, rotation, -WALL_OVERFLOW, tDistance + tHeight, sides, side);
		quad[3] = calcPoint(focus, rotation, -WALL_OVERFLOW, tDistance, sides, side);
		return quad;
	}

	Point Wall::calcPoint(const Point& focus, double rotation, double overflow, double distance, double sides, int side) {
		Point point = {0,0};
		double width = (double)side * TAU/sides + overflow;
		if(width > TAU + WALL_OVERFLOW) width = TAU + WALL_OVERFLOW;
		point.x = distance * std::cos(rotation + width) + (double)(focus.x);
		point.y = distance * std::sin(rotation + width) + (double)(focus.y);
		return point;
	}


	WallFactory::WallFactory(std::ifstream& file, int maxSide) {
		distance = read16(file);
		height = read16(file);
		side = read16(file);

		if(height < MIN_WALL_HEIGHT) height = MIN_WALL_HEIGHT;
		if(side >= maxSide) side = maxSide - 1;
	}

	std::unique_ptr<Wall> WallFactory::instantiate(double offsetDistance, int offsetSide, int sides) const {
		int newSide = side + offsetSide;
		newSide = newSide >= sides ? newSide - sides : newSide;
		double newDistance = distance + offsetDistance;
		double newHeight = height;
		return std::make_unique<Wall>(newDistance, newHeight, newSide);
	}
}
