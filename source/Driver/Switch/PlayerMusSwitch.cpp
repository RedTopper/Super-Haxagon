#include "Driver/Switch/PlayerMusSwitch.hpp"

namespace SuperHaxagon {
	PlayerMusSwitch::PlayerMusSwitch(Mix_Music* music) : _music(music) {}

	PlayerMusSwitch::~PlayerMusSwitch() {
		Mix_HaltMusic();
	}

	void PlayerMusSwitch::setLoop(const bool loop) {
		_loop = loop;
	}

	void PlayerMusSwitch::play() {
		if (_launched) {
			Mix_ResumeMusic();
			return;
		}

		_launched = true;
		Mix_PlayMusic(_music, _loop ? -1 : 1);
	}

	void PlayerMusSwitch::pause() {
		Mix_PauseMusic();
	}

	bool PlayerMusSwitch::isDone() const {
		return !Mix_PlayingMusic();
	}

	double PlayerMusSwitch::getTime() const {
		return 0;
	}
}
