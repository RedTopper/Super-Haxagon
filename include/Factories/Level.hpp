#ifndef SUPER_HAXAGON_LEVEL_HPP
#define SUPER_HAXAGON_LEVEL_HPP

#include "Core/Structs.hpp"
#include "Factories/Pattern.hpp"

#include <memory>
#include <string>
#include <vector>
#include <deque>
#include <map>

namespace SuperHaxagon {	
	class Game;
	class LevelFactory;
	class PatternFactory;
	class Twist;

	class Level {
	public:
		static constexpr double DIFFICULTY_SCALAR_WALLS = 0.0375;
		static constexpr double DIFFICULTY_SCALAR_ROT = 0.08;
		static constexpr double FLIP_FRAMES_MIN = 120;
		static constexpr double FLIP_FRAMES_MAX = 600;
		static constexpr double FRAMES_PER_CHANGE_SIDE = 50;
		static constexpr double FRAMES_PER_SPIN = 90;
		static constexpr double FRAMES_PER_PULSE = 10;
		static constexpr double SPIN_SPEED = TAU / 130.0;
		static constexpr double ROTATE_ZERO_SPEED = TAU / 200.0;
		static constexpr double PULSE_DISTANCE = 5.0;

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

		// Time
		double getFrame() const {return _frame;}

		const LevelFactory& getLevelFactory() const {return *_factory;}

		// Stuff for Win control
		std::deque<Pattern>& getPatterns() {return _patterns;}
		void setWinMultiplierRot(const double multiplier) {_multiplierRot = multiplier;}
		void setWinMultiplierWalls(const double multiplier) {_multiplierWalls = multiplier;}
		void setWinAutoPatternCreate(const bool autoPatternCreate) {_autoPatternCreate = autoPatternCreate;}
		void setWinShowCursor(const bool show) {_showCursor = show;}
		void setWinFrame(const double frame) {_frame = frame;}
		void setWinRotationToZero() {_rotateToZero = true;}
		void setWinFactory(const LevelFactory* factory);
		void setWinSides(int sides);
		void resetColors();

	private:
		void advanceWalls(Twist& rng, double patternDistDelete, double patternDistCreate);
		void reverseWalls(Twist& rng, double patternDistDelete, double patternDistCreate);
		
		const LevelFactory* _factory;

		std::deque<Pattern> _patterns;

		bool _autoPatternCreate = false;
		bool _showCursor = true;
		bool _rotateToZero = false;
		
		double _multiplierRot = 0.9; // Current direction and speed of rotation
		double _multiplierWalls = 0.85; // Current speed of the walls flying at you
		double _cursorPos{};
		double _rotation{};
		double _sidesTween{};
		int _lastSides{};
		int _currentSides{};

		double _frame{}; // Frame this level is on
		double _delayFrame{}; // Tween between side switches
		double _delayMax{};   // When a delay starts, this is the initial value of _delayFrame
		double _tweenFrame{}; // Tween colors
		double _flipFrame = FLIP_FRAMES_MAX; // Amount of frames left until flip

		std::map<LocColor, Color> _color;
		std::map<LocColor, Color> _colorNext;
		std::map<LocColor, size_t> _colorNextIndex;

		bool _bgInverted = false;
		double _pulse = 0.0;
		double _spin = 0.0;
		double _frontGap = 0.0;
	};

	class LevelFactory {
	public:
		static const char* LEVEL_HEADER;
		static const char* LEVEL_FOOTER;

		LevelFactory(std::ifstream& file, std::vector<std::shared_ptr<PatternFactory>>& shared, LocLevel location, Platform& platform, size_t levelIndexOffset);
		LevelFactory(const LevelFactory&) = delete;

		std::unique_ptr<Level> instantiate(Twist& rng, double renderDistance) const;

		bool isLoaded() const {return _loaded;}

		const std::vector<std::shared_ptr<PatternFactory>>& getPatterns() const {return _patterns;}
		const std::map<LocColor, std::vector<Color>>& getColors() const {return _colors;}

		const std::string& getName() const {return _name;}
		const std::string& getDifficulty() const {return _difficulty;}
		const std::string& getMode() const {return _mode;}
		const std::string& getCreator() const {return _creator;}
		const std::string& getMusic() const {return _music;}

		LocLevel getLocation() const {return _location;}
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

		LocLevel _location = LocLevel::INTERNAL;

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

#endif //SUPER_HAXAGON_LEVEL_HPP
