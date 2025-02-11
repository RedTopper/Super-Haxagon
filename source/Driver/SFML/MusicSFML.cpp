#include "Driver/Music.hpp"

#include <SFML/Audio.hpp>

namespace SuperHaxagon {
	struct Music::MusicData {
		explicit MusicData(sf::Music& music) : music(music) {}
		sf::Music& music;
	};

	std::unique_ptr<Music> createMusic(sf::Music& music) {
		return std::make_unique<Music>(std::make_unique<Music::MusicData>(music));
	}

	Music::Music(std::unique_ptr<MusicData> data) : _data(std::move(data)) {}

	Music::~Music() {
		_data->music.stop();
	}

	void Music::setLoop(const bool loop) const {
		_data->music.setLoop(loop);
	}

	void Music::play() const {
		_data->music.play();
	}

	void Music::pause() const {
		_data->music.pause();
	}

	bool Music::isDone() const {
		return _data->music.getStatus() == sf::SoundSource::Stopped;
	}

	float Music::getTime() const {
		return _data->music.getPlayingOffset().asSeconds();
	}
}
