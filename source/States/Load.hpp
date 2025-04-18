#ifndef SUPER_HAXAGON_LOAD_HPP
#define SUPER_HAXAGON_LOAD_HPP

// SPDX-FileCopyrightText: 2020 AJ Walter
// SPDX-License-Identifier: GPL-3.0

#include "State.hpp"

#include "Core/Structs.hpp"

namespace SuperHaxagon {
	enum class Location;
	class Game;
	class Platform;

	class Load : public State {
	public:
		static const char* PROJECT_HEADER;
		static const char* PROJECT_FOOTER;
		static const char* SCORE_HEADER;
		static const char* SCORE_FOOTER;

		explicit Load(Game& game);
		Load(Load&) = delete;
		~Load() override;

		bool loadLevels(std::istream& stream, Location location) const;
		bool loadScores(std::istream& stream) const;

		std::unique_ptr<State> update(float dilation) override;
		void enter() override;
		void drawGame(SurfaceGame&, SurfaceGame*) override {}
		void drawTopUI(SurfaceUI&) override {}
		void drawBotUI(SurfaceUI&) override {}

	private:
		Game& _game;
		Platform& _platform;
		bool _loaded = false;
	};
}

#endif //SUPER_HAXAGON_LOAD_HPP
