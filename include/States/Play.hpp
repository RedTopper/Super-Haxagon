#ifndef SUPER_HAXAGON_PLAY_HPP
#define SUPER_HAXAGON_PLAY_HPP

#include <deque>

#include "State.hpp"
#include "Structs.hpp"

namespace SuperHaxagon {
	class Game;
	class Level;
	class LevelFactory;
	class Platform;

	class Play : public State {
	public:
		static constexpr int PULSE_TIME = 75;
		static constexpr double PULSE_TIMES = 2.0;

		Play(Game& game, LevelFactory& level);
		Play(Play&) = delete;

		std::unique_ptr<State> update() override;
		void draw() override;
		void enter() override;

	private:
		LevelFactory& factory;
		std::unique_ptr<Level> level;

		Game& game;
		Platform& platform;

		int score{};
	};
}

#endif //SUPER_HAXAGON_PLAY_HPP
