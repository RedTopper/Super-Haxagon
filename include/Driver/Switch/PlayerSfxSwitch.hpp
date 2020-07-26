#ifndef SUPER_HAXAGON_PLAYER_SFX_SWITCH_H
#define SUPER_HAXAGON_PLAYER_SFX_SWITCH_H

#include "Driver/Player.hpp"

#include <SDL2/SDL_mixer.h>

namespace SuperHaxagon {
	class PlayerSfxSwitch : public Player {
	public:
		explicit PlayerSfxSwitch(Mix_Chunk* sfx);
		~PlayerSfxSwitch() override;

		void setChannel(int) override {};
		void setLoop(bool) override {};

		void play() override;
		void pause() override {};
		bool isDone() const override {return true;}
		double getTime() const override {return 0.0;}

	private:
		Mix_Chunk* _sfx;
	};
}


#endif //SUPER_HAXAGON_PLAYER_SFX_SWITCH_H
