#include "Play.hpp"
#include "Structs.hpp"
#include "Level.hpp"
#include "Game.hpp"
#include "Quit.hpp"
#include "Over.hpp"

namespace SuperHaxagon {
	Play::Play(Game& game, LevelFactory& level) : game(game), platform(game.getPlatform()), level(level) {
		nextIndexBG1 = (level.getColorsBG1().size() > 1 ? 1 : 0);
		nextIndexBG2 = (level.getColorsBG2().size() > 1 ? 1 : 0);
		nextIndexFG = (level.getColorsFG().size() > 1 ? 1 : 0);

		//fetch some random starting patterns
		auto distance = (double)game.getRenderDistance();
		for(int i = 0;  i < TOTAL_PATTERNS_AT_ONE_TIME;  i++) {
			int pCount = level.getPatterns().size();
			int pSelected = game.getTwister().rand(pCount);
			auto& pattern = level.getPatterns()[pSelected];
			patterns.emplace_back(pattern.instantiate(game.getTwister(), distance));
			distance = patterns.back().getFurthestWallDistance();
		}

		//set up the amount of sides the level should have.
		lastSides = patterns.front().getSides();
		currentSides = patterns.front().getSides();
		cursorPos = TAU/4.0 + (level.getSpeedCursor() / 2.0);
	}

	std::unique_ptr<State> Play::update() {
		// Update color frame and clamp
		tweenFrame++;
		if(tweenFrame >= level.getSpeedPulse()) {
			tweenFrame = 0;
			indexBG1 = nextIndexBG1;
			indexBG2 = nextIndexBG2;
			indexFG = nextIndexFG;
			nextIndexBG1 = (indexBG1 + 1 < level.getColorsBG1().size() ? indexBG1 + 1 : 0);
			nextIndexBG2 = (indexBG2 + 1 < level.getColorsBG2().size() ? indexBG2 + 1 : 0);
			nextIndexFG = (indexFG + 1 < level.getColorsFG().size() ? indexFG + 1 : 0);
		}

		// Bring walls forward if we are not delaying
		// Otherwise tween from one shape to another.
		if(delayFrame == 0) {
			sidesTween = currentSides;
			for(auto& pattern : patterns) {
				pattern.advance(level.getSpeedWall());
			}
		} else {
			double percent = (double)(delayFrame) / (double)(FRAMES_PER_CHANGE_SIDE * abs(currentSides - lastSides));
			sidesTween = Game::linear((double)currentSides, (double)lastSides, percent);
			delayFrame--;
		}

		// Shift patterns forward
		if(patterns.front().getFurthestWallDistance() < game.getHexLength()) {
			lastSides = patterns.front().getSides();
			patterns.pop_front();

			int pCount = level.getPatterns().size();
			int pSelected = game.getTwister().rand(pCount);
			auto& pattern = level.getPatterns()[pSelected];
			patterns.emplace_back(pattern.instantiate(game.getTwister(), patterns.back().getFurthestWallDistance()));

			currentSides = patterns.front().getSides();

			// Delay the level if the shifted pattern does  not have the same sides as the last.
			if(lastSides != currentSides) {
				delayFrame = FRAMES_PER_CHANGE_SIDE * abs(currentSides - lastSides);
			}
		}

		// Rotate level
		rotation += level.getSpeedRotation() * multiplier;
		if(rotation >= TAU) rotation -= TAU;
		if(rotation < 0) rotation  += TAU;

		// Flip level if needed
		flipFrame--;
		if(flipFrame == 0) {
			multiplier *= -1.0;
			flipFrame = game.getTwister().rand(FLIP_FRAMES_MIN, FLIP_FRAMES_MAX);
		}

		// Button presses
		platform.pollButtons();
		auto down = platform.getDown();
		auto pressed = platform.getPressed();

		// Check collision
		auto hit = collision();
		if(down.back || hit == Movement::DEAD) {
			return std::make_unique<Over>();
		}

		if (down.quit) {
			return std::make_unique<Quit>();
		}

		if (pressed.left && hit != Movement::CANNOT_MOVE_LEFT) {
			cursorPos += level.getSpeedCursor();
		} else if (pressed.right && hit != Movement::CANNOT_MOVE_RIGHT) {
			cursorPos -= level.getSpeedCursor();
		}

		// Clamp cursor position
		if(cursorPos >= TAU) cursorPos -= TAU;
		if(cursorPos < 0) cursorPos  += TAU;

		// Update score
		auto lastScoreText = Game::getScoreText(score);
		score++;
		if(lastScoreText != Game::getScoreText(score)) {
			multiplier *= DIFFICULTY_MULTIPLIER;
			platform.playSFX(game.getSfxLevelUp());
		}
	}

	void Play::draw() {
		// Calculate colors
		double percentTween = (double)(tweenFrame) / (double)(level.getSpeedPulse());
		Color FG = Game::interpolateColor(level.getColorsFG()[indexFG], level.getColorsFG()[nextIndexFG], percentTween);
		Color BG1 = Game::interpolateColor(level.getColorsBG1()[indexBG1], level.getColorsBG1()[nextIndexBG1], percentTween);
		Color BG2 = Game::interpolateColor(level.getColorsBG2()[indexBG2], level.getColorsBG2()[nextIndexBG2], percentTween);

		// Fix for triangle levels
		int diagonal = (sidesTween >= 3 && sidesTween < 4 ? game.getRenderDistance() * 2 : game.getRenderDistance());

		Point center = game.getScreenCenter();
		Point shadow = getShadowOffset();

		game.drawBackground(BG1, BG2, center, diagonal, rotation, sidesTween);

		// Draw shadows
		Point offsetFocus = {center.x + shadow.x, center.y + shadow.y};
		game.drawMovingPatterns(COLOR_SHADOW, offsetFocus, patterns, rotation, sidesTween, 0.0);
		game.drawRegular(COLOR_SHADOW, offsetFocus, game.getHexLength(), rotation, sidesTween);
		game.drawHumanCursor(COLOR_SHADOW, offsetFocus, cursorPos, rotation);

		// Draw real thing
		game.drawMovingPatterns(FG, center, patterns, rotation, sidesTween, 0.0);
		game.drawRegular(FG, center, game.getHexLength(), rotation, sidesTween);
		game.drawRegular(BG2, center, game.getHexLength() - game.getHexLengthBorder(), rotation, sidesTween);
		game.drawHumanCursor(FG, center, cursorPos, rotation);
	}

	Point Play::getShadowOffset() const {
		int min = game.getScreenDimMin();
		return {min/60, min/60};
	}

	Movement Play::collision() const {
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
}