#ifndef SUPER_HAXAGON_TITLE_HPP
#define SUPER_HAXAGON_TITLE_HPP

#include "State.hpp"

#include "Core/Structs.hpp"

#include <map>
#include <vector>

namespace SuperHaxagon {
	class Game;
	class Platform;
	class LevelFactory;

	class Title : public State {
	public:
		explicit Title(Game& game);
		Title(Title&) = delete;
		~Title() override;

		std::unique_ptr<State> update(float dilation) override;
		void drawGame(SurfaceGame& surface, SurfaceGame* shadows) override;
		void drawTopUI(SurfaceUI& surface) override;
		void drawBotUI(SurfaceUI& surface) override;
		void enter() override;
		void exit() override {}

	private:
		Game& _game;
		Platform& _platform;

		float _frameBackgroundColor = 0.0;
		float _rotation = 0.0f;
		LevelFactory& _firstLevel;

		GameColors _colorCurrent;

		std::map<LocColor, Color> _color;
		std::map<LocColor, Color> _colorNext;
		std::map<LocColor, size_t> _colorNextIndex;
	};
}

#endif //SUPER_HAXAGON_TITLE_HPP
