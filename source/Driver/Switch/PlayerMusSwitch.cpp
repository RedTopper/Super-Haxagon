#include "Driver/Switch/PlayerMusSwitch.hpp"

namespace SuperHaxagon {
	PlayerMusSwitch::PlayerMusSwitch(Mix_Music* music) : _music(music) {}

	PlayerMusSwitch::~PlayerMusSwitch() {
		Mix_PauseMusic();
	}

	void PlayerMusSwitch::setLoop(const bool loop) {
		_loop = loop;
	}

	void PlayerMusSwitch::play() {
		Mix_PlayMusic(_music, _loop ? -1 : 1);
	}

	bool PlayerMusSwitch::isDone() {
		return !Mix_PlayingMusic();
	}

	double PlayerMusSwitch::getVelocity() {
		return 0;
	}
}
