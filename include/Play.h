#ifndef SUPER_HAXAGON_PLAY_H
#define SUPER_HAXAGON_PLAY_H

#include <deque>

#include "State.h"
#include "Pattern.h"

namespace SuperHaxagon {
	class Game;
	class Level;
	class Platform;
	class Play : public State {
	public:
		static const int FLIP_FRAMES_MAX = 500;
		static const int TOTAL_PATTERNS_AT_ONE_TIME = 5;
		static const int FRAMES_PER_CHANGE_SIDE = 36;

		Play(Game& game, Level& level);
		std::unique_ptr<State> update() override;
		void draw() override;

	private:
		std::deque<PatternActive> patterns;

		Game& game;
		Platform& platform;
		Level& level;

		double cursorPos{};
		double rotation{};
		int lastSides;
		int currentSides;

		int delayFrame{}; //tweening between side switches
		int tweenFrame{}; //tweening colors
		int flipFrame = FLIP_FRAMES_MAX; //amount of frames left until flip

		int score{};
		int indexBG1{};
		int indexBG2{};
		int indexFG{};
		int nextIndexBG1;
		int nextIndexBG2;
		int nextIndexFG;

	};
}

#endif //SUPER_HAXAGON_PLAY_H
