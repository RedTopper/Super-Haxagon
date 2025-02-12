#include "Driver/Music.hpp"

#include <SDL2/SDL_mixer.h>

#include <switch/arm/counter.h>

#include <string>

namespace SuperHaxagon {
	struct Music::MusicData {
		MusicData(const std::string& path) {
			music = Mix_LoadMUS(path.c_str());
		}

		~MusicData() {
			Mix_FreeMusic(music);
		}

		Mix_Music* music;

		// Timing controls
		bool loop = false;
		float start = 0;
		float diff = 0;
	};

	float getNow() {
		// Note that this is kind of a shitty way to do this: if the user presses the
		// home button while the game is running (and not at a game over screen) the
		// clock will get offset from the music. I personally am not going to fix this,
		// but if someone ever stumbles across this comment feel free to do something
		// about it.
		return armTicksToNs(armGetSystemTick()) / 1.0e9;
	}

	std::unique_ptr<Music> createMusic(const std::string& path) {
		auto data = std::make_unique<Music::MusicData>(path);
		if (!data->music) return nullptr;
		return std::make_unique<Music>(std::move(data));
	}

	Music::Music(std::unique_ptr<Music::MusicData> data) : _data(std::move(data)) {}

	Music::~Music() {
		Mix_HaltMusic();
	}

	void Music::update() const {
		// For the switch, we only play the music once and restart it when Mix has
		// told us it's finished. This allows us to reset the timers in this Music
		// instance. It might be possible to register a callback instead, but eh.
		if (_data->loop && isDone()) {
			play();
		}
	}

	void Music::setLoop(const bool loop) const {
		_data->loop = loop;
	}

	void Music::play() const {
		if (isDone()) {
			_data->start = getNow();
			Mix_PlayMusic(_data->music, 1);
			return;
		}

		// We are not done, so resume the currently playing music.
		if (_data->diff > 0) _data->start += getNow() - _data->diff;
		_data->diff = 0;
		Mix_ResumeMusic();
	}

	void Music::pause() const {
		_data->diff = getNow();
		Mix_PauseMusic();
	}

	bool Music::isDone() const {
		// Note that Mix_PlayingMusic does not check if the channel is paused.
		return !Mix_PlayingMusic();
	}

	float Music::getTime() const {
		// If we set diff (paused), we are frozen in time. Otherwise, the current timestamp is
		// the system minus the start of the song.
		return _data->diff > 0 ? _data->diff - _data->start : getNow() - _data->start;
	}
}
