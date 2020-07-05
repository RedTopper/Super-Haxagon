#ifndef SUPER_HAXAGON_PLAYER_WIN_HPP
#define SUPER_HAXAGON_PLAYER_WIN_HPP

#include "Player.hpp"

namespace SuperHaxagon {
	class PlayerWin : public Player {
	public:
		PlayerWin();
		~PlayerWin();

		void setChannel(int channel) override;
		void setLoop(bool loop) override;

		void play() override;
		void stop() override;
		bool isDone() override;
	};
}

#endif //SUPER_HAXAGON_PLAYER_WIN_HPP
