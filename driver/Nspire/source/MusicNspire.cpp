// SPDX-FileCopyrightText: 2025 AJ Walter
// SPDX-License-Identifier: GPL-3.0-or-later OR MIT

#include "Driver/Music.hpp"

namespace SuperHaxagon {
	struct Music::MusicImpl {
		MusicImpl(float max, float* timeSinceLastGet) : maxTime(max), timeSinceLastGet(timeSinceLastGet) {}

		void play() {
			*timeSinceLastGet = 0;

			if (isDone()) {
				time = 0.0f;
			}

			playing = true;
		}

		void update() {
			// Update our current time from the platform's info
			if (playing) {
				time += *timeSinceLastGet;
				*timeSinceLastGet = 0;
			}

			// Loop if needed
			if (loop && isDone()) {
				play();
			}
		}

		bool isDone() const {
			return time > maxTime + 5.0f;
		}

		float maxTime;
		float time = 0.0;
		float* timeSinceLastGet;
		bool playing = false;
		bool loop = false;
	};

	Music::Music(std::unique_ptr<Music::MusicImpl> impl) : _impl(std::move(impl)) {}

	Music::~Music() = default;

	void Music::update() const {
		_impl->update();
	}

	void Music::setLoop(bool loop) const {
		_impl->loop = loop;
	}

	void Music::play() const {
		_impl->play();
	}
	
	void Music::pause() const {
		_impl->playing = false;
	}

	bool Music::isDone() const {
		return _impl->isDone();
	}
	
	float Music::getTime() const {
		return _impl->time;
	}

	std::unique_ptr<Music> createMusic(float max, float* timeSinceLastGet) {
		auto data = std::make_unique<Music::MusicImpl>(max, timeSinceLastGet);
		return std::make_unique<Music>(std::move(data));
	}
}
