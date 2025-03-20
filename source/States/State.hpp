#ifndef SUPER_HAXAGON_STATE_HPP
#define SUPER_HAXAGON_STATE_HPP

// SPDX-FileCopyrightText: 2020 AJ Walter
// SPDX-License-Identifier: GPL-3.0

#include <memory>

namespace SuperHaxagon {
	class SurfaceGame;
	class SurfaceUI;

	class State {
	public:
		virtual ~State() = default;

		virtual std::unique_ptr<State> update(float dilation) = 0;
		virtual void drawGame(SurfaceGame& surface, SurfaceGame* shadows) = 0;
		virtual void drawTopUI(SurfaceUI& surface) = 0;
		virtual void drawBotUI(SurfaceUI& surface) = 0;
		virtual void enter() {};
		virtual void exit() {};
	};
}

#endif //SUPER_HAXAGON_STATE_HPP
