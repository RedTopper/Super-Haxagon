#ifndef SUPER_HAXAGON_DRIVER_WINDOWS_H
#define SUPER_HAXAGON_DRIVER_WINDOWS_H

#include "driver.h"

namespace SuperHaxagon {
	class DriverWindows : public Driver {
	public:
		std::string getPath(std::string partial) override;
		std::string getPathRom(std::string partial) override;

		void playAudio(std::string path) override;
	};
}

#endif //SUPER_HAXAGON_DRIVER_WINDOWS_H
