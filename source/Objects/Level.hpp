#ifndef SUPER_HAXAGON_LEVEL_HPP
#define SUPER_HAXAGON_LEVEL_HPP

// SPDX-FileCopyrightText: 2020 AJ Walter
// SPDX-License-Identifier: GPL-3.0

#include "Core/Structs.hpp"
#include "Objects/Pattern.hpp"

#include <deque>
#include <map>

namespace SuperHaxagon {
	class Game;
	class LevelFactory;
	class PatternFactory;
	class Random;
	class SurfaceGame;

	class Level {
	public:
		static constexpr float STARTING_MULTIPLIER_WALLS = 0.9f;
		static constexpr float STARTING_MULTIPLIER_ROT = 0.9f;
		static constexpr float DIFFICULTY_SCALAR_WALLS = 0.2f;
		static constexpr float DIFFICULTY_SCALAR_ROT = 0.07f;
		static constexpr float FLIP_FRAMES_MIN = 120.0f;
		static constexpr float FLIP_FRAMES_MAX = 600.0f;
		static constexpr float FRAMES_DIFFICULTY_INCREASE = 120.0f;
		static constexpr float FRAMES_PER_CHANGE_SIDE = 30.0f;
		static constexpr float FRAMES_PER_SPIN = 75.0f;
		static constexpr float SPIN_SPEED = TAU / 100.0f;
		static constexpr float ROTATE_ZERO_SPEED = TAU / 200.0f;
		static constexpr float HEXAGON_PULSE_MULTIPLIER = 4.25f;
		static constexpr int MIN_SAME_SIDES = 3;
		static constexpr int MAX_SAME_SIDES = 5;

		Level(const LevelFactory& factory, Random& rng, float patternDistCreate, float rotation, float cursorPos);
		Level(Level&) = delete;
		~Level();

		void update(Random& rng, float dilation);
		void draw(SurfaceGame& game, SurfaceGame* shadows, float offset, float scaleFactor) const;
		Movement collision(float cursorDistance, float dilation) const;

		void increaseRotationMultiplier();
		void queueDifficultyIncrease();
		void clearPatterns();
		void rotate(float distance, float dilation);
		void left(float dilation);
		void right(float dilation);
		void clamp();

		// Effects
		void spin();
		void invertBG();

		// Time
		float getScore() const {return _score;}
		bool isDelaying() const {return _delayFrame > 0.0f;}

		// Transition information
		GameColors getCurrentColor() const {return _colorCurrent;}
		float getCurrentRotation() const {return _rotation;}
		float getCursorPos() const {return _cursorPos;}
		int getRotationDirection() const {return _multiplierRot < 0.0f ? -1 : 1;}

		const LevelFactory& getLevelFactory() const {return *_factory;}

		// Stuff for Win control
		std::deque<Pattern>& getPatterns() {return _patterns;}
		void setWinMultiplierRot(const float multiplier) {_multiplierRot = multiplier;}
		void setWinMultiplierWalls(const float multiplier) {_multiplierWalls = multiplier;}
		void setWinAutoPatternCreate(const bool autoPatternCreate) {_autoPatternCreate = autoPatternCreate;}
		void setWinShowCursor(const bool show) {_showCursor = show;}
		void setWinScore(const float frame) { _score = frame;}
		void setWinRotationToZero() {_rotateToZero = true;}
		void setWinFactory(const LevelFactory* factory);
		void setWinSides(int sides);
		void resetColors();

	private:
		void advanceWalls(Random& rng, float patternDistDelete, float patternDistCreate);
		void reverseWalls(Random& rng, float patternDistDelete, float patternDistCreate);
		const PatternFactory& getRandomPattern(Random& rng);
		
		const LevelFactory* _factory;

		std::deque<Pattern> _patterns;

		bool _autoPatternCreate = false;
		bool _showCursor = true;
		bool _rotateToZero = false;
		bool _difficultyIncrease = false;
		
		float _multiplierRot = STARTING_MULTIPLIER_ROT; // Current direction and speed of rotation
		float _multiplierWalls = STARTING_MULTIPLIER_WALLS; // Current speed of the walls flying at you
		float _cursorPos{};
		float _rotation{};
		float _sidesTween{};

		// Side change mechanics
		int _sidesLast{}; // The sides that we are transitioning FROM
		int _sidesCurrent{}; // The sides we are transitioning TO

		// Event timings
		float _score{}; // Internal score for this level
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
		float _spin = 0.0;
		float _frontGap = 0.0;
	};
}

#endif //SUPER_HAXAGON_LEVEL_HPP
