#ifndef SUPER_HAXAGON_PLAY_H
#define SUPER_HAXAGON_PLAY_H

#include <deque>

#include "State.h"
#include "Game.h"

namespace SuperHaxagon {
	class Play : public State {
	public:
		static const int FLIP_FRAMES_MAX = 500;
		static const int TOTAL_PATTERNS_AT_ONE_TIME = 5;

		Play(Game& game, Level& level);
		std::unique_ptr<State> update() override;
		void draw() override;

	private:
		Game& game;
		Platform& platform;
		Level& level;

		std::deque<???????> patterns;

		double cursorPos;
		double rotation;

		int delayFrame; //tweening between side switches
		int flipFrame; //amount of frames left until flip
		int tweenFrame; //tweening colors
		int indexBG1;
		int indexBG2;
		int indexFG;
		int nextIndexBG1;
		int nextIndexBG2;
		int nextIndexFG;
		int score;
	};
}

#endif //SUPER_HAXAGON_PLAY_H
