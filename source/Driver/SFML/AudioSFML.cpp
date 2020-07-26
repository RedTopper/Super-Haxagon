#include "Driver/SFML/AudioSFML.hpp"

#include "Driver/SFML/PlayerMusicSFML.hpp"
#include "Driver/SFML/PlayerSoundSFML.hpp"

namespace SuperHaxagon {
	AudioSFML::AudioSFML(const std::string& path, const Stream stream) {
		if (stream == Stream::DIRECT) {
			_buffer = std::make_unique<sf::SoundBuffer>();
			if (_buffer->loadFromFile(path + ".wav")) {
				_stream = Stream::DIRECT;
			}
		} else if (stream == Stream::INDIRECT) {
			_music = std::make_unique<sf::Music>();
			if (_music->openFromFile(path + ".ogg")) {
				_stream = Stream::INDIRECT;
			}
		}
	}

	AudioSFML::~AudioSFML() = default;

	std::unique_ptr<Player> AudioSFML::instantiate() {
		if (_stream == Stream::DIRECT) {
			auto sound = std::make_unique<sf::Sound>();
			sound->setBuffer(*_buffer);
			return std::make_unique<PlayerSoundSFML>(std::move(sound));
		}

		if (_stream == Stream::INDIRECT) {
			_stream = Stream::NONE;
			return std::make_unique<PlayerMusicSFML>(std::move(_music));
		}

		return nullptr;
	}
}
