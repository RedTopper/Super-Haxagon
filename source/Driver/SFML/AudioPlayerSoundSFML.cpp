#include "Driver/SFML/AudioPlayerSoundSFML.hpp"

namespace SuperHaxagon {
	AudioPlayerSoundSFML::AudioPlayerSoundSFML(std::unique_ptr<sf::Sound> sound) : _sound(std::move(sound)) {}

	AudioPlayerSoundSFML::~AudioPlayerSoundSFML() {
		_sound->stop();
	}

	void AudioPlayerSoundSFML::setLoop(const bool loop) {
		_sound->setLoop(loop);
	}

	void AudioPlayerSoundSFML::play() {
		_sound->play();
	}

	bool AudioPlayerSoundSFML::isDone() const {
		return _sound->getStatus() == sf::SoundSource::Stopped;
	}

	float AudioPlayerSoundSFML::getTime() const {
		return 0;
	}
}
