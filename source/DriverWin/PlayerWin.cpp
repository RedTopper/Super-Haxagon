#include "DriverWin/PlayerWin.hpp"

namespace SuperHaxagon {
	PlayerWin::PlayerWin(sf::SoundBuffer& buffer, bool loaded) : loaded(loaded) {
		if (loaded) sound.setBuffer(buffer);
	}

	PlayerWin::~PlayerWin() {
		PlayerWin::stop();
	}

	void PlayerWin::setLoop(bool _loop) {
		loop = _loop;
	}

	void PlayerWin::play() {
		if (!loaded) return;
		sound.setLoop(loop);
		sound.play();
	}

	void PlayerWin::stop() {
		if (!loaded) return;
		sound.stop();
	}

	bool PlayerWin::isDone() {
		if (!loaded) return true;
		return sound.getStatus() == sf::SoundSource::Stopped;
	}
}
