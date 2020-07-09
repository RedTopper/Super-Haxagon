#include <string>

#include "Driver3DS/AudioOgg3DS.hpp"
#include "Driver3DS/PlayerOgg3DS.hpp"

namespace SuperHaxagon {
	AudioOgg3DS::AudioOgg3DS(const std::string& path) : _path(path + ".ogg") {}
	AudioOgg3DS::~AudioOgg3DS() = default;

	std::unique_ptr<Player> AudioOgg3DS::instantiate() {
		return std::make_unique<PlayerOgg3DS>(_path);
	}
}
