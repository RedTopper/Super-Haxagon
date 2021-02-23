#include "States/Load.hpp"

#include "Core/Game.hpp"
#include "Core/Platform.hpp"
#include "Factories/LevelFactory.hpp"
#include "Factories/PatternFactory.hpp"
#include "States/Menu.hpp"
#include "States/Quit.hpp"

#include <memory>
#include <fstream>
#include <climits>
#include <filesystem>

namespace SuperHaxagon {
	const char* Load::PROJECT_HEADER = "HAX1.1";
	const char* Load::PROJECT_FOOTER = "ENDHAX";
	const char* Load::SCORE_HEADER = "SCDB1.0";
	const char* Load::SCORE_FOOTER = "ENDSCDB";

	Load::Load(Game& game) : _game(game), _platform(game.getPlatform()) {}
	Load::~Load() = default;

	bool Load::loadLevels(std::istream& stream, Location location) const {
		std::vector<std::shared_ptr<PatternFactory>> patterns;

		// Used to make sure that external levels link correctly.
		const auto levelIndexOffset = _game.getLevels().size();

		if(!readCompare(stream, PROJECT_HEADER)) {
			_platform.message(Dbg::WARN, "file", "file header invalid!");
			return false;
		}

		const auto numPatterns = read32(stream, 1, 300, _platform, "number of patterns");
		patterns.reserve(numPatterns);
		for (auto i = 0; i < numPatterns; i++) {
			auto pattern = std::make_shared<PatternFactory>(stream, _platform);
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

		const auto numLevels = read32(stream, 1, 300, _platform, "number of levels");
		for (auto i = 0; i < numLevels; i++) {
			auto level = std::make_unique<LevelFactory>(stream, patterns, location, _platform, levelIndexOffset);
			if (!level->isLoaded()) {
				_platform.message(Dbg::WARN, "file", "a level failed to load");
				return false;
			}

			_game.addLevel(std::move(level));
		}

		if(!readCompare(stream, PROJECT_FOOTER)) {
			_platform.message(Dbg::WARN, "load", "file footer invalid");
			return false;
		}

		return true;
	}

	bool Load::loadScores(std::istream& stream) const {
		if (!stream) {
			_platform.message(Dbg::INFO, "scores", "no score database");
			return true;
		}

		if (!readCompare(stream, SCORE_HEADER)) {
			_platform.message(Dbg::WARN,"scores", "score header invalid, skipping scores");
			return true; // If there is no score database silently fail.
		}

		const auto numScores = read32(stream, 1, 300, _platform, "number of scores");
		for (auto i = 0; i < numScores; i++) {
			auto name = readString(stream, _platform, "score level name");
			auto difficulty = readString(stream, _platform, "score level difficulty");
			auto mode = readString(stream, _platform, "score level mode");
			auto creator = readString(stream, _platform, "score level creator");
			const auto score = read32(stream, 0, INT_MAX, _platform, "score");
			for (const auto& level : _game.getLevels()) {
				if (level->getName() == name && level->getDifficulty() == difficulty && level->getMode() == mode && level->getCreator() == creator) {
					level->setHighScore(score);
				}
			}
		}

		if (!readCompare(stream, SCORE_FOOTER)) {
			_platform.message(Dbg::WARN,"scores", "file footer invalid, db broken");
			return false;
		}

		return true;
	}

	void Load::enter() {
		std::vector<std::pair<Location, std::string>> levels;
		levels.emplace_back(std::pair<Location, std::string>(Location::ROM, "/levels.haxagon"));

		if (static_cast<int>(_platform.supports() & Supports::FILESYSTEM)) {
			auto files = std::filesystem::directory_iterator(_platform.getPath("/", Location::USER));
			for (const auto& file : files) {
				if (file.path().extension() != ".haxagon") continue;
				_platform.message(Dbg::INFO, "load", "found " + file.path().string());
				levels.emplace_back(std::pair<Location, std::string>(Location::USER, file.path().string()));
			}
		}

		for (const auto& pair : levels) {
			const auto& path = pair.second;
			const auto location = pair.first;
			auto file = _platform.openFile(path, location);
			if (!file) continue;
			loadLevels(*file, location);
		}

		if (_game.getLevels().empty()) {
			_platform.message(Dbg::FATAL, "levels", "no levels loaded");
			return;
		}

		std::ifstream scores(_platform.getPath("/scores.db", Location::USER), std::ios::in | std::ios::binary);
		if (!loadScores(scores)) return;

		_loaded = true;
	}

	std::unique_ptr<State> Load::update(float) {
		if (_loaded) return std::make_unique<Menu>(_game, *_game.getLevels()[0]);
		return std::make_unique<Quit>(_game);
	}
}
