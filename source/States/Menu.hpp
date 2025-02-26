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
		const float FRAMES_PER_TRANSITION = 12.0f;
		const float FRAMES_PER_COLOR = 60.0f;

		Menu(Game& game, LevelFactory& selected, const GameColors& starting, int rotationDirection, float rotation);
		Menu(Menu&) = delete;
		~Menu() override;

		std::unique_ptr<State> update(float dilation) override;
		void drawGame(SurfaceGame& surface, SurfaceGame* shadows) override;
		void drawTopUI(SurfaceUI& surface) override;
		void drawBotUI(SurfaceUI& surface) override;
		void enter() override;
		void exit() override {}

	private:
		Game& _game;
		Platform& _platform;

		float _frameRotation = FRAMES_PER_TRANSITION;
		float _frameBackgroundColor = FRAMES_PER_COLOR;
		int _transitionDirection = 0;
		bool _justEntered = true;

		std::vector<std::unique_ptr<LevelFactory>>::const_iterator _selected;
		float _cameraRotation = 0.0f;
		std::map<LocColor, Color> _color;
		std::map<LocColor, Color> _colorNext;
		std::map<LocColor, size_t> _colorNextIndex;
	};
}

#endif //SUPER_HAXAGON_MENU_HPP
