#ifndef SUPER_HAXAGON_AUDIO_PLAYER_MUS_SWITCH_HPP
#define SUPER_HAXAGON_AUDIO_PLAYER_MUS_SWITCH_HPP

#include "Core/AudioPlayer.hpp"

#include <SDL2/SDL_mixer.h>

namespace SuperHaxagon {
	class AudioPlayerMusSwitch : public AudioPlayer {
	public:
		explicit AudioPlayerMusSwitch(Mix_Music* music);
		~AudioPlayerMusSwitch() override;

		void setChannel(int) override {}
		void setLoop(bool loop) override;

		void play() override;
		void pause() override;
		bool isDone() const override;
		float getTime() const override;
		float getNow() const;

	private:
		bool _loop = false;
		Mix_Music* _music;

		// Timing controls
		float _start = 0;
		float _diff = 0;
	};
}


#endif //SUPER_HAXAGON_AUDIO_PLAYER_MUS_SWITCH_HPP
