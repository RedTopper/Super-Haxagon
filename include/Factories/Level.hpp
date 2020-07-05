#ifndef SUPER_HAXAGON_LEVEL_HPP
#define SUPER_HAXAGON_LEVEL_HPP

#include <memory>
#include <string>
#include <vector>
#include <deque>

#include "Structs.hpp"

namespace SuperHaxagon {
	enum class Location {
		INTERNAL,
		EXTERNAL
	};

	class Game;
	class LevelFactory;
	class Pattern;
	class PatternFactory;
	class Twist;

	class Level {
	public:
		static constexpr double DIFFICULTY_MULTIPLIER = 1.1;
		static constexpr int FLIP_FRAMES_MIN = 120;
		static constexpr int FLIP_FRAMES_MAX = 500;
		static constexpr int FRAMES_PER_CHANGE_SIDE = 36;

		Level(const LevelFactory& factory, Twist& rng, int renderDistance);
		Level(Level&) = delete;
		~Level();

		void update(Twist& rng, int hexLength, int renderDistance);
		void draw(Game& game, double offset);
		Movement collision(int cursorDistance) const;

		void increaseMultiplier();
		void rotate(double distance);
		void left();
		void right();
		void clamp();


	private:
		const LevelFactory& factory;

		std::deque<std::unique_ptr<Pattern>> patterns;

		double multiplier = 1.0; // Current direction and speed of rotation
		double cursorPos{};
		double rotation{};
		double sidesTween{};
		int lastSides{};
		int currentSides{};

		int delayFrame{}; // Tween between side switches
		int tweenFrame{}; // Tween colors
		int flipFrame = FLIP_FRAMES_MAX; // Amount of frames left until flip

		int indexBG1{};
		int indexBG2{};
		int indexFG{};
		int nextIndexBG1{};
		int nextIndexBG2{};
		int nextIndexFG{};
	};

	class LevelFactory {
	public:
		static const char* LEVEL_HEADER;
		static const char* LEVEL_FOOTER;

		LevelFactory(std::ifstream& file, std::vector<std::shared_ptr<PatternFactory>>& shared, Location location);
		LevelFactory(const LevelFactory&) = delete;

		std::unique_ptr<Level> instantiate(Twist& rng, int renderDistance) const;

		const std::vector<std::shared_ptr<PatternFactory>>& getPatterns() const {return patterns;}
		const std::vector<Color>& getColorsFG() const {return colorsFG;}
		const std::vector<Color>& getColorsBG1() const {return colorsBG1;}
		const std::vector<Color>& getColorsBG2() const {return colorsBG2;}

		const std::string& getName() const {return name;}
		const std::string& getDifficulty() const {return difficulty;}
		const std::string& getMode() const {return mode;}
		const std::string& getCreator() const {return creator;}
		const std::string& getMusic() const {return music;}

		Location getLocation() const {return location;}
		int getHighScore() const {return highScore;}
		int getSpeedPulse() const {return speedPulse;}
		float getSpeedCursor() const {return speedCursor;}
		float getSpeedRotation() const {return speedRotation;}
		float getSpeedWall() const {return speedWall;}

		bool setHighScore(int score);

	private:
		std::vector<std::shared_ptr<PatternFactory>> patterns;
		std::vector<Color> colorsFG;
		std::vector<Color> colorsBG1;
		std::vector<Color> colorsBG2;

		std::string name;
		std::string difficulty;
		std::string mode;
		std::string creator;
		std::string music;

		Location location = Location::INTERNAL;

		int highScore;
		int speedPulse;
		float speedWall;
		float speedRotation;
		float speedCursor;
	};
}

#endif //SUPER_HAXAGON_LEVEL_HPP
