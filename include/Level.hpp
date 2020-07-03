#ifndef SUPER_HAXAGON_LEVEL_HPP
#define SUPER_HAXAGON_LEVEL_HPP

#include <string>
#include <vector>

#include "Structs.hpp"
#include "Pattern.hpp"

namespace SuperHaxagon {
	class Level {
	public:
		Level(const Level&) = delete;

		const std::vector<Pattern>& getPatterns() const {return patterns;}
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
		std::vector<Pattern> patterns;
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
