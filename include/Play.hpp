#ifndef SUPER_HAXAGON_PLAY_HPP
#define SUPER_HAXAGON_PLAY_HPP

#include <deque>

#include "State.hpp"
#include "Pattern.hpp"

namespace SuperHaxagon {
	class Game;
	class Level;
	class Platform;
	class Play : public State {
	public:
		static constexpr double DIFFICULTY_MULTIPLIER = 1.1;
		static constexpr int FLIP_FRAMES_MIN = 120;
		static constexpr int FLIP_FRAMES_MAX = 500;
		static constexpr int TOTAL_PATTERNS_AT_ONE_TIME = 5;
		static constexpr int FRAMES_PER_CHANGE_SIDE = 36;

		Play(Game& game, Level& level);
		std::unique_ptr<State> update() override;
		void draw() override;


	private:
		std::deque<PatternActive> patterns;

		Game& game;
		Platform& platform;
		Level& level;

		double multiplier = 1.0; // Current direction and speed of rotation
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

#endif //SUPER_HAXAGON_PLAY_HPP
