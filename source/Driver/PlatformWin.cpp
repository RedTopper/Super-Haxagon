#include <string>
#include <algorithm>

#include "Driver/PlatformWin.h"

namespace SuperHaxagon {
	std::string PlatformWin::getPath(const std::string& partial) {
		auto path = std::string("./data/custom") + partial;
		std::replace(path.begin(), path.end(), '/', '\\');
		return path;
	}

	std::string PlatformWin::getPathRom(const std::string& partial) {
		auto path = std::string("./data/rom") + partial;
		std::replace(path.begin(), path.end(), '/', '\\');
		return path;
	}

	std::unique_ptr<Audio> PlatformWin::loadAudio(const std::string& path) {
		return std::unique_ptr<Audio>();
	}

	void PlatformWin::playSFX(Audio* audio) {

	}

	void PlatformWin::playBGM(Audio* audio) {

	}

	void PlatformWin::stopBGM() {

	}
}
