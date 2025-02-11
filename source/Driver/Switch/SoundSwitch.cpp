#include "Driver/Sound.hpp"

#include <SDL2/SDL_mixer.h>

namespace SuperHaxagon {
	struct Sound::SoundData {
		SoundData(Mix_Chunk* sfx) : sfx(sfx) {}

		Mix_Chunk* sfx;
	};

	std::unique_ptr<Sound> createSound(Mix_Chunk* sfx) {
		return std::make_unique<Sound>(std::make_unique<Sound::SoundData>(sfx));
	}

	Sound::Sound(std::unique_ptr<SoundData> data) : _data(std::move(data)) {}
	
	Sound::~Sound() = default;

	void Sound::setChannel(int) {}

	void Sound::play() const {
		Mix_PlayChannel(-1, _data->sfx, 0);
	}

	bool Sound::isDone() const {
		return true;
	}
}
