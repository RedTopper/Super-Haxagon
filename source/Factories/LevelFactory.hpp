#ifndef SUPER_HAXAGON_LEVEL_FACTORY_HPP
#define SUPER_HAXAGON_LEVEL_FACTORY_HPP

// SPDX-FileCopyrightText: 2021 AJ Walter
// SPDX-License-Identifier: GPL-3.0-or-later

#include "Core/Structs.hpp"
#include "Driver/Platform.hpp"

#include <memory>
#include <string>
#include <vector>
#include <map>

namespace SuperHaxagon {	
	class Game;
	class LevelFactory;
	class PatternFactory;
	class Twist;
	class Level;

	class LevelFactory {
	public:
		static const char* LEVEL_HEADER;
		static const char* LEVEL_FOOTER;

		LevelFactory(std::istream& stream, std::vector<std::shared_ptr<PatternFactory>>& shared, Location location, Platform& platform, size_t levelIndexOffset);
		LevelFactory(const LevelFactory&) = delete;

		std::unique_ptr<Level> instantiate(Twist& rng, float renderDistance, float rotation, float cursorPos) const;

		bool isLoaded() const {return _loaded;}

		const std::vector<std::shared_ptr<PatternFactory>>& getPatterns() const {return _patterns;}
		const std::map<LocColor, std::vector<Color>>& getColors() const {return _colors;}

		const std::string& getName() const {return _name;}
		const std::string& getDifficulty() const {return _difficulty;}
		const std::string& getMode() const {return _mode;}
		const std::string& getCreator() const {return _creator;}
		const std::string& getMusic() const {return _music;}
		bool isCreditsLevel() const {
			return _creator == "REDHAT" &&
			    _name == "CREDITS" &&
			    _difficulty == "SPOILERS" &&
			    _mode == "(DUH)";
		}

		Location getLocation() const {return _location;}
		int getHighScore() const {return _highScore;}
		int getSpeedPulse() const {return _speedPulse;}
		float getSpeedCursor() const {return _speedCursor;}
		float getSpeedRotation() const {return _speedRotation;}
		float getSpeedWall() const {return _speedWall;}
		int getNextIndex() const {return _nextIndex;}
		float getNextTime() const {return _nextTime;}

		bool setHighScore(int score);

	private:
		std::vector<std::shared_ptr<PatternFactory>> _patterns;
		std::map<LocColor, std::vector<Color>> _colors;

		std::string _name;
		std::string _difficulty;
		std::string _mode;
		std::string _creator;
		std::string _music;

		Location _location = Location::ROM;

		int _highScore = 0;
		int _speedPulse = 0;
		int _nextIndex = -1;
		float _speedWall = 0;
		float _speedRotation = 0;
		float _speedCursor = 0;
		float _nextTime = 0;
		bool _loaded = false;
	};
}

#endif //SUPER_HAXAGON_LEVEL_FACTORY_HPP
