#include "Core/Game.hpp"

#if defined _3DS
#include "Driver/3DS/Platform3DS.hpp"
#elif defined __SWITCH__
#include "Driver/Switch/PlatformSwitch.hpp"
#elif defined _WIN64 || defined __CYGWIN__
#include "Driver/Win/PlatformWin.hpp"
#elif defined __linux__
#include "Driver/Linux/PlatformLinux.hpp"
#else
#error "Target platform is not supported by any driver."
#endif

namespace SuperHaxagon {
	std::unique_ptr<Platform> getPlatform() {
		#if defined _3DS
		return std::make_unique<Platform3DS>(Dbg::FATAL);
		#elif defined __SWITCH__
		return std::make_unique<PlatformSwitch>(Dbg::INFO);
		#elif defined _WIN64 || defined __CYGWIN__
		return std::make_unique<PlatformWin>(Dbg::INFO);
		#elif defined __linux__
		return std::make_unique<PlatformLinux>(Dbg::INFO);
		#else
		return nullptr;
		#endif
	}
}

#ifdef _WIN64
int WinMain() {
#else
int main(int, char**) {
#endif
	const auto platform = SuperHaxagon::getPlatform();
	platform->message(SuperHaxagon::Dbg::INFO, "main", "starting game");
	SuperHaxagon::Game game(*platform);
	const auto ret = game.run();
	platform->message(SuperHaxagon::Dbg::INFO, "main", "stopping game");
	platform->show();
	return ret;
}