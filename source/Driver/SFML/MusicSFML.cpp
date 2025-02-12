#include "Driver/Music.hpp"

#include <SFML/Audio.hpp>

namespace SuperHaxagon {
	struct Music::MusicData {
		explicit MusicData(std::unique_ptr<sf::Music> music) : music(std::move(music)) {}
		std::unique_ptr<sf::Music> music;
	};

	std::unique_ptr<Music> createMusic(std::unique_ptr<sf::Music> music) {
		return std::make_unique<Music>(std::make_unique<Music::MusicData>(std::move(music)));
	}

	Music::Music(std::unique_ptr<MusicData> data) : _data(std::move(data)) {}

	Music::~Music() {
		_data->music->stop();
	}

	// SFML: Nothing to do on update.
	void Music::update() const {}

	void Music::setLoop(const bool loop) const {
		_data->music->setLoop(loop);
	}

	void Music::play() const {
		_data->music->play();
	}

	void Music::pause() const {
		_data->music->pause();
	}

	bool Music::isDone() const {
		return _data->music->getStatus() == sf::SoundSource::Stopped;
	}

	float Music::getTime() const {
		return _data->music->getPlayingOffset().asSeconds();
	}
}
