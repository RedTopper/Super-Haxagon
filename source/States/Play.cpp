#include "Factories/Level.hpp"
#include "States/Play.hpp"
#include "States/Quit.hpp"
#include "States/Over.hpp"

#include "Structs.hpp"
#include "Game.hpp"

namespace SuperHaxagon {
	Play::Play(Game& game, const LevelFactory& factory) :
		game(game),
		platform(game.getPlatform()),
		factory(factory),
		level(factory.instantiate(game.getTwister(), game.getRenderDistance()))
	{}

	std::unique_ptr<State> Play::update() {

		// Button presses
		platform.pollButtons();
		auto down = platform.getDown();
		auto pressed = platform.getPressed();

		// Check collision
		int cursorDistance = game.getHexLength() + game.getHumanPadding() + game.getHumanHeight();
		auto hit = level->collision(cursorDistance);
		if(down.back || hit == Movement::DEAD) {
			return std::make_unique<Over>(game, factory, std::move(level));
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
		level->draw(game, 0);

		Point posLevelUp = {4,4};
		Point posScore = {BOT_WIDTH - 4, 4};
		Point posBest = {BOT_WIDTH - 4, 20};

		drawBlackBot();

		char* scoreTime = getScoreTime(liveLevel.score);
		writeFont(WHITE, posScore, scoreTime, FONT16, ALIGN_RIGHT_C);
		writeFont(WHITE, posLevelUp, getScoreText(liveLevel.score), FONT16, ALIGN_LEFT_C);
		free(scoreTime);

		if(level.highScore > 0 && liveLevel.score > level.highScore) {
			Color textColor = WHITE;
			if (liveLevel.score - level.highScore <= PULSE_TIMES * PULSE_TIME) {
				double percent = getPulse(liveLevel.score, PULSE_TIME, level.highScore);
				textColor = interpolateColor(PULSE_LOW, PULSE_HIGH, percent);
			}
			writeFont(textColor, posBest, "NEW RECORD!", FONT16, ALIGN_RIGHT_C);
		} else {
			char* bestTime = getBestTime(level.highScore);
			writeFont(WHITE, posBest, bestTime, FONT16, ALIGN_RIGHT_C);
			free(bestTime);
		}
	}
}