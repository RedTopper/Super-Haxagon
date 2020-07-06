#ifndef SUPER_HAXAGON_PLAY_HPP
#define SUPER_HAXAGON_PLAY_HPP

#include <deque>

#include "State.hpp"
#include "Core/Structs.hpp"

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

		std::unique_ptr<State> update(double dilation) override;
		void drawTop() override;
		void drawBot() override;
		void enter() override;

	private:
		Game& game;
		Platform& platform;
		LevelFactory& factory;
		std::unique_ptr<Level> level;

		double score = 0;
	};
}

#endif //SUPER_HAXAGON_PLAY_HPP
