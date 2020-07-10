#include <memory>
#include <fstream>
#include <climits>

#include "Core/Game.hpp"
#include "Factories/Level.hpp"
#include "Factories/Pattern.hpp"
#include "States/Menu.hpp"
#include "States/Load.hpp"

namespace SuperHaxagon {
	const char* Load::PROJECT_HEADER = "HAX1.1";
	const char* Load::PROJECT_FOOTER = "ENDHAX";
	const char* Load::SCORE_HEADER = "SCDB1.0";
	const char* Load::SCORE_FOOTER = "ENDSCDB";

	Load::Load(Game& game) : _game(game), _platform(game.getPlatform()) {}
	Load::~Load() = default;

	void Load::loadLevel(std::ifstream& file, Location location) const {
		std::vector<std::shared_ptr<PatternFactory>> patterns;

		if(!readCompare(file, PROJECT_HEADER))
			throw malformed("load", "file header invalid!");

		const int numPatterns = read32(file, 1, 300, "number of patterns");
		patterns.reserve(numPatterns);
		for (auto i = 0; i < numPatterns; i++) {
			patterns.emplace_back(std::make_shared<PatternFactory>(file));
		}

		const int numLevels = read32(file, 1, 300, "number of levels");
		for (auto i = 0; i < numLevels; i++) {
			_game.addLevel(std::make_unique<LevelFactory>(file, patterns, location));
		}

		if(!readCompare(file, PROJECT_FOOTER))
			throw malformed("load", "file footer invalid!");
	}

	void Load::loadScores(std::ifstream& file) const {
		if (!readCompare(file, SCORE_HEADER))
			return; // If there is no score database silently fail.

		const int numScores = read32(file, 1, 300, "number of scores");
		for (auto i = 0; i < numScores; i++) {
			auto name = readString(file, "score level name");
			auto difficulty = readString(file, "score level difficulty");
			auto mode = readString(file, "score level mode");
			auto creator = readString(file, "score level creator");
			const int score = read32(file, 0, INT_MAX, "score");
			for (const auto& level : _game.getLevels()) {
				if (level->getName() == name && level->getDifficulty() == difficulty && level->getMode() == mode && level->getCreator() == creator) {
					level->setHighScore(score);
				}
			}
		}

		if (!readCompare(file, SCORE_FOOTER))
			throw malformed("score", "file footer invalid, db broken");
	}

	void Load::enter() {
		// Maybe use a glob or something someday
		std::vector<std::pair<Location, std::string>> locations;
		locations.emplace_back(std::pair<Location, std::string>(Location::INTERNAL, _platform.getPathRom("/levels.haxagon")));
		locations.emplace_back(std::pair<Location, std::string>(Location::EXTERNAL, _platform.getPath("/levels.haxagon")));

		for (const auto& location : locations) {
			const auto& path = location.second;
			const auto loc = location.first;
			std::ifstream file(path, std::ios::in | std::ios::binary);
			if (!file) continue;
			loadLevel(file, loc);
		}

		if (_game.getLevels().empty()) {
			throw malformed("enter", "no levels loaded!");
		}

		std::ifstream scores(_platform.getPath("/scores.db"), std::ios::in | std::ios::binary);
		loadScores(scores);
	}

	std::unique_ptr<State> Load::update(double) {
		return std::make_unique<Menu>(_game, 0);
	}
}
