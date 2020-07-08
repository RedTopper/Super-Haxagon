#include <SFML/Audio/Sound.hpp>

#include "DriverWin/AudioWin.hpp"
#include "DriverWin/PlayerWin.hpp"

namespace SuperHaxagon {
	AudioWin::AudioWin(const std::string& path, const Stream stream) {
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

	AudioWin::~AudioWin() = default;

	std::unique_ptr<Player> AudioWin::instantiate() {
		if (_stream == Stream::DIRECT) {
			auto sound = std::make_unique<sf::Sound>();
			sound->setBuffer(*_buffer);
			return std::make_unique<PlayerWin>(std::move(sound));
		}

		if (_stream == Stream::INDIRECT) {
			_stream = Stream::NONE;
			return std::make_unique<PlayerWin>(std::move(_music));
		}

		return nullptr;
	}
}
