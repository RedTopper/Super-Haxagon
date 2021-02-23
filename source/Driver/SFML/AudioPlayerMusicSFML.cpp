#include "Driver/SFML/AudioPlayerMusicSFML.hpp"

namespace SuperHaxagon {
	AudioPlayerMusicSFML::AudioPlayerMusicSFML(std::unique_ptr<sf::Music> music) : _music(std::move(music)) {}

	AudioPlayerMusicSFML::~AudioPlayerMusicSFML() {
		_music->stop();
	}

	void AudioPlayerMusicSFML::setLoop(const bool loop) {
		_music->setLoop(loop);
	}

	void AudioPlayerMusicSFML::play() {
		_music->play();
	}

	void AudioPlayerMusicSFML::pause() {
		_music->pause();
	}

	bool AudioPlayerMusicSFML::isDone() const {
		return _music->getStatus() == sf::SoundSource::Stopped;
	}

	float AudioPlayerMusicSFML::getTime() const {
		return _music->getPlayingOffset().asSeconds();
	}
}
