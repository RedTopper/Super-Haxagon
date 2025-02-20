#include "Driver/Music.hpp"

#include <SDL2/SDL_mixer.h>

#include <chrono>
#include <string>

namespace SuperHaxagon {
	std::chrono::system_clock::time_point getNow() {
		// Unfortunately the implementation of Mix_GetMusicPosition seems to be...
		// inaccurate, to say the least. It also forces us to use version 2.6.0.
		// So instead of relying on the kinda buggy implementation that Mix provides,
		// keep track of the time ourselves.

		// This does mean that if the application is paused for any reason (say, a debugger)
		// the current time the music is at will be offset from what it actually is.

		// A good compromise would be to use a mix of both, where if Mix_GetMusicPosition
		// deviates too far form what we have, update our timestamp with Mix's, but that
		// would again force us to use a newer version of Mix.
		return std::chrono::high_resolution_clock::now();
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
				paused = false;
				auto now = getNow();
				timeStarted = now;
				timePaused = now;
				Mix_PlayMusic(music, 1);
				return;
			}

			// We are not done, so resume the currently playing music.
			// Add the length of time we were paused to the start time of the music.
			if (paused) timeStarted += getNow() - timePaused;
			Mix_ResumeMusic();
			paused = false;
		}

		void pause() {
			paused = true;
			timePaused = getNow();
			Mix_PauseMusic();
		}

		float getTime() {
			// If we are paused, always return the difference from when we paused and when we started
			// On resume, timeStarted will update to include the time paused.
			// Return the current time in seconds (which is what the game expects)
			std::chrono::duration<float> duration = paused ? timePaused - timeStarted : getNow() - timeStarted;
			return duration.count();
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
		bool paused = true;
		std::chrono::system_clock::time_point timeStarted;
		std::chrono::system_clock::time_point timePaused;
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
