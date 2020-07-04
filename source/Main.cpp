#include <exception>

#include "Game.hpp"



#ifdef _3DS
#include "Driver/Platform3DS.hpp"
#elif __SWITCH__
#include "Driver/PlatformSwitch.hpp"
#elif _WIN32
#include "Driver/PlatformWin.hpp"
int main();
int WinMain() {return main();}
#else
#error "Target platform is not supported by driver."
#endif

namespace SuperHaxagon {
	std::unique_ptr<Platform> getPlatform() {
#ifdef _3DS
		return std::make_unique<Platform3DS>();
#elif __SWITCH__
		return std::make_unique<PlatformSwitch>();
#elif _WIN32
		return std::make_unique<PlatformWin>();
#else
		returnn nullptr;
#endif
	}
}

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