#include <exception>

#include "Core/Game.hpp"

#ifdef _3DS
#include "Driver3DS//Platform3DS.hpp"
#elif __SWITCH__
#include "DriverSwitch/PlatformSwitch.hpp"
#elif _WIN32
#include "DriverWin/PlatformWin.hpp"
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
		std::cout << e.what();
		return -1;
	}
}