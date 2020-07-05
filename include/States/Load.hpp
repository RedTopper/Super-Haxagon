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
		static const char* LEVEL_HEADER;
		static const char* LEVEL_FOOTER;

		explicit Load(Game& game);
		Load(Load&) = delete;

		void load(std::ifstream& file, const std::string& path, Location location);

		std::unique_ptr<State> update() override;
		void enter() override;
		void draw() override {};

	private:
		Game& game;
		Platform& platform;
	};
}

#endif //SUPER_HAXAGON_LOAD_HPP
