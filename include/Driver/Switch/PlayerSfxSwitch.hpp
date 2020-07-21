#ifndef SUPER_HAXAGON_PLAYER_SFX_SWITCH_H
#define SUPER_HAXAGON_PLAYER_SFX_SWITCH_H

#include <SDL2/SDL_mixer.h>

#include "Driver/Player.hpp"

namespace SuperHaxagon {
	class PlayerSfxSwitch : public Player {
	public:
		explicit PlayerSfxSwitch(Mix_Chunk* sfx);
		~PlayerSfxSwitch() override = default;

	private:
		void setChannel(int) override {};
		void setLoop(bool) override {};

		void play() override;
		bool isDone() override {return true;}
		double getVelocity() override {return 0.0;}

		Mix_Chunk* _sfx;
	};
}


#endif //SUPER_HAXAGON_PLAYER_SFX_SWITCH_H
