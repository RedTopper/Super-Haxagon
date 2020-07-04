#ifdef _WIN32
#include <string>
#include <algorithm>

#include "Driver/PlatformWin.hpp"

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

	void PlatformWin::playSFX(Audio& audio) {

	}

	void PlatformWin::playBGM(Audio& audio) {

	}

	void PlatformWin::stopBGM() {

	}

	std::unique_ptr<Twist> Platform3DS::getTwister() {
		std::random_device source;
		std::mt19937::result_type data[std::mt19937::state_size];
		generate(std::begin(data), std::end(data), ref(source));
		return std::make_unique<Twist>(
				std::make_unique<std::seed_seq>(std::begin(data), std::end(data))
		);
	}
}
#endif
