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
		static constexpr double GAME_OVER_ACCELERATION_RATE = 0.1;
		static constexpr int FRAMES_PER_GAME_OVER = 60;
		static constexpr int PULSE_TIME = 75;

		Over(Game& game, LevelFactory& factory, std::unique_ptr<Level> level, const double score, int levelIndex);
		Over(Over&) = delete;
		~Over() override;

		std::unique_ptr<State> update(double dilation) override;
		void drawTop(double scale) override;
		void drawBot(double scale) override;
		void enter() override;

	private:
		Game& _game;
		Platform& _platform;
		LevelFactory& _factory;
		std::unique_ptr<Level> _level;

		bool _high = false;
		double _score = 0;
		double _frames = 0;
		double _offset = 1.0;
		int _levelIndex = 0;
	};
}

#endif //SUPER_HAXAGON_OVER_HPP
