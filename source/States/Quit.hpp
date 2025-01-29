#ifndef SUPER_HAXAGON_QUIT_HPP
#define SUPER_HAXAGON_QUIT_HPP

#include "State.hpp"

namespace SuperHaxagon {
	class Game;

	class Quit : public State {
	public:
		explicit Quit(Game& game);
		Quit(Quit&) = delete;
		~Quit() override = default;

		std::unique_ptr<State> update(float) override;
		void drawTop(float) override {}
		void drawBot(float) override {}

	private:
		Game& _game;
	};
}

#endif //SUPER_HAXAGON_QUIT_HPP
