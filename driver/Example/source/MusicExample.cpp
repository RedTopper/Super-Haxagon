// SPDX-FileCopyrightText: 2025 AJ Walter, see driver/Example/README.md for MIT usage.
// SPDX-License-Identifier: GPL-3.0-or-later OR MIT

#include "Driver/Music.hpp"

namespace SuperHaxagon {
	struct Music::MusicImpl {
		MusicImpl() {}

		bool _loop;
	};

	Music::Music(std::unique_ptr<MusicImpl> data) : _impl(std::move(data)) {}

	Music::~Music() = default;

	void Music::update() const {}

	void Music::setLoop(const bool loop) const {
		_impl->_loop = loop;
	}

	void Music::play() const {}

	void Music::pause() const {}

	bool Music::isDone() const {
		return true;
	}

	float Music::getTime() const {
		// Used for getting the offset in the Metadata, for timing
		// pulses to the beat of the music. Should be set to something
		// other than 0.0f
		return 0.0f;
	}

	std::unique_ptr<Music> createMusic() {
		// You can pass in either a path, or maybe a fully functioning sound object.
		return std::make_unique<Music>(std::make_unique<Music::MusicImpl>());
	}
}
