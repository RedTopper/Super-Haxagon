#ifndef SUPER_HAXAGON_LEVEL_HPP
#define SUPER_HAXAGON_LEVEL_HPP

#include <memory>
#include <string>
#include <vector>
#include <deque>

#include "Core/Structs.hpp"

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
		static constexpr double DIFFICULTY_MULTIPLIER_WALLS = 1.05;
		static constexpr double DIFFICULTY_MULTIPLIER_ROT = 1.1;
		static constexpr double FLIP_FRAMES_MIN = 120;
		static constexpr double FLIP_FRAMES_MAX = 600;
		static constexpr double FRAMES_PER_CHANGE_SIDE = 36;

		Level(const LevelFactory& factory, Twist& rng, double patternDistCreate);
		Level(Level&) = delete;
		~Level();

		void update(Twist& rng, double patternDistDelete, double patternDistCreate, double dilation);
		void draw(Game& game, const double scale, const double offset, const double beat) const;
		Movement collision(double cursorDistance, double dilation) const;

		void increaseMultiplier();
		void clearPatterns();
		void rotate(double distance, double dilation);
		void left(double dilation);
		void right(double dilation);
		void clamp();


	private:
		const LevelFactory& _factory;

		std::deque<std::unique_ptr<Pattern>> _patterns;

		double _multiplierRot = 1.0; // Current direction and speed of rotation
		double _multiplierWalls = 1.0; // Current speed of the walls flying at you
		double _cursorPos{};
		double _rotation{};
		double _sidesTween{};
		int _lastSides{};
		int _currentSides{};

		double _delayFrame{}; // Tween between side switches
		double _tweenFrame{}; // Tween colors
		double _flipFrame = FLIP_FRAMES_MAX; // Amount of frames left until flip

		int _indexBg1{};
		int _indexBg2{};
		int _indexFg{};
		int _nextIndexBg1{};
		int _nextIndexBg2{};
		int _nextIndexFg{};
	};

	class LevelFactory {
	public:
		static const char* LEVEL_HEADER;
		static const char* LEVEL_FOOTER;

		LevelFactory(std::ifstream& file, std::vector<std::shared_ptr<PatternFactory>>& shared, Location location);
		LevelFactory(const LevelFactory&) = delete;

		std::unique_ptr<Level> instantiate(Twist& rng, double renderDistance) const;

		const std::vector<std::shared_ptr<PatternFactory>>& getPatterns() const {return _patterns;}
		const std::vector<Color>& getColorsFG() const {return _colorsFg;}
		const std::vector<Color>& getColorsBG1() const {return _colorsBg1;}
		const std::vector<Color>& getColorsBG2() const {return _colorsBg2;}

		const std::string& getName() const {return _name;}
		const std::string& getDifficulty() const {return _difficulty;}
		const std::string& getMode() const {return _mode;}
		const std::string& getCreator() const {return _creator;}
		const std::string& getMusic() const {return _music;}

		Location getLocation() const {return _location;}
		int getHighScore() const {return _highScore;}
		int getSpeedPulse() const {return _speedPulse;}
		float getSpeedCursor() const {return _speedCursor;}
		float getSpeedRotation() const {return _speedRotation;}
		float getSpeedWall() const {return _speedWall;}

		bool setHighScore(int score);

	private:
		std::vector<std::shared_ptr<PatternFactory>> _patterns;
		std::vector<Color> _colorsFg;
		std::vector<Color> _colorsBg1;
		std::vector<Color> _colorsBg2;

		std::string _name;
		std::string _difficulty;
		std::string _mode;
		std::string _creator;
		std::string _music;

		Location _location = Location::INTERNAL;

		int _highScore = 0;
		int _speedPulse;
		float _speedWall;
		float _speedRotation;
		float _speedCursor;
	};
}

#endif //SUPER_HAXAGON_LEVEL_HPP
