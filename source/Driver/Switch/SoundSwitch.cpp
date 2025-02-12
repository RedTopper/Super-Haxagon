#include "Driver/Sound.hpp"

#include <SDL2/SDL_mixer.h>

#include <string>

namespace SuperHaxagon {
	struct Sound::SoundData {
		SoundData(const std::string& path) {
			sfx = Mix_LoadWAV(path.c_str());
		}

		~SoundData() {
			Mix_FreeChunk(sfx);
		}

		Mix_Chunk* sfx;
	};

	std::unique_ptr<Sound> createSound(const std::string& path) {
		auto data = std::make_unique<Sound::SoundData>(path);
		if (!data->sfx) return nullptr;
		return std::make_unique<Sound>(std::move(data));
	}

	Sound::Sound(std::unique_ptr<SoundData> data) : _data(std::move(data)) {}
	
	Sound::~Sound() = default;

	void Sound::play() const {
		Mix_PlayChannel(-1, _data->sfx, 0);
	}
}
