#ifndef SUPER_HAXAGON_PLAYER_MUS_SWITCH_H
#define SUPER_HAXAGON_PLAYER_MUS_SWITCH_H

#include <SDL2/SDL_mixer.h>

#include "Driver/Player.hpp"

namespace SuperHaxagon {
	class PlayerMusSwitch : public Player {
	public:
		explicit PlayerMusSwitch(Mix_Music* music);
		~PlayerMusSwitch() override;

	private:
		void setChannel(int) override {};
		void setLoop(bool loop) override;

		void play() override;
		bool isDone() override;
		double getVelocity() override;

		bool _loop;
		Mix_Music* _music;
	};
}


#endif //SUPER_HAXAGON_PLAYER_MUS_SWITCH_H
