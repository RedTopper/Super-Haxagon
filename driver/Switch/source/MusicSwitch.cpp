// SPDX-FileCopyrightText: 2020 AJ Walter
// SPDX-License-Identifier: GPL-3.0-or-later OR MIT

#include "Driver/Music.hpp"

#include <SDL2/SDL_mixer.h>

#include <switch/arm/counter.h>

#include <string>

namespace SuperHaxagon {
	float getNow() {
		// Note that this is kind of a shitty way to do this: if the user presses the
		// home button while the game is running (and not at a game over screen) the
		// clock will get offset from the music. I personally am not going to fix this,
		// but if someone ever stumbles across this comment feel free to do something
		// about it.
		return armTicksToNs(armGetSystemTick()) / 1.0e9;
	}

	struct Music::MusicImpl {
		MusicImpl(const std::string& path) {
			music = Mix_LoadMUS(path.c_str());
		}

		~MusicImpl() {
			Mix_HaltMusic();
			Mix_FreeMusic(music);
		}

		bool isDone() const {
			// Note that Mix_PlayingMusic does not check if the channel is paused.
			return !Mix_PlayingMusic();
		}

		void play() {
			if (isDone()) {
				start = getNow();
				Mix_PlayMusic(music, 1);
				return;
			}

			// We are not done, so resume the currently playing music.
			if (diff > 0) start += getNow() - diff;
			diff = 0;
			Mix_ResumeMusic();
		}

		void pause() {
			diff = getNow();
			Mix_PauseMusic();
		}

		float getTime() {
			// If we set diff (paused), we are frozen in time. Otherwise, the current timestamp is
			// the system minus the start of the song.
			return diff > 0 ? diff - start : getNow() - start;
		}

		void update() {
			// For the switch, we only play the music once and restart it when Mix has
			// told us it's finished. This allows us to reset the timers in this Music
			// instance. It might be possible to register a callback instead, but eh.
			if (loop && isDone()) {
				play();
			}
		}

		Mix_Music* music;

		// Timing controls
		bool loop = false;
		float start = 0;
		float diff = 0;
	};

	Music::Music(std::unique_ptr<Music::MusicImpl> impl) : _impl(std::move(impl)) {}

	Music::~Music() = default;

	void Music::update() const {
		_impl->update();
	}

	void Music::setLoop(const bool loop) const {
		_impl->loop = loop;
	}

	void Music::play() const {
		_impl->play();
	}

	void Music::pause() const {
		_impl->pause();
	}

	bool Music::isDone() const {
		return _impl->isDone();
	}

	float Music::getTime() const {
		return _impl->getTime();
	}

	std::unique_ptr<Music> createMusic(const std::string& path) {
		auto data = std::make_unique<Music::MusicImpl>(path);
		if (!data->music) return nullptr;
		return std::make_unique<Music>(std::move(data));
	}
}
