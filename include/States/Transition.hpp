#ifndef SUPER_HAXAGON_TRANSITION_HPP
#define SUPER_HAXAGON_TRANSITION_HPP

#include "State.hpp"

#include "Core/Structs.hpp"

namespace SuperHaxagon {
	class Level;
	class LevelFactory;
	class Game;
	class Platform;

	class Transition : public State {
	public:
		static constexpr double TRANSITION_ACCELERATION_RATE = 0.1;
		static constexpr int TRANSITION_FRAMES = 120;
		
		Transition(Game& game, std::unique_ptr<Level> level, double score, int levelIndex);
		Transition(Transition&) = delete;
		~Transition() override;

		std::unique_ptr<State> update(double dilation) override;
		void drawTop(double scale) override;
		void drawBot(double scale) override;
		void enter() override;

	private:
		Game& _game;
		Platform& _platform;
		std::unique_ptr<Level> _level;

		double _score = 0;
		double _frames = 0;
		double _offset = 1.0;
		int _levelIndex = 0;
	};
}

#endif //SUPER_HAXAGON_TRANSITION_HPP
