#ifndef SUPER_HAXAGON_AUDIO_PLAYER_MUSIC_HPP
#define SUPER_HAXAGON_AUDIO_PLAYER_MUSIC_HPP

// SPDX-FileCopyrightText: 2025 AJ Walter
// SPDX-License-Identifier: GPL-3.0-or-later OR MIT

#include <memory>

namespace SuperHaxagon {


	class Music {
	public:
		struct MusicImpl;

		explicit Music(std::unique_ptr<MusicImpl> impl);
		Music(Music&) = delete;
		~Music();

		// Some platforms need to update their state
		// every frame. Call this function to do that.
		void update() const;

		void setLoop(bool loop) const;
		void play() const;
		void pause() const;
		bool isDone() const;
		float getTime() const;

	private:
		std::unique_ptr<MusicImpl> _impl;
	};
}

#endif //SUPER_HAXAGON_AUDIO_PLAYER_MUSIC_HPP
