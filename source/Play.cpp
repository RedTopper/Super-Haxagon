#include "Play.hpp"
#include "Structs.hpp"
#include "Level.hpp"
#include "Game.hpp"
#include "Quit.hpp"
#include "Over.hpp"

namespace SuperHaxagon {
	Play::Play(Game& game, LevelFactory& factory) : game(game), platform(game.getPlatform()), factory(factory), level(factory.instantiate(game.getTwister(), game.getRenderDistance())) {}

	std::unique_ptr<State> Play::update() {

		// Button presses
		platform.pollButtons();
		auto down = platform.getDown();
		auto pressed = platform.getPressed();

		// Check collision
		auto hit = level->collision();
		if(down.back || hit == Movement::DEAD) {
			return std::make_unique<Over>();
		}

		if (down.quit) {
			return std::make_unique<Quit>();
		}

		if (pressed.left && hit != Movement::CANNOT_MOVE_LEFT) {
			level->left();
		} else if (pressed.right && hit != Movement::CANNOT_MOVE_RIGHT) {
			level->right();
		}

		// Make sure the cursor doesn't extend too far
		level->clamp();

		// Update score
		auto lastScoreText = getScoreText(score);
		score++;

		if(lastScoreText != getScoreText(score)) {
			level->increaseMultiplier();
			platform.playSFX(game.getSfxLevelUp());
		}
	}

	void Play::draw() {

		// Calculate colors
		double percentTween = (double)(tweenFrame) / (double)(level.getSpeedPulse());
		Color FG = interpolateColor(level.getColorsFG()[indexFG], level.getColorsFG()[nextIndexFG], percentTween);
		Color BG1 = interpolateColor(level.getColorsBG1()[indexBG1], level.getColorsBG1()[nextIndexBG1], percentTween);
		Color BG2 = interpolateColor(level.getColorsBG2()[indexBG2], level.getColorsBG2()[nextIndexBG2], percentTween);

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
}