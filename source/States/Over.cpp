#include "Core/Game.hpp"
#include "Driver/Font.hpp"
#include "Factories/Level.hpp"
#include "States/Quit.hpp"
#include "States/Over.hpp"
#include "States/Play.hpp"
#include "States/Menu.hpp"

namespace SuperHaxagon {

	Over::Over(Game& game, LevelFactory& factory, std::unique_ptr<Level> level, int score) :
		game(game),
		platform(game.getPlatform()),
		factory(factory),
		level(std::move(level)),
		score(score)
	{
		high = factory.setHighScore(score);
	}

	void Over::enter() {
		platform.playSFX(game.getSfxOver());
	}

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
				return std::make_unique<Menu>(game);
			}
		}

		return nullptr;
	}

	void Over::drawTop() {
		level->draw(game, offset);
	}

	void Over::drawBot() {
		const auto& large = game.getFontLarge();
		const auto& small = game.getFontSmall();
		int width = platform.getScreenDim().x;
		int heightLarge = large.getHeight();
		int heightSmall = small.getHeight();

		Point posGameOver = {width / 2, 4};
		Point posTime = {width / 2, posGameOver.y + heightLarge + 4};
		Point posBest = {width / 2, posTime.y + heightSmall + 2};
		Point posA = {width / 2, posBest.y + heightSmall * 2 + 2};
		Point posB = {width / 2, posBest.y + heightSmall + 2};

		auto textScore = std::string("SCORE: ") + getTime(score);
		large.draw(COLOR_WHITE, posGameOver, Alignment::CENTER,  "GAME OVER");
		small.draw(COLOR_WHITE, posTime, Alignment::CENTER, textScore);

		if(high) {
			double percent = getPulse(frames, PULSE_TIME, 0);
			Color pulse = interpolateColor(PULSE_LOW, PULSE_HIGH, percent);
			small.draw(pulse, posBest, Alignment::CENTER, "NEW RECORD!");
		} else {
			auto textBest = std::string("BEST: ") + getTime(factory.getHighScore());
			small.draw(COLOR_WHITE, posBest, Alignment::CENTER, textBest);
		}

		if(frames >= FRAMES_PER_GAME_OVER) {
			small.draw(COLOR_WHITE, posA, Alignment::CENTER, "PRESS A TO PLAY");
			small.draw(COLOR_WHITE, posB, Alignment::CENTER, "PRESS B TO QUIT");
		}
	}
}
