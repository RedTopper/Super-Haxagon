#include "Play.hpp"
#include "Structs.hpp"
#include "Level.hpp"
#include "Game.hpp"
#include "Quit.hpp"
#include "Over.hpp"

namespace SuperHaxagon {
	Play::Play(Game& game, LevelFactory& factory) :
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
		level->draw(game);
	}
}