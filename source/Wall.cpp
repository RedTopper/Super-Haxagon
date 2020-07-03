#include "Wall.hpp"
#include "Pattern.hpp"

namespace SuperHaxagon {
	WallActive::WallActive(double distance, double height, int side) :
		distance(distance),
		height(height),
		side(side)
	{}

	void WallActive::advance(double speed) {
		distance -= speed;
	}

	Movement WallActive::collision(double cursorHeight, double cursorPos, double cursorStep, int sides) const {

		//Check if we are between the wall vertically
		if(cursorHeight < distance || cursorHeight > distance + height) {
			return Movement::CAN_MOVE;
		}

		double leftRotStep = cursorPos + cursorStep;
		double rightRotStep = cursorPos - cursorStep;

		//If the cursor wrapped and the range we need to calculate overflows beyond TAU we also need to check the other equivalent regions:
		//exactly one TAU ago and the next TAU.
		//This is particularly usefull when the cursor's next step is beyond a TAU or below zero, OR a wall resides along "the seam"
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
