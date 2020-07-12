#include <memory>
#include <fstream>
#include <climits>

#include "Core/Game.hpp"
#include "Factories/Level.hpp"
#include "Factories/Pattern.hpp"
#include "States/Menu.hpp"
#include "States/Load.hpp"
#include "States/Quit.hpp"

namespace SuperHaxagon {
	const char* Load::PROJECT_HEADER = "HAX1.1";
	const char* Load::PROJECT_FOOTER = "ENDHAX";
	const char* Load::SCORE_HEADER = "SCDB1.0";
	const char* Load::SCORE_FOOTER = "ENDSCDB";

	Load::Load(Game& game) : _game(game), _platform(game.getPlatform()) {}
	Load::~Load() = default;

	bool Load::loadFile(std::ifstream& file, Location location) const {
		std::vector<std::shared_ptr<PatternFactory>> patterns;

		if(!readCompare(file, PROJECT_HEADER)) {
			_platform.message(Dbg::WARN, "file", "file header invalid!");
			return false;
		}

		const int numPatterns = read32(file, 1, 300, _platform, "number of patterns");
		patterns.reserve(numPatterns);
		for (auto i = 0; i < numPatterns; i++) {
			auto pattern = std::make_shared<PatternFactory>(file, _platform);
			if (!pattern->isLoaded()) {
				_platform.message(Dbg::WARN, "file", "a pattern failed to load");
				return false;
			}

			patterns.emplace_back(std::move(pattern));
		}

		if (patterns.empty()) {
			_platform.message(Dbg::WARN, "file", "no patterns loaded");
			return false;
		}

		const int numLevels = read32(file, 1, 300, _platform, "number of levels");
		for (auto i = 0; i < numLevels; i++) {
			auto level = std::make_unique<LevelFactory>(file, patterns, location, _platform);
			if (!level->isLoaded()) {
				_platform.message(Dbg::WARN, "file", "a level failed to load");
				return false;
			}

			_game.addLevel(std::move(level));
		}

		if(!readCompare(file, PROJECT_FOOTER)) {
			_platform.message(Dbg::WARN, "load", "file footer invalid");
			return false;
		}

		return true;
	}

	bool Load::loadScores(std::ifstream& file) const {
		if (!file) {
			_platform.message(Dbg::INFO, "scores", "no score database");
			return true;
		}

		if (!readCompare(file, SCORE_HEADER)) {
			_platform.message(Dbg::WARN,"scores", "score header invalid, skipping scores");
			return true; // If there is no score database silently fail.
		}

		const int numScores = read32(file, 1, 300, _platform, "number of scores");
		for (auto i = 0; i < numScores; i++) {
			auto name = readString(file, _platform, "score level name");
			auto difficulty = readString(file, _platform, "score level difficulty");
			auto mode = readString(file, _platform, "score level mode");
			auto creator = readString(file, _platform, "score level creator");
			const int score = read32(file, 0, INT_MAX, _platform, "score");
			for (const auto& level : _game.getLevels()) {
				if (level->getName() == name && level->getDifficulty() == difficulty && level->getMode() == mode && level->getCreator() == creator) {
					level->setHighScore(score);
				}
			}
		}

		if (!readCompare(file, SCORE_FOOTER)) {
			_platform.message(Dbg::WARN,"scores", "file footer invalid, db broken");
			return false;
		}

		return true;
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
			if (!loadFile(file, loc)) continue;
		}

		if (_game.getLevels().empty()) {
			_platform.message(Dbg::FATAL, "scores", "no levels loaded");
			return;
		}

		std::ifstream scores(_platform.getPath("/scores.db"), std::ios::in | std::ios::binary);
		if (!loadScores(scores)) return;

		_loaded = true;
	}

	std::unique_ptr<State> Load::update(double) {
		if (_loaded) {
			return std::make_unique<Menu>(_game, 0);
		} else {
			return std::make_unique<Quit>(_game);
		}
	}
}
