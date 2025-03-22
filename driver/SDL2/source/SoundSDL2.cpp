// SPDX-FileCopyrightText: 2025 AJ Walter, see driver/SDL2/README.md for MIT usage.
// SPDX-License-Identifier: GPL-3.0-or-later OR MIT

#include "Driver/Sound.hpp"

#include <SDL2/SDL_mixer.h>

#include <string>

namespace SuperHaxagon {
	struct Sound::SoundImpl {
		SoundImpl(const std::string& path) {
			sfx = Mix_LoadWAV(path.c_str());
		}

		~SoundImpl() {
			Mix_FreeChunk(sfx);
		}

		Mix_Chunk* sfx;
	};

	Sound::Sound(std::unique_ptr<SoundImpl> impl) : _impl(std::move(impl)) {}
	
	Sound::~Sound() = default;

	void Sound::play() const {
		Mix_PlayChannel(-1, _impl->sfx, 0);
	}

	std::unique_ptr<Sound> createSound(const std::string& path) {
		auto data = std::make_unique<Sound::SoundImpl>(path);
		if (!data->sfx) return nullptr;
		return std::make_unique<Sound>(std::move(data));
	}
}
