#include "Game.h"
#include "exception"

int main() {
	SuperHaxagon::Game game;

	try {
		return game.run(SuperHaxagon::getPlatform());
	} catch (std::exception& e) {
		// Do something?
		return -1;
	}
}