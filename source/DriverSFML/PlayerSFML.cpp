#include <SFML/Audio/Sound.hpp>
#include <SFML/Audio/Music.hpp>

#include "DriverSFML/PlayerSFML.hpp"

namespace SuperHaxagon {
	PlayerSFML::PlayerSFML(std::unique_ptr<sf::SoundSource> source) : _source(std::move(source)) {}

	PlayerSFML::~PlayerSFML() {
		_source->stop();
	}

	void PlayerSFML::setLoop(const bool loop) {
		if (auto* sound = dynamic_cast<sf::Sound*>(_source.get())) {
			sound->setLoop(loop);
		} else if (auto* music = dynamic_cast<sf::Music*>(_source.get())) {
			music->setLoop(loop);
		}
	}

	void PlayerSFML::play() {
		_source->play();
	}

	bool PlayerSFML::isDone() {
		return _source->getStatus() == sf::SoundSource::Stopped;
	}

	double PlayerSFML::getVelocity() {
		return 0;
	}
}
