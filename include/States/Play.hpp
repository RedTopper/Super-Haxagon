#ifndef SUPER_HAXAGON_PLAY_HPP
#define SUPER_HAXAGON_PLAY_HPP

#include <deque>

#include "Factories/Pattern.hpp"

#include "State.hpp"
#include "Structs.hpp"


namespace SuperHaxagon {
	class Game;
	class Level;
	class LevelFactory;
	class Platform;

	class Play : public State {
	public:
		Play(Game& game, const LevelFactory& level);

		std::unique_ptr<State> update() override;
		void draw() override;

	private:
		const LevelFactory& factory;
		std::unique_ptr<Level> level;

		Game& game;
		Platform& platform;

		int score{};
	};
}

#endif //SUPER_HAXAGON_PLAY_HPP
