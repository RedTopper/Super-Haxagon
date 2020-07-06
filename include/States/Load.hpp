#ifndef SUPER_HAXAGON_LOAD_HPP
#define SUPER_HAXAGON_LOAD_HPP

#include "State.hpp"
#include "Factories/Level.hpp"

namespace SuperHaxagon {
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

		void loadLevel(std::ifstream& file, Location location);
		void loadScores(std::ifstream& file);

		std::unique_ptr<State> update(double dilation) override;
		void enter() override;
		void drawTop(double) override {};
		void drawBot(double) override {};

	private:
		Game& game;
		Platform& platform;
	};
}

#endif //SUPER_HAXAGON_LOAD_HPP
