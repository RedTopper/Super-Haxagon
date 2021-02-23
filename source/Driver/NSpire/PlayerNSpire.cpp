#include "Driver/NSpire/PlayerNSpire.hpp"

namespace SuperHaxagon {
	PlayerNSpire::PlayerNSpire(const float maxTime) {
		_maxTime = maxTime;
	}

	void PlayerNSpire::play() {
		if (isDone()) {
			_time = 0.0f;
		}
		
		_playing = true;
	}
	
	void PlayerNSpire::pause() {
		_playing = false;
	}

	bool PlayerNSpire::isDone() const {
		return _time > _maxTime + 5.0f;
	}
	
	float PlayerNSpire::getTime() const {
		return _time;
	}
	
	void PlayerNSpire::addTime(const float time) {
		if (_playing) {
			_time += time;
		}
	}
}
