#include "Core/Game.hpp"
#include "Driver/Platform.hpp"

#ifdef _WIN64
int WinMain() {
#else
int main(int, char**) {
#endif
	SuperHaxagon::Platform platform;
	platform.message(SuperHaxagon::Dbg::INFO, "main", "starting main");

	if (platform.loop()) {
		SuperHaxagon::Game game(platform);
		game.run();
	}

	platform.message(SuperHaxagon::Dbg::INFO, "main", "stopping main");
	platform.shutdown();

	return 0;
}
