#include <memory>

#include "Load.hpp"

namespace SuperHaxagon {
	Load::Load(Game& game) : game(game), platform(game.getPlatform()) {

	}

	std::unique_ptr<State> Load::update() {
		return nullptr;
	}

	void Load::draw() {

	}
}