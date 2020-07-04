#ifndef SUPER_HAXAGON_LEVEL_HPP
#define SUPER_HAXAGON_LEVEL_HPP

#include <string>
#include <vector>
#include <deque>

#include "Structs.hpp"
#include "Pattern.hpp"

namespace SuperHaxagon {
	class Game;
	class Level {
	public:
		static constexpr int TOTAL_PATTERNS_AT_ONE_TIME = 5;
		static constexpr int FLIP_FRAMES_MIN = 120;
		static constexpr int FLIP_FRAMES_MAX = 500;

		Level();

	private:
		std::deque<Pattern> patterns;

		double multiplier = 1.0; // Current direction and speed of rotation
		double cursorPos{};
		double rotation{};
		double sidesTween{};
		int lastSides;
		int currentSides;

		int delayFrame{}; //tweening between side switches
		int tweenFrame{}; //tweening colors
		int flipFrame = FLIP_FRAMES_MAX; //amount of frames left until flip

		int score{};
		int indexBG1{};
		int indexBG2{};
		int indexFG{};
		int nextIndexBG1;
		int nextIndexBG2;
		int nextIndexFG;
	};

	class LevelFactory {
	public:
		LevelFactory(const LevelFactory&) = delete;

		const std::vector<PatternFactory>& getPatterns() const {return patterns;}
		const std::vector<Color>& getColorsFG() const {return colorsFG;}
		const std::vector<Color>& getColorsBG1() const {return colorsBG1;}
		const std::vector<Color>& getColorsBG2() const {return colorsBG2;}

		const std::string& getName() const {return name;}
		const std::string& getDifficulty() const {return difficulty;}
		const std::string& getMode() const {return mode;}
		const std::string& getCreator() const {return creator;}
		const std::string& getMusic() const {return music;}

		int getHighScore() const {return highScore;}
		int getSpeedPulse() const {return speedPulse;}
		float getSpeedCursor() const {return speedCursor;}
		float getSpeedRotation() const {return speedRotation;}
		float getSpeedWall() const {return speedWall;}

	private:
		std::vector<PatternFactory> patterns;
		std::vector<Color> colorsFG;
		std::vector<Color> colorsBG1;
		std::vector<Color> colorsBG2;

		std::string name;
		std::string difficulty;
		std::string mode;
		std::string creator;
		std::string music;

		int highScore;
		int speedPulse;
		float speedWall;
		float speedRotation;
		float speedCursor;
	};
}

#endif //SUPER_HAXAGON_LEVEL_HPP
