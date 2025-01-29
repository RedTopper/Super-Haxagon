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
		static constexpr float GAME_OVER_ROT_SPEED = TAU/400.0f;
		static constexpr float GAME_OVER_ACCELERATION_RATE = 0.1f;
		static constexpr int FRAMES_PER_GAME_OVER = 60;
		static constexpr int PULSE_TIME = 75;

		Over(Game& game, std::unique_ptr<Level> level, LevelFactory& selected, float score, std::string text);
		Over(Over&) = delete;
		~Over() override;

		std::unique_ptr<State> update(float dilation) override;
		void drawTop(float scale) override;
		void drawBot(float scale) override;
		void enter() override;

	private:
		Game& _game;
		Platform& _platform;
		LevelFactory& _selected;
		std::unique_ptr<Level> _level;
		std::string _text = "GAME OVER";

		bool _high = false;
		float _score = 0;
		float _frames = 0;
		float _offset = 1.0;
	};
}

#endif //SUPER_HAXAGON_OVER_HPP
