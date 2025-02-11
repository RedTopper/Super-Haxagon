#ifndef SUPER_HAXAGON_AUDIO_PLAYER_SOUND_HPP
#define SUPER_HAXAGON_AUDIO_PLAYER_SOUND_HPP

#include <memory>

namespace SuperHaxagon {
	class Sound {
	public:
		struct SoundData;
		explicit Sound(std::unique_ptr<SoundData> data);
		Sound(Sound&) = delete;
		~Sound();

		void setChannel(int channel);
		void play() const;
		bool isDone() const;

	private:
		std::unique_ptr<SoundData> _data;
	};
}

#endif //SUPER_HAXAGON_AUDIO_PLAYER_SOUND_HPP
