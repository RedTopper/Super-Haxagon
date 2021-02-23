#include "Driver/NSpire/AudioPlayerNspire.hpp"

namespace SuperHaxagon {
	AudioPlayerNspire::AudioPlayerNspire(const float maxTime) {
		_maxTime = maxTime;
	}

	void AudioPlayerNspire::play() {
		if (isDone()) {
			_time = 0.0f;
		}
		
		_playing = true;
	}
	
	void AudioPlayerNspire::pause() {
		_playing = false;
	}

	bool AudioPlayerNspire::isDone() const {
		return _time > _maxTime + 5.0f;
	}
	
	float AudioPlayerNspire::getTime() const {
		return _time;
	}
	
	void AudioPlayerNspire::addTime(const float time) {
		if (_playing) {
			_time += time;
		}
	}
}
