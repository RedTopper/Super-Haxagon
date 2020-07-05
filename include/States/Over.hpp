#ifndef SUPER_HAXAGON_OVER_HPP
#define SUPER_HAXAGON_OVER_HPP

#include "State.hpp"
#include "Core/Structs.hpp"

namespace SuperHaxagon {
	class Level;
	class LevelFactory;
	class Game;
	class Platform;

	class Over : public State {
	public:
		static constexpr double GAME_OVER_ROT_SPEED = TAU/240.0;
		static constexpr double GAME_OVER_ACCELERATION_RATE = 1.1;
		static constexpr int FRAMES_PER_GAME_OVER = 60;
		static constexpr int PULSE_TIME = 75;

		Over(Game& game, LevelFactory& factory, std::unique_ptr<Level> level, int score);
		Over(Over&) = delete;

		std::unique_ptr<State> update() override;
		void drawTop() override;
		void drawBot() override;
		void enter() override;

	private:
		Game& game;
		Platform& platform;
		LevelFactory& factory;
		std::unique_ptr<Level> level;

		bool high = false;
		int frames = 0;
		int score = 0;
		double offset = 0;
	};
}

#endif //SUPER_HAXAGON_OVER_HPP
