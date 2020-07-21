#include "Driver/Switch/PlayerMusSwitch.hpp"
#include "Driver/Switch/AudioSwitch.hpp"
#include "Driver/Platform.hpp"

namespace SuperHaxagon {

	AudioSwitch::AudioSwitch(Platform& platform, const std::string& path, Stream stream) {
		platform.message(Dbg::INFO, "music", "loading " + path);
		if (stream == Stream::DIRECT) _sfx = Mix_LoadWAV((path + ".wav").c_str());
		if (stream == Stream::INDIRECT) _music = Mix_LoadMUS((path + ".ogg").c_str());

		if (!_music && stream == Stream::INDIRECT) {
			platform.message(Dbg::FATAL, "music", Mix_GetError());
		}

		platform.message(Dbg::INFO, "music", "aight");
	}

	AudioSwitch::~AudioSwitch() {
		if (_sfx) Mix_FreeChunk(_sfx);
		if (_music) Mix_FreeMusic(_music);
	}

	std::unique_ptr<Player> AudioSwitch::instantiate() {
		if (_sfx) return nullptr;
		if (_music) return std::make_unique<PlayerMusSwitch>(_music);
		return nullptr;
	}
}
