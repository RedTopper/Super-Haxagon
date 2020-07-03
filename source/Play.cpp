#include "Play.h"

namespace SuperHaxagon {
	Play::Play(Game& game) : game(game), platform(game.getPlatform()) {

	}

	std::unique_ptr<State> Play::update() {
		return nullptr;
	}

	void Play::draw() {

	}
}