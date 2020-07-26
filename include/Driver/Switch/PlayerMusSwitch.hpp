#ifndef SUPER_HAXAGON_PLAYER_MUS_SWITCH_H
#define SUPER_HAXAGON_PLAYER_MUS_SWITCH_H

#include "Driver/Player.hpp"

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
		double getTime() const override;
		double getNow() const;

	private:
		bool _loop = false;
		Mix_Music* _music;

		// Timing controls
		double _start = 0;
		double _diff = 0;
	};
}


#endif //SUPER_HAXAGON_PLAYER_MUS_SWITCH_H
