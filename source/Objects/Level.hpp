#ifndef SUPER_HAXAGON_LEVEL_HPP
#define SUPER_HAXAGON_LEVEL_HPP

#include "Core/Structs.hpp"
#include "Objects/Pattern.hpp"

#include <deque>
#include <map>

namespace SuperHaxagon {
	class Game;
	class LevelFactory;
	class PatternFactory;
	class Twist;
	class SurfaceGame;

	class Level {
	public:
		static constexpr float DIFFICULTY_SCALAR_WALLS = 0.0375f;
		static constexpr float DIFFICULTY_SCALAR_ROT = 0.08f;
		static constexpr float FLIP_FRAMES_MIN = 120;
		static constexpr float FLIP_FRAMES_MAX = 600;
		static constexpr float FRAMES_PER_CHANGE_SIDE = 30;
		static constexpr float FRAMES_PER_SPIN = 90;
		static constexpr float FRAMES_PER_PULSE = 15;
		static constexpr float SPIN_SPEED = TAU / 130.0f;
		static constexpr float ROTATE_ZERO_SPEED = TAU / 200.0f;
		static constexpr float PULSE_DISTANCE = 0.05f;
		static constexpr int MIN_SAME_SIDES = 3;
		static constexpr int MAX_SAME_SIDES = 5;

		Level(const LevelFactory& factory, Twist& rng, float patternDistCreate);
		Level(Level&) = delete;
		~Level();

		void update(Twist& rng, float dilation);
		void draw(SurfaceGame& game, SurfaceGame* shadows, float offset) const;
		Movement collision(float cursorDistance, float dilation) const;

		void increaseMultiplier();
		void clearPatterns();
		void rotate(float distance, float dilation);
		void left(float dilation);
		void right(float dilation);
		void clamp();

		// Effects
		void spin();
		void invertBG();
		void pulse(float pulse);

		// Time
		float getFrame() const {return _frame;}
		GameColors getCurrentColor() const {return _colorCurrent;}

		const LevelFactory& getLevelFactory() const {return *_factory;}

		// Stuff for Win control
		std::deque<Pattern>& getPatterns() {return _patterns;}
		void setWinMultiplierRot(const float multiplier) {_multiplierRot = multiplier;}
		void setWinMultiplierWalls(const float multiplier) {_multiplierWalls = multiplier;}
		void setWinAutoPatternCreate(const bool autoPatternCreate) {_autoPatternCreate = autoPatternCreate;}
		void setWinShowCursor(const bool show) {_showCursor = show;}
		void setWinFrame(const float frame) {_frame = frame;}
		void setWinRotationToZero() {_rotateToZero = true;}
		void setWinFactory(const LevelFactory* factory);
		void setWinSides(int sides);
		void resetColors();

	private:
		void advanceWalls(Twist& rng, float patternDistDelete, float patternDistCreate);
		void reverseWalls(Twist& rng, float patternDistDelete, float patternDistCreate);
		const PatternFactory& getRandomPattern(Twist& rng);
		
		const LevelFactory* _factory;

		std::deque<Pattern> _patterns;

		bool _autoPatternCreate = false;
		bool _showCursor = true;
		bool _rotateToZero = false;
		
		float _multiplierRot = 0.9f; // Current direction and speed of rotation
		float _multiplierWalls = 0.85f; // Current speed of the walls flying at you
		float _cursorPos{};
		float _rotation{};
		float _sidesTween{};

		// Side change mechanics
		int _sidesLast{}; // The sides that we are transitioning FROM
		int _sidesCurrent{}; // The sides we are transitioning TO

		// Event timings
		float _frame{}; // Frame this level is on
		float _delayFrame{}; // Tween between side switches
		float _delayMax{};   // When a delay starts, this is the initial value of _delayFrame
		float _tweenFrame{}; // Tween colors
		float _flipFrame = FLIP_FRAMES_MAX; // Amount of frames left until it rotates in the opposite direction

		GameColors _colorCurrent{};
		std::map<LocColor, Color> _color;
		std::map<LocColor, Color> _colorNext;
		std::map<LocColor, size_t> _colorNextIndex;

		int _sameCount = 0; // When 0, allows the level to select any pattern instead of currentSides
		int _sameSides = 0; // Sides of the last selected pattern
		bool _bgInverted = false;
		float _pulse = 0.0;
		float _spin = 0.0;
		float _frontGap = 0.0;
	};
}

#endif //SUPER_HAXAGON_LEVEL_HPP
