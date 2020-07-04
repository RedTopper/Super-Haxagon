#include "Factories/Level.hpp"
#include "States/Quit.hpp"
#include "States/Over.hpp"
#include "States/Play.hpp"
#include "States/Menu.hpp"

#include "Game.hpp"

namespace SuperHaxagon {

	Over::Over(Game& game,const LevelFactory& factory, std::unique_ptr<Level> level) :
		game(game),
		factory(factory),
		level(std::move(level)),
		platform(game.getPlatform())
	{}

	std::unique_ptr<State> Over::update() {
		frames++;
		level->rotate(GAME_OVER_ROT_SPEED);
		level->clamp();

		auto press = platform.getDown();
		if(press.quit) return std::make_unique<Quit>();

		if(frames <= FRAMES_PER_GAME_OVER) {
			offset *= GAME_OVER_ACCELERATION_RATE;
		}

		if(frames >= FRAMES_PER_GAME_OVER) {
			if (press.select) {
				return std::make_unique<Play>(game, factory);
			}

			if (press.back) {
				return std::make_unique<Menu>(game, false);
			}
		}
	}

	void Over::draw() {
		level->draw(game, offset);

		Point posGameOver = {BOT_WIDTH / 2, 4};
		Point posTime = {BOT_WIDTH / 2, 40};
		Point posBest = {BOT_WIDTH / 2, 56};
		Point posA = {BOT_WIDTH / 2, 78};
		Point posB = {BOT_WIDTH / 2, 94};

		drawBlackBot();

		char* scoreTime  = getScoreTime(score);
		writeFont(WHITE, posGameOver, "GAME OVER", FONT32, ALIGN_CENTER_C);
		writeFont(WHITE, posTime, scoreTime, FONT16, ALIGN_CENTER_C);
		free(scoreTime);

		if(score > highScore) {
			double percent = getPulse(frames, PULSE_TIME, 0);
			Color pulse = interpolateColor(PULSE_LOW, PULSE_HIGH, percent);
			writeFont(pulse, posBest, "NEW RECORD!", FONT16, ALIGN_CENTER_C);
		} else {
			char* bestTime = getBestTime(highScore);
			writeFont(WHITE, posBest, bestTime, FONT16, ALIGN_CENTER_C);
			free(bestTime);
		}

		if(showText) {
			writeFont(WHITE, posA, "PRESS A TO PLAY", FONT16, ALIGN_CENTER_C);
			writeFont(WHITE, posB, "PRESS B TO QUIT", FONT16, ALIGN_CENTER_C);
		}

		drawFramerate(fps);
	}
}
