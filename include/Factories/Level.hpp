#ifndef SUPER_HAXAGON_LEVEL_HPP
#define SUPER_HAXAGON_LEVEL_HPP

#include "Core/Structs.hpp"
#include "Factories/Pattern.hpp"

#include <memory>
#include <string>
#include <vector>
#include <deque>

namespace SuperHaxagon {
	enum class Location {
		INTERNAL,
		EXTERNAL
	};

	class Game;
	class LevelFactory;
	class PatternFactory;
	class Twist;

	class Level {
	public:
		static constexpr double DIFFICULTY_SCALAR_WALLS = 0.04;
		static constexpr double DIFFICULTY_SCALAR_ROT = 0.09;
		static constexpr double FLIP_FRAMES_MIN = 120;
		static constexpr double FLIP_FRAMES_MAX = 600;
		static constexpr double FRAMES_PER_CHANGE_SIDE = 50;
		static constexpr double FRAMES_PER_SPIN = 90;
		static constexpr double FRAMES_PER_PULSE = 10;
		static constexpr double SPIN_SPEED = TAU / 160.0;
		static constexpr double PULSE_DISTANCE = 4.0;

		Level(const LevelFactory& factory, Twist& rng, double patternDistCreate);
		Level(Level&) = delete;
		~Level();

		void update(Twist& rng, double patternDistDelete, double patternDistCreate, double dilation);
		void draw(Game& game, double scale, double offsetWall) const;
		Movement collision(double cursorDistance, double dilation) const;

		void increaseMultiplier();
		void clearPatterns();
		void rotate(double distance, double dilation);
		void left(double dilation);
		void right(double dilation);
		void clamp();

		// Effects
		void spin();
		void invertBG();
		void pulse(double scale);

		const LevelFactory& getLevelFactory() const {return _factory;}

	private:
		const LevelFactory& _factory;

		std::deque<Pattern> _patterns;

		double _multiplierRot = 0.9; // Current direction and speed of rotation
		double _multiplierWalls = 0.85; // Current speed of the walls flying at you
		double _cursorPos{};
		double _rotation{};
		double _sidesTween{};
		int _lastSides{};
		int _currentSides{};

		double _delayFrame{}; // Tween between side switches
		double _delayMax{};   // When a delay starts, this is the initial value of _delayFrame
		double _tweenFrame{}; // Tween colors
		double _flipFrame = FLIP_FRAMES_MAX; // Amount of frames left until flip

		int _indexBg1{};
		int _indexBg2{};
		int _indexFg{};
		int _nextIndexBg1{};
		int _nextIndexBg2{};
		int _nextIndexFg{};

		bool _bgInverted = false;
		double _pulse = 0.0;
		double _spin = 0.0;
	};

	class LevelFactory {
	public:
		static const char* LEVEL_HEADER;
		static const char* LEVEL_FOOTER;

		LevelFactory(std::ifstream& file, std::vector<std::shared_ptr<PatternFactory>>& shared, Location location, Platform& platform);
		LevelFactory(const LevelFactory&) = delete;

		std::unique_ptr<Level> instantiate(Twist& rng, double renderDistance) const;

		bool isLoaded() const {return _loaded;}

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
		int _speedPulse = 0;
		float _speedWall = 0;
		float _speedRotation = 0;
		float _speedCursor = 0;
		bool _loaded = false;
	};
}

#endif //SUPER_HAXAGON_LEVEL_HPP
