#ifndef SUPER_HAXAGON_PLAY_HPP
#define SUPER_HAXAGON_PLAY_HPP

#include "State.hpp"

#include "Core/Vector.hpp"

namespace SuperHaxagon {
	class Game;
	class Level;
	class LevelFactory;
	class Platform;

	class Play : public State {
	public:
		static constexpr int PULSE_TIME = 75;
		static constexpr float PULSE_TIMES = 2.0f;
		static constexpr float SKEW_MAX = PI/4.0;
		static constexpr float SKEW_MIN = PI/16.0;
		static constexpr float SKEW_MIN_FRAMES = 120.0f;

		Play(Game& game, LevelFactory& factory, LevelFactory& selected, float startScore, float rotation, float cursorPos);
		Play(Play&) = delete;
		~Play() override;

		std::unique_ptr<State> update(float dilation) override;
		void drawGame(SurfaceGame& surface, SurfaceGame* shadows) override;
		void drawTopUI(SurfaceUI&) override {}
		void drawBotUI(SurfaceUI& surface) override;
		void enter() override;
		void exit() override;

	private:
		Game& _game;
		Platform& _platform;
		LevelFactory& _factory;
		LevelFactory& _selected;
		std::unique_ptr<Level> _level;

		float _scalePrev = 0;
		float _scoreWidth = 0;
		float _score = 0;
		bool _skewing = false;
	};
}

#endif //SUPER_HAXAGON_PLAY_HPP
