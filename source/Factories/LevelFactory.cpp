// SPDX-FileCopyrightText: 2021 AJ Walter
// SPDX-License-Identifier: GPL-3.0-or-later

#include "Factories/LevelFactory.hpp"

#include "Core/Game.hpp"
#include "Core/Twist.hpp"
#include "Driver/Platform.hpp"
#include "Factories/PatternFactory.hpp"
#include "Objects/Level.hpp"

namespace SuperHaxagon {
	const char* LevelFactory::LEVEL_HEADER = "LEV3.0";
	const char* LevelFactory::LEVEL_FOOTER = "ENDLEV";

	LevelFactory::LevelFactory(std::istream& stream, std::vector<std::shared_ptr<PatternFactory>>& shared, const Location location, Platform& platform, const size_t levelIndexOffset) {
		_location = location;

		if (!readCompare(stream, LEVEL_HEADER)) {
			platform.message(Dbg::WARN, "level", "level header invalid!");
			return;
		}

		_name = readString(stream, platform, "level name");
		_difficulty = readString(stream, platform, _name + " level difficulty");
		_mode = readString(stream, platform, _name + " level mode");
		_creator = readString(stream, platform, _name + " level creator");
		_music = "/" + readString(stream, platform, _name + " level music");

		const auto numColorsBG1 = read32(stream, 1, 512, platform, "level background 1");
		_colors[LocColor::BG1].reserve(numColorsBG1);
		for (auto i = 0; i < numColorsBG1; i++) _colors[LocColor::BG1].emplace_back(readColor(stream));

		const auto numColorsBG2 = read32(stream, 1, 512, platform, "level background 2");
		_colors[LocColor::BG2].reserve(numColorsBG2);
		for (auto i = 0; i < numColorsBG2; i++) _colors[LocColor::BG2].emplace_back(readColor(stream));

		const auto numColorsFG = read32(stream, 1, 512, platform, "level foreground");
		_colors[LocColor::FG].reserve(numColorsFG);
		for (auto i = 0; i < numColorsFG; i++) _colors[LocColor::FG].emplace_back(readColor(stream));

		_speedWall = readFloat(stream) * SCALE_TO_SCREEN_SPACE;
		_speedRotation = readFloat(stream);
		_speedCursor = readFloat(stream);
		_speedPulse = read32(stream, 4, 8192, platform, "level pulse");
		_nextIndex = read32(stream, -1, 8192, platform, "next index");
		_nextTime = readFloat(stream);

		// Negative numbers should remain invalid. -1 usually means load no other level.
		if (_nextIndex >= 0) _nextIndex += static_cast<int>(levelIndexOffset);

		const auto numPatterns = read32(stream, 1, 512, platform, "level pattern count");
		for (auto i = 0; i < numPatterns; i++) {
			auto found = false;
			auto search = readString(stream, platform, "level pattern name match");
			for (const auto& pattern : shared) {
				if (pattern->getName() == search) {
					_patterns.push_back(pattern);
					found = true;
					break;
				}
			}

			if (!found) {
				platform.message(Dbg::WARN, "level", "could not find pattern " + search + " for " + _name);
				return;
			}
		}

		if (!readCompare(stream, LEVEL_FOOTER)) {
			platform.message(Dbg::WARN, "level", "level footer invalid!");
			return;
		}

		_loaded = true;
	}

	std::unique_ptr<Level> LevelFactory::instantiate(Twist& rng, float renderDistance, float rotation, float cursorPos) const {
		return std::make_unique<Level>(*this, rng, renderDistance, rotation, cursorPos);
	}
	bool LevelFactory::setHighScore(const int score) {
		if(score > _highScore) {
			_highScore = score;
			return true;
		}

		return false;
	}
}
