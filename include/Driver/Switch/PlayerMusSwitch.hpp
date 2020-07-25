#ifndef SUPER_HAXAGON_PLAYER_MUS_SWITCH_H
#define SUPER_HAXAGON_PLAYER_MUS_SWITCH_H

#include <SDL2/SDL_mixer.h>

#include "Driver/Player.hpp"

namespace SuperHaxagon {
	class PlayerMusSwitch : public Player {
	public:
		explicit PlayerMusSwitch(Mix_Music* music);
		~PlayerMusSwitch() override;

		void setChannel(int) override {};
		void setLoop(bool loop) override;

		void play() override;
		void pause() override;
		bool isDone() const override;
		double getTime() const override;

	private:
		bool _loop = false;
		bool _launched = false;
		Mix_Music* _music;
	};
}


#endif //SUPER_HAXAGON_PLAYER_MUS_SWITCH_H
