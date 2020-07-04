#include "Level.hpp"
#include "Twist.hpp"
#include "Pattern.hpp"

namespace SuperHaxagon {
	Level::Level(const LevelFactory& factory, Twist& rng, int renderDistance) : factory(factory) {
		nextIndexBG1 = (factory.getColorsBG1().size() > 1 ? 1 : 0);
		nextIndexBG2 = (factory.getColorsBG2().size() > 1 ? 1 : 0);
		nextIndexFG = (factory.getColorsFG().size() > 1 ? 1 : 0);

		//fetch some random starting patterns
		auto distance = (double)renderDistance;
		for(int i = 0;  i < TOTAL_PATTERNS_AT_ONE_TIME;  i++) {
			int pCount = factory.getPatterns().size();
			int pSelected = rng.rand(pCount);
			auto& pattern = factory.getPatterns()[pSelected];
			patterns.emplace_back(pattern->instantiate(rng, distance));
			distance = patterns.back()->getFurthestWallDistance();
		}

		//set up the amount of sides the level should have.
		lastSides = patterns.front()->getSides();
		currentSides = patterns.front()->getSides();
		cursorPos = TAU/4.0 + (factory.getSpeedCursor() / 2.0);
	}

	void Level::update(Twist& rng, int hexLength) {
		// Update color frame and clamp
		tweenFrame++;
		if(tweenFrame >= factory.getSpeedPulse()) {
			tweenFrame = 0;
			indexBG1 = nextIndexBG1;
			indexBG2 = nextIndexBG2;
			indexFG = nextIndexFG;
			nextIndexBG1 = (indexBG1 + 1 < factory.getColorsBG1().size() ? indexBG1 + 1 : 0);
			nextIndexBG2 = (indexBG2 + 1 < factory.getColorsBG2().size() ? indexBG2 + 1 : 0);
			nextIndexFG = (indexFG + 1 < factory.getColorsFG().size() ? indexFG + 1 : 0);
		}

		// Bring walls forward if we are not delaying
		// Otherwise tween from one shape to another.
		if(delayFrame == 0) {
			sidesTween = currentSides;
			for(auto& pattern : patterns) {
				pattern->advance(factory.getSpeedWall());
			}
		} else {
			double percent = (double)(delayFrame) / (double)(FRAMES_PER_CHANGE_SIDE * abs(currentSides - lastSides));
			sidesTween = linear((double)currentSides, (double)lastSides, percent);
			delayFrame--;
		}

		// Shift patterns forward
		if(patterns.front()->getFurthestWallDistance() < hexLength) {
			lastSides = patterns.front()->getSides();
			patterns.pop_front();

			int pCount = factory.getPatterns().size();
			int pSelected = rng.rand(pCount);
			auto& pattern = factory.getPatterns()[pSelected];
			patterns.emplace_back(pattern->instantiate(rng, patterns.back()->getFurthestWallDistance()));

			currentSides = patterns.front()->getSides();

			// Delay the level if the shifted pattern does  not have the same sides as the last.
			if(lastSides != currentSides) {
				delayFrame = FRAMES_PER_CHANGE_SIDE * abs(currentSides - lastSides);
			}
		}

		// Rotate level
		rotation += factory.getSpeedRotation() * multiplier;
		if(rotation >= TAU) rotation -= TAU;
		if(rotation < 0) rotation  += TAU;

		// Flip level if needed
		flipFrame--;
		if(flipFrame == 0) {
			multiplier *= -1.0;
			flipFrame = rng.rand(FLIP_FRAMES_MIN, FLIP_FRAMES_MAX);
		}
	}

	Movement Level::collision() const {
		auto collision = Movement::CAN_MOVE;
		auto cursorDistance = game.getHexLength() + game.getHumanPadding() + game.getHumanHeight();

		// For all patterns
		// TODO: Do I really need to do this?
		for(const auto& pattern : patterns) {

			// For all walls
			for(const auto& wall : pattern.getWalls()) {
				Movement check = wall.collision(cursorDistance, cursorPos, level.getSpeedCursor(), pattern.getSides());

				// Update collision
				if(collision == Movement::CAN_MOVE) collision = check; //If we can move, try and replace it with something else
				if(check == Movement::DEAD)  { //If we are ever dead, return it.
					return Movement::DEAD;
				}
			}
		}

		return collision;
	}

	void Level::increaseMultiplier() {
		multiplier *= DIFFICULTY_MULTIPLIER;
	}

	void Level::left() {
		cursorPos += factory.getSpeedCursor();
	}

	void Level::right() {
		cursorPos -= factory.getSpeedCursor();
	}

	void Level::clamp() {
		if(cursorPos >= TAU) cursorPos -= TAU;
		if(cursorPos < 0) cursorPos  += TAU;
	}

	std::unique_ptr<Level> LevelFactory::instantiate(Twist& rng, int renderDistance) {
		return std::make_unique<Level>(*this, rng, renderDistance);
	}
}