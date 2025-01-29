#ifndef SUPER_HAXAGON_AUDIO_PLAYER_SFX_SWITCH_HPP
#define SUPER_HAXAGON_AUDIO_PLAYER_SFX_SWITCH_HPP

#include "Core/AudioPlayer.hpp"

#include <SDL2/SDL_mixer.h>

namespace SuperHaxagon {
	class AudioPlayerSfxSwitch : public AudioPlayer {
	public:
		explicit AudioPlayerSfxSwitch(Mix_Chunk* sfx);
		~AudioPlayerSfxSwitch() override;

		void setChannel(int) override {}
		void setLoop(bool) override {}

		void play() override;
		void pause() override {}
		bool isDone() const override {return true;}
		float getTime() const override {return 0.0;}

	private:
		Mix_Chunk* _sfx;
	};
}


#endif //SUPER_HAXAGON_PLAYER_SFX_SWITCH_HPP
