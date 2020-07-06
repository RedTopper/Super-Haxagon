#include "DriverWin/AudioWin.hpp"
#include "DriverWin/PlayerWin.hpp"

namespace SuperHaxagon {
	AudioWin::AudioWin(const std::string& path, Stream _stream) {
		if (_stream == Stream::DIRECT) {
			buffer = std::make_unique<sf::SoundBuffer>();
			if (buffer->loadFromFile(path + ".wav")) {
				stream = Stream::DIRECT;
			}
		} else if (_stream == Stream::INDIRECT) {
			music = std::make_unique<sf::Music>();
			if (music->openFromFile(path + ".ogg")) {
				stream = Stream::INDIRECT;
			}
		}
	}

	AudioWin::~AudioWin() = default;

	std::unique_ptr<Player> AudioWin::instantiate() {
		if (stream == Stream::DIRECT) {
			auto sound = std::make_unique<sf::Sound>();
			sound->setBuffer(*buffer);
			return std::make_unique<PlayerWin>(std::move(sound));
		} else if (stream == Stream::INDIRECT) {
			stream = Stream::NONE;
			return std::make_unique<PlayerWin>(std::move(music));
		}

		return nullptr;
	}
}
