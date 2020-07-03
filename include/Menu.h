#ifndef SUPER_HAXAGON_MENU_H
#define SUPER_HAXAGON_MENU_H

#include "Driver/Platform.h"
#include "State.h"

namespace SuperHaxagon {
	class Menu : public State {
	public:
		const int FRAMES_PER_TRANSITION = 12;
		Menu(Platform& driver, Game& game, Audio& bgmMenu, Audio& hexagon, Audio& select, bool showLoadLevels);
		std::unique_ptr<State> update() override;
		void draw() override;
		void enter() override;
		void exit() override;

	private:
		Platform& platform;
		Game& game;
		Audio& bgmMenu;
		Audio& sfxHexagon;
		Audio& sfxSelect;

		int showLoadLevels;
		int transitionFrame = 0;
		int transitionDirection = 0;
		int lastLevel = 0;
		int level = 0;
	};
}

#endif //SUPER_HAXAGON_MENU_H
