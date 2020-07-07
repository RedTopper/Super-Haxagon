#include <cstring>

#include "Core/Game.hpp"
#include "Driver/Font.hpp"
#include "Factories/Level.hpp"
#include "States/Quit.hpp"
#include "States/Over.hpp"
#include "States/Play.hpp"
#include "States/Menu.hpp"
#include "States/Load.hpp"

namespace SuperHaxagon {

	Over::Over(Game& game, LevelFactory& factory, std::unique_ptr<Level> level, int score) :
		game(game),
		platform(game.getPlatform()),
		factory(factory),
		level(std::move(level)),
		score(score) {
		high = factory.setHighScore(score);
	}

	Over::~Over() = default;

	void Over::enter() {
		platform.playSFX(game.getSfxOver());

		std::ofstream scores(platform.getPath("/scores.db"), std::ios::out | std::ios::binary);

		if (!scores) {
			std::cout << "cannot open score db for write" << std::endl;
			return;
		}

		scores.write(Load::SCORE_HEADER, strlen(Load::SCORE_HEADER));
		uint32_t levels = game.getLevels().size();
		scores.write(reinterpret_cast<char*>(&levels), sizeof(levels));

		for (const auto& lev : game.getLevels()) {
			writeString(scores, lev->getName());
			writeString(scores, lev->getDifficulty());
			writeString(scores, lev->getMode());
			writeString(scores, lev->getCreator());
			uint32_t highSc = lev->getHighScore();
			scores.write(reinterpret_cast<char*>(&highSc), sizeof(highSc));
		}

		scores.write(Load::SCORE_FOOTER, strlen(Load::SCORE_FOOTER));
	}

	std::unique_ptr<State> Over::update(double dilation) {
		frames += dilation;
		level->rotate(GAME_OVER_ROT_SPEED, dilation);
		level->clamp();

		auto press = platform.getPressed();
		if(press.quit) return std::make_unique<Quit>();

		if(frames <= FRAMES_PER_GAME_OVER) {
			offset *= GAME_OVER_ACCELERATION_RATE * dilation + 1.0;
		}

		if(frames >= FRAMES_PER_GAME_OVER) {
			level->clearPatterns();
			if (press.select) {
				return std::make_unique<Play>(game, factory);
			}

			if (press.back) {
				return std::make_unique<Menu>(game);
			}
		}

		return nullptr;
	}

	void Over::drawTop(double scale) {
		level->draw(game, scale, offset);
	}

	void Over::drawBot(double scale) {
		auto& large = game.getFontLarge();
		auto& small = game.getFontSmall();
		large.setScale(scale);
		small.setScale(scale);

		double PAD_TEXT = 3 * scale;
		double PAD_REAL = 8 * scale;
		double MARGIN = 20 * scale;
		double width = platform.getScreenDim().x;
		double height = platform.getScreenDim().y;
		double heightLarge = large.getHeight();
		double heightSmall = small.getHeight();

		Point posGameOver = {width / 2, MARGIN};
		Point posTime = {width / 2, posGameOver.y + heightLarge + PAD_TEXT};
		Point posBest = {width / 2, posTime.y + heightSmall + PAD_TEXT};
		Point posB = {width / 2, height - MARGIN - heightSmall};
		Point posA = {width / 2, posB.y - heightSmall - PAD_TEXT};

		double bkgWidth = large.getWidth("GAME OVER");
		Point bkgPos = {posGameOver.x - bkgWidth/2 - PAD_REAL, posGameOver.y - PAD_REAL};
		Point bkgSize = {bkgWidth + PAD_REAL * 2, posBest.y + heightSmall + PAD_REAL * 2 - MARGIN};
		platform.drawRect(COLOR_TRANSPARENT, bkgPos, bkgSize);

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
			small.draw(COLOR_WHITE, posA, Alignment::CENTER, "PRESS (SEL) TO PLAY");
			small.draw(COLOR_WHITE, posB, Alignment::CENTER, "PRESS (ESC) TO QUIT");
		}
	}
}
