#ifndef SUPER_HAXAGON_WIN_HPP
#define SUPER_HAXAGON_WIN_HPP

#include "State.hpp"

#include <string>
#include <vector>

namespace SuperHaxagon {
	class Game;
	class Level;
	class LevelFactory;
	class Platform;
	class Pattern;

	struct Credits {
		std::string name;
		std::string line1;
		std::string line2;
	};

	class Win : public State {
	public:
		Win(Game& game, std::unique_ptr<Level> level, LevelFactory& selected, float score, std::string text);
		Win(Win&) = delete;
		~Win() override = default;

		std::unique_ptr<State> update(float dilation) override;
		void enter() override;
		void drawTop(float scale) override;
		void drawBot(float scale) override;

	private:
		Game& _game;
		Platform& _platform;
		LevelFactory& _selected;

		std::unique_ptr<Level> _level;
		std::unique_ptr<Pattern> _surround;
		std::vector<Credits> _credits;
		
		float _score = 0.0;

		size_t _index = 0;
		float _timer = 0.0;

		std::string _text = "GAME OVER";
	};
}

#endif //SUPER_HAXAGON_WIN_HPP
