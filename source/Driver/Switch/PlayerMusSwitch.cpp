#include "Driver/Switch/PlayerMusSwitch.hpp"

#include <ctime>

namespace SuperHaxagon {
	PlayerMusSwitch::PlayerMusSwitch(Mix_Music* music) : _music(music) {}

	PlayerMusSwitch::~PlayerMusSwitch() {
		Mix_HaltMusic();
	}

	void PlayerMusSwitch::setLoop(const bool loop) {
		_loop = loop;
	}

	void PlayerMusSwitch::play() {
		if (isDone()) {
			// For the switch, we only play the music once and restart it in the platform.
			// This is because we need to detect when the music is over in order to reset the timers.
			// The Switch platform specifically continuously calls play, so if we are not
			// playing anything restart the music.
			_start = getNow();
			Mix_PlayMusic(_music, 1);
			return;
		}

		// We are not done, so resume the currently playing music.
		if (_diff > 0) _start += getNow() - _diff;
		_diff = 0;
		Mix_ResumeMusic();
	}

	void PlayerMusSwitch::pause() {
		_diff = getNow();
		Mix_PauseMusic();
	}

	bool PlayerMusSwitch::isDone() const {
		// Note that Mix_PlayingMusic does not check if the channel is paused.
		return !Mix_PlayingMusic();
	}

	double PlayerMusSwitch::getTime() const {
		// If we set diff (paused), we are frozen in time. Otherwise, the current timestamp is
		// the system minus the start of the song.
		return _diff > 0 ? _diff - _start : getNow() - _start;
	}

	double PlayerMusSwitch::getNow() const {
		// Note that this is kind of a shitty way to do this: if the user presses the
		// home button while the game is running (and not at a game over screen) the
		// clock will get offset from the music. I personally am not going to fix this,
		// but if someone ever stumbles across this comment feel free to do something
		// about it.
		timespec time{};
		clock_gettime(CLOCK_MONOTONIC, &time);
		double seconds = time.tv_sec;
		seconds += time.tv_nsec / 1.0e9;
		return seconds;
	}
}
