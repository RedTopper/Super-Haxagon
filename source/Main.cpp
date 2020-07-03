#include <exception>

#include "Game.hpp"

int main() {
	auto platform = SuperHaxagon::getPlatform();
	SuperHaxagon::Game game(*platform);

	try {
		return game.run();
	} catch (std::exception& e) {
		// Do something?
		return -1;
	}
}