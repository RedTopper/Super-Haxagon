#ifndef SUPER_HAXAGON_OVER_HPP
#define SUPER_HAXAGON_OVER_HPP

#include "State.hpp"
#include "Structs.hpp"

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

		Over(Game& game, const LevelFactory& factory, std::unique_ptr<Level> level);
		Over(Over&) = delete;

		std::unique_ptr<State> update() override;
		void draw() override;

	private:
		const LevelFactory& factory;
		std::unique_ptr<Level> level;
		Game& game;
		Platform& platform;

		int frames = 0;
		double offset = 0;
	};
}

#endif //SUPER_HAXAGON_OVER_HPP
