#ifndef SUPER_HAXAGON_WIN_HPP
#define SUPER_HAXAGON_WIN_HPP

#include "State.hpp"

namespace SuperHaxagon {
	class Game;
	class Level;

	struct Credits {
		std::string name;
		std::string line1;
		std::string line2;
	};

	class Win : public State {
	public:
		Win(Game& game, std::unique_ptr<Level> level, LevelFactory& selected, double score, std::string text);
		Win(Win&) = delete;
		~Win() override = default;

		std::unique_ptr<State> update(double dilation) override;
		void enter() override;
		void drawTop(double scale) override;
		void drawBot(double scale) override;

	private:
		Game& _game;
		Platform& _platform;
		LevelFactory& _selected;

		std::unique_ptr<Level> _level;
		std::unique_ptr<Pattern> _surround;
		std::vector<Credits> _credits;
		
		double _score = 0.0;
		double _lastTime = 0.0;
		
		size_t _index = 0;
		double _timer = 0.0;

		std::string _text = "GAME OVER";
	};
}

#endif //SUPER_HAXAGON_WIN_HPP
