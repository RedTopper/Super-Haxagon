#ifndef SUPER_HAXAGON_DRIVER_H
#define SUPER_HAXAGON_DRIVER_H

#include <memory>

namespace SuperHaxagon {
	class Driver {
	public:
		virtual std::string getPathRom(std::string partial) = 0;
		virtual std::string getPath(std::string partial) = 0;

		virtual void playAudio(std::string path) = 0;
	};
}

#ifdef _3DS
#include "driver_3ds.h"
#elif __SWITCH__
#include "driver_switch.h"
#else
#include "driver_windows.h"
#endif

static std::shared_ptr<SuperHaxagon::Driver> getPlatformDriver() {
#ifdef _3DS
	return std::make_shared<SuperHaxagon::Driver3DS>();
#elif __SWITCH__
	return std::make_shared<SuperHaxagon::DriverSwitch>();
#else
	return std::make_shared<SuperHaxagon::DriverWindows>();
#endif
}

#endif //SUPER_HAXAGON_DRIVER_H
