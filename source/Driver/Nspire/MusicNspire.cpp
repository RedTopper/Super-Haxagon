#include "Driver/Music.hpp"

namespace SuperHaxagon {
	struct Music::MusicData {
		MusicData(float max, float* timeSinceLastGet) : maxTime(max), timeSinceLastGet(timeSinceLastGet) {}
		float maxTime{};
		float time{};
		float* timeSinceLastGet;
		bool playing = false;
		bool isDone = true;
	};

	std::unique_ptr<Music> createMusic(float max, float* timeSinceLastGet) {
		auto data = std::make_unique<Music::MusicData>(max, timeSinceLastGet);
		return std::make_unique<Music>(std::move(data));
	}

	Music::Music(std::unique_ptr<Music::MusicData> data) : _data(std::move(data)) {}

	Music::~Music() = default;

	void Music::setLoop(bool) const {}

	void Music::play() const {
		if (isDone()) {
			_data->time = 0.0f;
		}

		_data->playing = true;
	}
	
	void Music::pause() const {
		_data->playing = false;
	}

	bool Music::isDone() const {
		return _data->time > _data->maxTime + 5.0f;
	}
	
	float Music::getTime() const {
		if (_data->playing) {
			_data->time += *_data->timeSinceLastGet;
			*_data->timeSinceLastGet = 0;
		}

		return _data->time;
	}
}
