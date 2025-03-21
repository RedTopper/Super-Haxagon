// SPDX-FileCopyrightText: 2025 AJ Walter, see driver/Example/README.md for MIT usage.
// SPDX-License-Identifier: GPL-3.0-or-later OR MIT

#include "Driver/Sound.hpp"

namespace SuperHaxagon {
	struct Sound::SoundImpl {};

	Sound::Sound(std::unique_ptr<SoundImpl> data) : _impl(std::move(data)) {}

	Sound::~Sound() = default;

	void Sound::play() const {}

	std::unique_ptr<Sound> createSound() {
		// This will return an instance of an empty sound, but the game
		// supports returning a nullptr from the Platform as well.
		// If you don't support sound effects, consider doing that instead.
		return std::make_unique<Sound>(std::make_unique<Sound::SoundImpl>());
	}
}
