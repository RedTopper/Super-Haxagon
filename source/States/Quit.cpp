#include "States/Quit.hpp"

#include "Core/Game.hpp"

namespace SuperHaxagon {
	Quit::Quit(Game& game) : _game(game) {}

	std::unique_ptr<State> Quit::update(double) {
		_game.setRunning(false);
		return nullptr;
	}
}