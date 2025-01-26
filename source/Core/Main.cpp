#include "Core/Game.hpp"
#include "Core/Platform.hpp" 

#if defined __3DS__ || defined _3DS
#include "Driver/3DS/Platform3DS.hpp"
#elif defined __SWITCH__
#include "Driver/Switch/PlatformSwitch.hpp"
#elif defined _WIN64 || defined __CYGWIN__
#include "Driver/Win/PlatformWin.hpp"
#elif defined __linux__
#include "Driver/Linux/PlatformLinux.hpp"
#elif defined _nspire
#include "Driver/NSpire/PlatformNspire.hpp"
#elif defined __APPLE__
	#include "TargetConditionals.h"
	#if defined TARGET_OS_OSX
		#include "Driver/macOS/PlatformMacOS.hpp"
	#else
		#error "Target apple device is not supported by any driver."
	#endif
#else
#error "Target platform is not supported by any driver."
#endif

namespace SuperHaxagon {
	std::unique_ptr<Platform> getPlatform() {
		#if defined __3DS__  || defined _3DS
		return std::make_unique<Platform3DS>(Dbg::FATAL);
		#elif defined __SWITCH__
		return std::make_unique<PlatformSwitch>(Dbg::INFO);
		#elif defined _WIN64 || defined __CYGWIN__
		return std::make_unique<PlatformWin>(Dbg::INFO);
		#elif defined __linux__
		return std::make_unique<PlatformLinux>(Dbg::INFO);
		#elif defined _nspire
		return std::make_unique<PlatformNspire>(Dbg::INFO);
		#elif defined __APPLE__
			#include "TargetConditionals.h"
			#if defined TARGET_OS_OSX
				return std::make_unique<PlatformMacOS>(Dbg::INFO);
			#else
				return nullptr;
			#endif
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
	platform->message(SuperHaxagon::Dbg::INFO, "main", "starting main");

	if (platform->loop()) {
		SuperHaxagon::Game game(*platform);
		game.run();
	}

	platform->message(SuperHaxagon::Dbg::INFO, "main", "stopping main");
	platform->shutdown();

	return 0;
}
