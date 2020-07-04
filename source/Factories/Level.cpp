#include "Factories/Level.hpp"
#include "Factories/Pattern.hpp"
#include "Factories/Wall.hpp"

#include "Twist.hpp"
#include "Game.hpp"

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

	Level::~Level() = default;

	void Level::update(Twist& rng, int hexLength) {
		// Update color frame and clamp
		tweenFrame++;
		if(tweenFrame >= factory.getSpeedPulse()) {
			tweenFrame = 0;
			indexBG1 = nextIndexBG1;
			indexBG2 = nextIndexBG2;
			indexFG = nextIndexFG;
			nextIndexBG1 = (indexBG1 + 1 < (int)factory.getColorsBG1().size() ? indexBG1 + 1 : 0);
			nextIndexBG2 = (indexBG2 + 1 < (int)factory.getColorsBG2().size() ? indexBG2 + 1 : 0);
			nextIndexFG = (indexFG + 1 < (int)factory.getColorsFG().size() ? indexFG + 1 : 0);
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

	void Level::draw(Game& game, double offset) {

		// Calculate colors
		double percentTween = (double)(tweenFrame) / (double)(factory.getSpeedPulse());
		Color FG = interpolateColor(factory.getColorsFG()[indexFG], factory.getColorsFG()[nextIndexFG], percentTween);
		Color BG1 = interpolateColor(factory.getColorsBG1()[indexBG1], factory.getColorsBG1()[nextIndexBG1], percentTween);
		Color BG2 = interpolateColor(factory.getColorsBG2()[indexBG2], factory.getColorsBG2()[nextIndexBG2], percentTween);

		// Fix for triangle levels
		int diagonal = (sidesTween >= 3 && sidesTween < 4 ? game.getRenderDistance() * 2 : game.getRenderDistance());

		Point center = game.getScreenCenter();
		Point shadow = game.getShadowOffset();

		game.drawBackground(BG1, BG2, center, diagonal, rotation, sidesTween);

		// Draw shadows
		Point offsetFocus = {center.x + shadow.x, center.y + shadow.y};
		game.drawPatterns(COLOR_SHADOW, offsetFocus, patterns, rotation, sidesTween, offset);
		game.drawRegular(COLOR_SHADOW, offsetFocus, game.getHexLength(), rotation, sidesTween);
		game.drawCursor(COLOR_SHADOW, offsetFocus, cursorPos, rotation);

		// Draw real thing
		game.drawPatterns(FG, center, patterns, rotation, sidesTween, offset);
		game.drawRegular(FG, center, game.getHexLength(), rotation, sidesTween);
		game.drawRegular(BG2, center, game.getHexLength() - game.getHexLengthBorder(), rotation, sidesTween);
		game.drawCursor(FG, center, cursorPos, rotation);
	}

	Movement Level::collision(int cursorDistance) const {
		auto collision = Movement::CAN_MOVE;

		// For all patterns (technically only need to check front two)
		for(const auto& pattern : patterns) {

			// For all walls
			for(const auto& wall : patterns.front()->getWalls()) {
				Movement check = wall->collision(cursorDistance, cursorPos, factory.getSpeedCursor(), pattern->getSides());

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

	void Level::rotate(double distance) {
		cursorPos += distance;
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

	LevelFactory::LevelFactory(Location location) :
		location(location)
	{}

	std::unique_ptr<Level> LevelFactory::instantiate(Twist& rng, int renderDistance) const {
		return std::make_unique<Level>(*this, rng, renderDistance);
	}

	bool LevelFactory::setHighScore(int score) {
		if(score > highScore) {
			highScore = score;
			return true;
		}

		return false;
	}
}