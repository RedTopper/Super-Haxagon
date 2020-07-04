#ifndef SUPER_HAXAGON_PLAY_HPP
#define SUPER_HAXAGON_PLAY_HPP

#include <deque>

#include "State.hpp"
#include "Pattern.hpp"
#include "Structs.hpp"

namespace SuperHaxagon {
	class Game;
	class Level;
	class LevelFactory;
	class Platform;

	class Play : public State {
	public:
		Play(Game& game, LevelFactory& level);

		std::unique_ptr<State> update() override;
		void draw() override;

	private:
		std::unique_ptr<Level> level;

		Game& game;
		Platform& platform;
		LevelFactory& factory;

		int score{};
	};
}

#endif //SUPER_HAXAGON_PLAY_HPP
