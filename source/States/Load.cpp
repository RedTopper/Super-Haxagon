#include <memory>
#include <fstream>

#include "Factories/Level.hpp"
#include "Factories/Pattern.hpp"
#include "States/Quit.hpp"
#include "States/Menu.hpp"
#include "States/Load.hpp"

#include "Game.hpp"

namespace SuperHaxagon {
	Load::Load(Game& game) : game(game), platform(game.getPlatform()) {}

	std::runtime_error error(const std::string& path, const std::string& message) {
		return std::runtime_error("[file] '" + path + "': " + message);
	}

	void Load::load(std::ifstream& file, const std::string& path, Location location) {
		std::vector<std::shared_ptr<PatternFactory>> patterns;

		//header
		if(!readCompare(file, PROJECT_HEADER))
			throw error(path, "File header invalid!");

		int numPatterns = readSize(file);
		if (numPatterns > 300 || numPatterns < 1)
			throw error(path, "Amount of patterns is less than 1 or greater than 300!");

		patterns.reserve(numPatterns);
		for (int i = 0; i < numPatterns; i++) {
			patterns.emplace_back(std::make_shared<PatternFactory>(file));
		}

		int numLevels = readSize(file);
		if (numLevels > 300 || numLevels < 1)
			throw error(path, "Amount of levels is less than 1 or greater than 300!");

		for (int i = 0; i < numPatterns; i++) {
			game.addLevel(std::make_unique<LevelFactory>(file, patterns, location));
		}

		if(!readCompare(file, PROJECT_FOOTER))
			throw error(path, "File footer invalid!");
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
			load(file, path, loc);
		}

		if (game.getLevels().empty()) {
			throw error("Unknown", "No levels loaded!");
		}
	}

	std::unique_ptr<State> Load::update() {
		return std::make_unique<Menu>(game);
	}
}