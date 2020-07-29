#ifndef SUPER_HAXAGON_MENU_HPP
#define SUPER_HAXAGON_MENU_HPP

#include "State.hpp"

#include "Core/Structs.hpp"

#include <map>
#include <vector>

namespace SuperHaxagon {
	class Game;
	class Platform;
	class LevelFactory;

	class Menu : public State {
	public:
		const int FRAMES_PER_TRANSITION = 12;
		const int FRAMES_PER_COLOR = 60;

		Menu(Game& game, LevelFactory& selected);
		Menu(Menu&) = delete;
		~Menu() override;

		std::unique_ptr<State> update(double dilation) override;
		void drawTop(double scale) override;
		void drawBot(double scale) override;
		void enter() override;
		void exit() override {};

	private:
		Game& _game;
		Platform& _platform;

		double _frameRotation = FRAMES_PER_TRANSITION;
		double _frameBackgroundColor = FRAMES_PER_COLOR;
		int _transitionDirection = 0;

		std::vector<std::unique_ptr<LevelFactory>>::const_iterator _selected;
		std::map<LocColor, Color> _color;
		std::map<LocColor, Color> _colorNext;
		std::map<LocColor, size_t> _colorNextIndex;
	};
}

#endif //SUPER_HAXAGON_MENU_HPP
