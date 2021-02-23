#include "Driver/3DS/AudioLoaderOgg3DS.hpp"

#include "Driver/3DS/AudioPlayerOgg3DS.hpp"

namespace SuperHaxagon {
	AudioLoaderOgg3DS::AudioLoaderOgg3DS(const std::string& path) : _path(path + ".ogg") {}
	AudioLoaderOgg3DS::~AudioLoaderOgg3DS() = default;

	std::unique_ptr<AudioPlayer> AudioLoaderOgg3DS::instantiate() {
		return std::make_unique<AudioPlayerOgg3DS>(_path);
	}
}
