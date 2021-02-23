#include "Driver/Switch/AudioLoaderSwitch.hpp"

#include "Core/Platform.hpp"
#include "Driver/Switch/AudioPlayerMusSwitch.hpp"
#include "Driver/Switch/AudioPlayerSfxSwitch.hpp"

namespace SuperHaxagon {

	AudioLoaderSwitch::AudioLoaderSwitch(Platform& platform, const std::string& path, const Stream stream) {
		if (stream == Stream::DIRECT) _sfx = Mix_LoadWAV((path + ".wav").c_str());
		if (stream == Stream::INDIRECT) _music = Mix_LoadMUS((path + ".ogg").c_str());

		if (!_music && stream == Stream::INDIRECT) {
			platform.message(Dbg::WARN, "music", Mix_GetError());
		}
	}

	AudioLoaderSwitch::~AudioLoaderSwitch() {
		if (_sfx) Mix_FreeChunk(_sfx);
		if (_music) Mix_FreeMusic(_music);
	}

	std::unique_ptr<AudioPlayer> AudioLoaderSwitch::instantiate() {
		if (_sfx) return std::make_unique<AudioPlayerSfxSwitch>(_sfx);
		if (_music) return std::make_unique<AudioPlayerMusSwitch>(_music);
		return nullptr;
	}
}
