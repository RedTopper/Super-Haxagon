#ifndef SUPER_HAXAGON_LOAD_HPP
#define SUPER_HAXAGON_LOAD_HPP

#include "State.hpp"
#include "Factories/Level.hpp"

namespace SuperHaxagon {
	class Game;
	class Platform;

	class Load : public State {
	public:
		const char* UNLOADED = "ERROR";
		const char* PROJECT_HEADER = "HAX1.1";
		const char* PROJECT_FOOTER = "ENDHAX";
		const char* LEVEL_HEADER = "LEV2.1";
		const char* LEVEL_FOOTER = "ENDLEV";
		const char* PATTERN_HEADER = "PTN1.1";
		const char* PATTERN_FOOTER = "ENDPTN";

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
