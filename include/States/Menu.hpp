#ifndef SUPER_HAXAGON_MENU_HPP
#define SUPER_HAXAGON_MENU_HPP

#include "State.hpp"
#include "Driver/Platform.hpp"

namespace SuperHaxagon {
	class Game;

	class Menu : public State {
	public:
		const int FRAMES_PER_TRANSITION = 12;
		const int FRAMES_PER_COLOR = 60;

		explicit Menu(Game& game, int levelIndex);
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

		double _transitionFrame = FRAMES_PER_TRANSITION;
		double _colorFrame = FRAMES_PER_COLOR;
		int _transitionDirection = 0;
		int _level = 0;

		Color _fg{};
		Color _bg1{};
		Color _bg2{};

		size_t _fgIndex = 0;
		size_t _bg1Index = 0;
		size_t _bg2Index = 0;

		size_t _fgIndexLast = 0;
		size_t _bg1IndexLast = 0;
		size_t _bg2IndexLast = 0;
	};
}

#endif //SUPER_HAXAGON_MENU_HPP
