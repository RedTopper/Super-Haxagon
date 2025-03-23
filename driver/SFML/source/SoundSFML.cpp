// SPDX-FileCopyrightText: 2020 AJ Walter
// SPDX-License-Identifier: GPL-3.0-or-later OR MIT

#include "Driver/Sound.hpp"

#include "CreateSFML.hpp"

#include <SFML/Audio.hpp>

namespace SuperHaxagon {
	struct Sound::SoundImpl {
		~SoundImpl() {
			for (auto& player : players) {
				player.stop();
			}
		}

		void play() {
			// Clear out any finished players
			for (auto it = players.begin(); it != players.end();) {
				const auto& playing = *it;
				if (playing.getStatus() == sf::SoundSource::Stopped) {
					it = players.erase(it);
				} else {
					++it;
				}
			}

			players.emplace_back(buffer).play();
		}

		sf::SoundBuffer buffer;
		std::vector<sf::Sound> players{};
	};

	Sound::Sound(std::unique_ptr<SoundImpl> data) : _impl(std::move(data)) {}

	Sound::~Sound() = default;

	void Sound::play() const {
		_impl->play();
	}

	std::unique_ptr<Sound> createSound(const std::string& path) {
		auto sound = std::make_unique<Sound::SoundImpl>();

		const bool loaded = sound->buffer.loadFromFile(path);
		if (!loaded) return nullptr;

		return std::make_unique<Sound>(std::move(sound));
	}
}
