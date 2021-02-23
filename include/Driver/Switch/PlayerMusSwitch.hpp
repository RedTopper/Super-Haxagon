#ifndef SUPER_HAXAGON_PLAYER_MUS_SWITCH_H
#define SUPER_HAXAGON_PLAYER_MUS_SWITCH_H

#include "Core/Player.hpp"

#include <SDL2/SDL_mixer.h>

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


#endif //SUPER_HAXAGON_PLAYER_MUS_SWITCH_H
