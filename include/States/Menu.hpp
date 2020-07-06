#ifndef SUPER_HAXAGON_MENU_HPP
#define SUPER_HAXAGON_MENU_HPP

#include "State.hpp"
#include "Driver/Platform.hpp"

namespace SuperHaxagon {
	class Game;

	class Menu : public State {
	public:
		const int FRAMES_PER_TRANSITION = 12;

		explicit Menu(Game& game);
		Menu(Menu&) = delete;
		~Menu();

		std::unique_ptr<State> update(double dilation) override;
		void drawTop(double scale) override;
		void drawBot(double scale) override;
		void enter() override;
		void exit() override;

	private:
		Game& game;
		Platform& platform;

		double transitionFrame = 0;
		int transitionDirection = 0;
		int lastLevel = 0;
		int level = 0;
	};
}

#endif //SUPER_HAXAGON_MENU_HPP
