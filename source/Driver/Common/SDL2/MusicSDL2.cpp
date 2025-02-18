#include "Driver/Music.hpp"

#include <SDL2/SDL_mixer.h>

#include <string>

namespace SuperHaxagon {
	struct Music::MusicImpl {
		explicit MusicImpl(const std::string& path) {
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
				Mix_PlayMusic(music, loop ? -1 : 1);
			} else {
				Mix_ResumeMusic();
			}
		}

		void pause() {
			Mix_PauseMusic();
		}

		float getTime() {
			return Mix_GetMusicPosition(music);
		}

		Mix_Music* music;

		bool loop = false;
	};

	Music::Music(std::unique_ptr<Music::MusicImpl> impl) : _impl(std::move(impl)) {}

	Music::~Music() = default;

	void Music::update() const {}

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
