#include <string>
#include <algorithm>

#include "driver/driver_windows.h"

namespace SuperHaxagon {
	std::string DriverWindows::getPath(std::string partial) {
		auto path = std::string("./data/custom") + partial;
		std::replace(path.begin(), path.end(), '/', '\\');
		return path;
	}

	std::string DriverWindows::getPathRom(std::string partial) {
		auto path = std::string("./data/rom") + partial;
		std::replace(path.begin(), path.end(), '/', '\\');
		return path;
	}

	void DriverWindows::playAudio(std::string path) {

	}
}
