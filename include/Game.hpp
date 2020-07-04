#ifndef SUPER_HAXAGON_GAME_HPP
#define SUPER_HAXAGON_GAME_HPP

#include <cmath>
#include <deque>
#include <memory>

#include "Driver/Platform.hpp"

namespace SuperHaxagon {
	struct Point;
	struct Color;
	class LevelFactory;
	class Audio;
	class State;
	class Pattern;
	class Wall;

	class Game {
	public:
		explicit Game(Platform& platform);
		Game(const Game&) = delete;
		~Game();

		const std::vector<std::unique_ptr<LevelFactory>>& getLevels() const {return levels;}
		void addLevel(std::unique_ptr<LevelFactory> level) {levels.push_back(level);}

		const std::vector<std::unique_ptr<PatternFactory>>& getPatterns() const {return patterns;}
		void addPattern(std::unique_ptr<PatternFactory> pattern) {patterns.push_back(pattern);}

		Platform& getPlatform() const {return platform;}
		Twist& getTwister() const {return *twister;}
		Audio& getSfxBegin() const {return *sfxBegin;}
		Audio& getSfxHexagon() const {return *sfxHexagon;}
		Audio& getSfxOver() const {return *sfxOver;}
		Audio& getSfxSelect() const {return *sfxSelect;}
		Audio& getSfxLevelUp() const {return *sfxLevelUp;}
		Audio& getBgmMenu() const {return *bgmMenu;}

		Font& getFontSmall() const {return *small;}
		Font& getFontLarge() const {return *large;}

		int getRenderDistance() const {return getScreenDimMax();}
		int getHexLength() const {return getScreenDimMin() / 10;}
		int getHexLengthBorder() const {return getScreenDimMin() / 60;}
		int getHumanPadding() const {return getScreenDimMin() / 48;}
		int getHumanHeight() const {return getScreenDimMin() / 48;}
		static double getHumanWidth() {return TAU/30.0;}

		int getScreenDimMax() const {
			auto size = platform.getScreenDim();
			return std::max(size.x, size.y);
		}

		int getScreenDimMin() const {
			auto size = platform.getScreenDim();
			return std::min(size.x, size.y);
		}

		/**
		 * Runs the game
		 */
		int run();

		/**
		 * Draws the background of the screen (the radiating colors part)
		 */
		void drawBackground(const Color& color1, const Color& color2, const Point& focus, double height, double rotation, double sides) const;

		/**
		 * Draws a regular polygon at some point focus. Useful for generating
		 * the regular polygon in the center of the screen.
		 */
		void drawRegular(const Color& color, const Point& focus, int height, double rotation, double sides) const;

		/**
		 * Draws the little cursor in the center of the screen controlled by a human.
		 */
		void drawCursor(const Color& color, const Point& focus, double cursor, double rotation) const;

		/**
		 * Completely draws all patterns in a live level. Can also be used to create
		 * an "Explosion" effect if you use "offset". (for game overs)
		 */
		void drawPatterns(const Color& color, const Point& focus, const std::deque<std::unique_ptr<Pattern>>& patterns, double rotation, double sides, double offset) const;

		/**
		 * Draws a single moving wall based on a live wall, a color, some rotational value, and the total
		 * amount of sides that appears.
		 */
		void drawWalls(const Color& color, const Point& focus, const Wall& wall, double rotation, double sides, double offset) const;

		/**
		 * Draws a trapezoid using an array of points and a color.
		 * The array must have 4 points.
		 */
		void drawTrap(Color color, const std::array<Point, 4>& points) const;

		/**
		 * Clears the bottom screen, if the platftorm has one.
		 */
		void clearBotAndSwitchScreens() const;

		/**
		 * Gets the center of the screen from the platform
		 */
		Point getScreenCenter() const;

		/**
		 * Gets the offset in pixels of the shadow
		 */
		Point getShadowOffset() const;

	private:
		Platform& platform;

		std::vector<std::unique_ptr<LevelFactory>> levels;
		std::vector<std::unique_ptr<PatternFactory>> patterns;

		std::unique_ptr<Twist> twister;
		std::unique_ptr<State> state;

		std::unique_ptr<Audio> sfxBegin;
		std::unique_ptr<Audio> sfxHexagon;
		std::unique_ptr<Audio> sfxOver;
		std::unique_ptr<Audio> sfxSelect;
		std::unique_ptr<Audio> sfxLevelUp;
		std::unique_ptr<Audio> bgmMenu;

		std::unique_ptr<Font> small;
		std::unique_ptr<Font> large;
	};
}

#endif //SUPER_HAXAGON_GAME_HPP
