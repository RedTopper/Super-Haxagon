#include "Core/Game.hpp"
#include "Driver/Font.hpp"
#include "Driver/Audio.hpp"
#include "Factories/Level.hpp"
#include "States/Play.hpp"
#include "States/Quit.hpp"
#include "States/Over.hpp"

namespace SuperHaxagon {

	Play::Play(Game& game, LevelFactory& factory) :
		game(game),
		platform(game.getPlatform()),
		factory(factory),
		level(factory.instantiate(game.getTwister(), SCALE_BASE_DISTANCE))
	{}

	Play::~Play() {
		std::cout << "dead" << std::endl;
	}


	void Play::enter() {
		bgm = platform.loadAudio(platform.getPath(factory.getMusic()));
		platform.playSFX(game.getSfxBegin());
		platform.playBGM(*bgm);
	}

	void Play::exit() {
		platform.stopBGM();
	}

	std::unique_ptr<State> Play::update(double dilation) {
		double maxRenderDistance = SCALE_BASE_DISTANCE * (game.getScreenDimMax() / 240);
		level->update(game.getTwister(), SCALE_HEX_LENGTH, maxRenderDistance, dilation);

		// Button presses
		auto pressed = platform.getPressed();

		// Check collision
		double cursorDistance = SCALE_HEX_LENGTH + SCALE_HUMAN_PADDING + SCALE_HUMAN_HEIGHT;
		auto hit = level->collision(cursorDistance, dilation);
		if(pressed.back || hit == Movement::DEAD) {
			return std::make_unique<Over>(game, factory, std::move(level), score);
		}

		if (pressed.quit) {
			return std::make_unique<Quit>();
		}

		if (pressed.left && hit != Movement::CANNOT_MOVE_LEFT) {
			level->left(dilation);
		} else if (pressed.right && hit != Movement::CANNOT_MOVE_RIGHT) {
			level->right(dilation);
		}

		// Make sure the cursor doesn't extend too far
		level->clamp();

		// Update score
		auto lastScoreText = getScoreText((int)score);
		score += dilation;

		if(lastScoreText != getScoreText((int)score)) {
			level->increaseMultiplier();
			platform.playSFX(game.getSfxLevelUp());
		}

		return nullptr;
	}

	void Play::drawTop(double scale) {
		level->draw(game, scale, 0);
	}

	void Play::drawBot(double) {
		double width = platform.getScreenDim().x;

		const auto& small = game.getFontSmall();
		Point posLevelUp = {4,4};
		Point posScore = { width - 4, 4};
		Point posBest = {width - 4, posScore.y + small.getHeight()};

		auto textScore = std::string("TIME: ") + getTime((int)score);

		small.draw(COLOR_WHITE, posScore, Alignment::RIGHT, textScore);
		small.draw(COLOR_WHITE, posLevelUp, Alignment::LEFT, getScoreText((int)score));

		if(factory.getHighScore() > 0 && score > factory.getHighScore()) {
			Color textColor = COLOR_WHITE;
			if (score - factory.getHighScore() <= PULSE_TIMES * PULSE_TIME) {
				double percent = getPulse(score, PULSE_TIME, factory.getHighScore());
				textColor = interpolateColor(PULSE_LOW, PULSE_HIGH, percent);
			}

			small.draw(textColor, posBest, Alignment::RIGHT,  "NEW RECORD!");
		} else {
			auto textBest = std::string("BEST: ") + getTime(factory.getHighScore());
			small.draw(COLOR_WHITE, posBest, Alignment::RIGHT, textBest);
		}
	}
}
