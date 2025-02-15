#ifndef SUPER_HAXAGON_AUDIO_PLAYER_SOUND_HPP
#define SUPER_HAXAGON_AUDIO_PLAYER_SOUND_HPP

#include <memory>

namespace SuperHaxagon {
	class Sound {
	public:
		struct SoundImpl;
		explicit Sound(std::unique_ptr<SoundImpl> impl);
		Sound(Sound&) = delete;
		~Sound();

		void play() const;

	private:
		std::unique_ptr<SoundImpl> _impl;
	};
}

#endif //SUPER_HAXAGON_AUDIO_PLAYER_SOUND_HPP
