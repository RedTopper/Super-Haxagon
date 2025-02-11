#include "Driver/Sound.hpp"

#include "DataSFML.hpp"

#include <SFML/Audio.hpp>

namespace SuperHaxagon {
	struct Sound::SoundData {
		sf::Sound sound;
	};

	std::unique_ptr<Sound> createSound(const sf::SoundBuffer& buffer) {
		auto sound = std::make_unique<Sound::SoundData>();
		sound->sound.setBuffer(buffer);
		return std::make_unique<Sound>(std::move(sound));
	}

	Sound::Sound(std::unique_ptr<SoundData> data) : _data(std::move(data)) {}

	Sound::~Sound() {
		_data->sound.stop();
	}

	// SFML does not use channels
	// ReSharper disable once CppMemberFunctionMayBeStatic
	void Sound::setChannel(int) {}

	void Sound::play() const {
		_data->sound.play();
	}

	bool Sound::isDone() const {
		return _data->sound.getStatus() == sf::SoundSource::Stopped;
	}
}
