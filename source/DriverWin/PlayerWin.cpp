#include "DriverWin/PlayerWin.hpp"

namespace SuperHaxagon {
	PlayerWin::PlayerWin(std::unique_ptr<sf::SoundSource> source) : source(std::move(source)) {}

	PlayerWin::~PlayerWin() {
		PlayerWin::stop();
	}

	void PlayerWin::setLoop(bool loop) {
		if (auto sound = dynamic_cast<sf::Sound*>(source.get())) {
			sound->setLoop(loop);
		} else if (auto music = dynamic_cast<sf::Music*>(source.get())) {
			music->setLoop(loop);
		}
	}

	void PlayerWin::play() {
		source->play();
	}

	void PlayerWin::stop() {
		source->stop();
	}

	bool PlayerWin::isDone() {
		return source->getStatus() == sf::SoundSource::Stopped;
	}
}
