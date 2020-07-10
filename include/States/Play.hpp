#ifndef SUPER_HAXAGON_PLAY_HPP
#define SUPER_HAXAGON_PLAY_HPP

#include <deque>

#include "State.hpp"
#include "Core/Structs.hpp"

namespace SuperHaxagon {
	class Game;
	class Audio;
	class Level;
	class LevelFactory;
	class Platform;

	class Play : public State {
	public:
		static constexpr int PULSE_TIME = 75;
		static constexpr double PULSE_TIMES = 2.0;

		Play(Game& game, LevelFactory& factory, int levelIndex);
		Play(Play&) = delete;
		~Play() override;

		std::unique_ptr<State> update(double dilation) override;
		void drawTop(double scale) override;
		void drawBot(double scale) override;
		void enter() override;
		void exit() override;

	private:
		Game& _game;
		Platform& _platform;
		LevelFactory& _factory;
		std::unique_ptr<Level> _level;
		std::unique_ptr<Audio> _bgm;

		double _scalePrev = 0;
		double _scoreWidth = 0;
		double _score = 0;
		int _levelIndex = 0;
	};
}

#endif //SUPER_HAXAGON_PLAY_HPP
