#ifndef SUPER_HAXAGON_LEVEL_H
#define SUPER_HAXAGON_LEVEL_H

#include <vector>

#include "Game.h"

namespace SuperHaxagon {
	class Level {
	public:
		Level(const Level&) = delete;

		const std::vector<Color>& getColorsFG() const {return colorsFG;}
		const std::vector<Color>& getColorsBG1() const {return colorsBG1;}
		const std::vector<Color>& getColorsBG2() const {return colorsBG2;}

		const std::string& getName() const {return name;}
		const std::string& getDifficulty() const {return difficulty;}
		const std::string& getMode() const {return mode;}
		const std::string& getCreator() const {return creator;}
		const std::string& getMusic() const {return music;}
		int getScore() const {return score;}

	private:
		std::vector<Color> colorsFG;
		std::vector<Color> colorsBG1;
		std::vector<Color> colorsBG2;

		std::string name;
		std::string difficulty;
		std::string mode;
		std::string creator;
		std::string music;

		int score;
	};
}

#endif //SUPER_HAXAGON_LEVEL_H
