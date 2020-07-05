#include "Core/Game.hpp"
#include "Driver/Font.hpp"
#include "Factories/Level.hpp"
#include "States/Play.hpp"
#include "States/Quit.hpp"
#include "States/Over.hpp"

namespace SuperHaxagon {

	Play::Play(Game& game, LevelFactory& factory) :
		game(game),
		platform(game.getPlatform()),
		factory(factory),
		level(factory.instantiate(game.getTwister(), game.getRenderDistance()))
	{}

	void Play::enter() {
		platform.playSFX(game.getSfxBegin());
	}

	std::unique_ptr<State> Play::update() {
		level->update(game.getTwister(), game.getHexLength(), game.getRenderDistance());

		// Button presses
		platform.pollButtons();
		auto down = platform.getDown();
		auto pressed = platform.getPressed();

		// Check collision
		int cursorDistance = game.getHexLength() + game.getHumanPadding() + game.getHumanHeight();
		auto hit = level->collision(cursorDistance);
		if(down.back || hit == Movement::DEAD) {
			return std::make_unique<Over>(game, factory, std::move(level), score);
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

		return nullptr;
	}

	void Play::draw() {
		level->draw(game, 0);

		game.clearBotAndSwitchScreens();

		int width = platform.getScreenDim().x;

		const auto& large = game.getFontLarge();
		const auto& small = game.getFontSmall();
		Point posLevelUp = {4,4};
		Point posScore = { width - 4, 4};
		Point posBest = {width - 4, posScore.y + small.getHeight()};

		auto textScore = std::string("TIME: ") + getTime(score);

		small.draw(COLOR_WHITE, posScore, Alignment::RIGHT, textScore);
		small.draw(COLOR_WHITE, posLevelUp, Alignment::LEFT, getScoreText(score));

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
