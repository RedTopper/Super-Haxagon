#ifndef SUPER_HAXAGON_TRANSITION_HPP
#define SUPER_HAXAGON_TRANSITION_HPP

// SPDX-FileCopyrightText: 2020 AJ Walter
// SPDX-License-Identifier: GPL-3.0

#include "State.hpp"

#include "Core/Structs.hpp"

namespace SuperHaxagon {
	class Level;
	class LevelFactory;
	class Game;
	class Platform;
	class SurfaceGame;

	class Transition : public State {
	public:
		static constexpr float TRANSITION_ACCELERATION_RATE = 0.1f;
		static constexpr int TRANSITION_FRAMES = 120;
		
		Transition(Game& game, std::unique_ptr<Level> level, LevelFactory& selected, float score);
		Transition(Transition&) = delete;
		~Transition() override;

		std::unique_ptr<State> update(float dilation) override;
		void drawGame(SurfaceGame& surface, SurfaceGame* shadows)  override;
		void drawTopUI(SurfaceUI&) override {};
		void drawBotUI(SurfaceUI& ui) override;
		void enter() override;

	private:
		Game& _game;
		Platform& _platform;
		LevelFactory& _selected;
		std::unique_ptr<Level> _level;

		float _score = 0;
		float _frames = 0;
		float _offset = 1.0;
	};
}

#endif //SUPER_HAXAGON_TRANSITION_HPP
