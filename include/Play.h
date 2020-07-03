#ifndef SUPER_HAXAGON_PLAY_H
#define SUPER_HAXAGON_PLAY_H

#include "State.h"
#include "Game.h"

namespace SuperHaxagon {
	class Play : public State {
	public:
		explicit Play(Game& game);
		std::unique_ptr<State> update() override;
		void draw() override;

	private:
		Game& game;
		Platform& platform;
	};
}

#endif //SUPER_HAXAGON_PLAY_H
