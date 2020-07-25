#include <SFML/Audio/Sound.hpp>
#include <SFML/Audio/Music.hpp>

#include "Driver/SFML/PlayerMusicSFML.hpp"

namespace SuperHaxagon {
	PlayerMusicSFML::PlayerMusicSFML(std::unique_ptr<sf::Music> music) : _music(std::move(music)) {}

	PlayerMusicSFML::~PlayerMusicSFML() {
		_music->stop();
	}

	void PlayerMusicSFML::setLoop(const bool loop) {
		_music->setLoop(loop);
	}

	void PlayerMusicSFML::play() {
		_music->play();
	}

	void PlayerMusicSFML::pause() {
		_music->pause();
	}

	bool PlayerMusicSFML::isDone() const {
		return _music->getStatus() == sf::SoundSource::Stopped;
	}

	double PlayerMusicSFML::getTime() const {
		return _music->getPlayingOffset().asSeconds();
	}
}
