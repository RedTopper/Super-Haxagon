// SPDX-FileCopyrightText: 2020 AJ Walter
// SPDX-License-Identifier: GPL-3.0

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
		void drawGame(SurfaceGame&, SurfaceGame*) override {}
		void drawTopUI(SurfaceUI&) override {}
		void drawBotUI(SurfaceUI&) override {}

	private:
		Game& _game;
	};
}

#endif //SUPER_HAXAGON_QUIT_HPP
