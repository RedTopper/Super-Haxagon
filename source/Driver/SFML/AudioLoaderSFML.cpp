#include "Driver/SFML/AudioLoaderSFML.hpp"

#include "Driver/SFML/AudioPlayerMusicSFML.hpp"
#include "Driver/SFML/AudioPlayerSoundSFML.hpp"

namespace SuperHaxagon {
	AudioLoaderSFML::AudioLoaderSFML(const std::string& path, const Stream stream) {
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

	AudioLoaderSFML::~AudioLoaderSFML() = default;

	std::unique_ptr<AudioPlayer> AudioLoaderSFML::instantiate() {
		if (_stream == Stream::DIRECT) {
			auto sound = std::make_unique<sf::Sound>();
			sound->setBuffer(*_buffer);
			return std::make_unique<AudioPlayerSoundSFML>(std::move(sound));
		}

		if (_stream == Stream::INDIRECT) {
			_stream = Stream::NONE;
			return std::make_unique<AudioPlayerMusicSFML>(std::move(_music));
		}

		return nullptr;
	}
}
