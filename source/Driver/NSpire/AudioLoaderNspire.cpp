#include "Driver/NSpire/AudioLoaderNspire.hpp"

#include <istream>

namespace SuperHaxagon {
	AudioLoaderNspire::AudioLoaderNspire(std::unique_ptr<std::istream> file) {
		Metadata metadata(std::move(file));
		_time = metadata.getMaxTime();
	}

	std::unique_ptr<AudioPlayer> AudioLoaderNspire::instantiate() {
		return std::make_unique<AudioPlayerNspire>(_time);
	}
}
