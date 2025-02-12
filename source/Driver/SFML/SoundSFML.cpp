#include "Driver/Sound.hpp"

#include "DataSFML.hpp"

#include <SFML/Audio.hpp>

namespace SuperHaxagon {
	struct Sound::SoundData {
		~SoundData() {
			for (auto& player : players) {
				player.stop();
			}
		}

		void clearFinished() {
			for (auto it = players.begin(); it != players.end();) {
				const auto& playing = *it;
				if (playing.getStatus() == sf::SoundSource::Stopped) {
					it = players.erase(it);
				} else {
					++it;
				}
			}
		}

		sf::SoundBuffer buffer;
		std::vector<sf::Sound> players{};
	};

	std::unique_ptr<Sound> createSound(const std::string& path) {
		auto sound = std::make_unique<Sound::SoundData>();

		const bool loaded = sound->buffer.loadFromFile(path);
		if (!loaded) return nullptr;

		return std::make_unique<Sound>(std::move(sound));
	}

	Sound::Sound(std::unique_ptr<SoundData> data) : _data(std::move(data)) {}

	Sound::~Sound() = default;

	void Sound::play() const {
		_data->clearFinished();
		_data->players.emplace_back(_data->buffer).play();
	}
}
