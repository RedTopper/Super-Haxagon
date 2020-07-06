#include <memory>
#include <fstream>

#include "Core/Game.hpp"
#include "Factories/Level.hpp"
#include "Factories/Pattern.hpp"
#include "States/Quit.hpp"
#include "States/Menu.hpp"
#include "States/Load.hpp"

namespace SuperHaxagon {
	const char* Load::PROJECT_HEADER = "HAX1.1";
	const char* Load::PROJECT_FOOTER = "ENDHAX";

	Load::Load(Game& game) : game(game), platform(game.getPlatform()) {}

	void Load::load(std::ifstream& file, Location location) {
		std::vector<std::shared_ptr<PatternFactory>> patterns;

		//header
		if(!readCompare(file, PROJECT_HEADER))
			throw malformed("load", "file header invalid!");

		int numPatterns = read32(file, 1, 300, "number of patterns");
		patterns.reserve(numPatterns);
		for (int i = 0; i < numPatterns; i++) {
			patterns.emplace_back(std::make_shared<PatternFactory>(file));
		}

		int numLevels = read32(file, 1, 300, "number of levels");
		for (int i = 0; i < numLevels; i++) {
			game.addLevel(std::make_unique<LevelFactory>(file, patterns, location));
		}

		if(!readCompare(file, PROJECT_FOOTER))
			throw malformed("load", "file footer invalid!");
	}

	void Load::enter() {

		// Maybe use a glob or something someday
		std::vector<std::pair<Location, std::string>> locations;
		locations.emplace_back(std::pair<Location, std::string>(Location::INTERNAL, platform.getPathRom("/levels.haxagon")));
		locations.emplace_back(std::pair<Location, std::string>(Location::EXTERNAL, platform.getPath("/levels.haxagon")));

		for (const auto& location : locations) {
			const auto& path = location.second;
			Location loc = location.first;
			std::ifstream file(path, std::ios::in | std::ios::binary);
			if (!file) continue;
			load(file, loc);
		}

		if (game.getLevels().empty()) {
			throw malformed("Unknown", "No levels loaded!");
		}
	}

	std::unique_ptr<State> Load::update(double dilation) {
		return std::make_unique<Menu>(game);
	}
}
