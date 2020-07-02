#ifndef SUPER_HAXAGON_PLAYER_3DS_H
#define SUPER_HAXAGON_PLAYER_3DS_H

#include "Player.h"

namespace SuperHaxagon {
	class Player3DS : public Player {
		void setChannel(int channel) override;

		void play() override;

		void stop() override;

		bool isPlaying() override;
	};
}

#endif //SUPER_HAXAGON_PLAYER_3DS_H
