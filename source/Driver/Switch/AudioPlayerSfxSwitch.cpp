#include "Driver/Switch/AudioPlayerSfxSwitch.hpp"

namespace SuperHaxagon {
	AudioPlayerSfxSwitch::AudioPlayerSfxSwitch(Mix_Chunk* sfx) : _sfx(sfx) {}
	
	AudioPlayerSfxSwitch::~AudioPlayerSfxSwitch() = default;

	void AudioPlayerSfxSwitch::play() {
		Mix_PlayChannel(-1, _sfx, 0);
	}
}
