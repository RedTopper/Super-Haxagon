#ifndef SUPER_HAXAGON_GAME_HPP
#define SUPER_HAXAGON_GAME_HPP

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

		const std::vector<std::unique_ptr<LevelFactory>>& getLevels() const {return _levels;}

		Platform& getPlatform() const {return _platform;}
		Twist& getTwister() const {return *_twister;}
		Audio& getSfxBegin() const {return *_sfxBegin;}
		Audio& getSfxHexagon() const {return *_sfxHexagon;}
		Audio& getSfxOver() const {return *_sfxOver;}
		Audio& getSfxSelect() const {return *_sfxSelect;}
		Audio& getSfxLevelUp() const {return *_sfxLevelUp;}

		Font& getFontSmall() const {return *_small;}
		Font& getFontLarge() const {return *_large;}

		double getScreenDimMax() const {
			const auto size = _platform.getScreenDim();
			return std::max(size.x, size.y);
		}

		double getScreenDimMin() const {
			const auto size = _platform.getScreenDim();
			return std::min(size.x, size.y);
		}

		/**
		 * Runs the game
		 */
		int run();

		/**
		 * Loads a level into the game
		 */
		void addLevel(std::unique_ptr<LevelFactory> level);

		/**
		 * Draws the background of the screen (the radiating colors part)
		 */
		void drawBackground(const Color& color1, const Color& color2, const Point& focus, double multiplier, double rotation, double sides) const;

		/**
		 * Draws a regular polygon at some point focus. Useful for generating
		 * the regular polygon in the center of the screen.
		 */
		void drawRegular(const Color& color, const Point& focus, double height, double rotation, double sides) const;

		/**
		 * Draws the little cursor in the center of the screen controlled by a human.
		 */
		void drawCursor(const Color& color, const Point& focus, double cursor, double rotation, double scale) const;

		/**
		 * Completely draws all patterns in a live level. Can also be used to create
		 * an "Explosion" effect if you use "offset". (for game overs)
		 */
		void drawPatterns(const Color& color, const Point& focus, const std::deque<std::unique_ptr<Pattern>>& patterns, double rotation, double sides, double offset, double scale) const;

		/**
		 * Draws a single moving wall based on a live wall, a color, some rotational value, and the total
		 * amount of sides that appears.
		 */
		void drawWalls(const Color& color, const Point& focus, const Wall& wall, double rotation, double sides, double offset, double scale) const;

		/**
		 * Draws a trapezoid using an array of points and a color.
		 * The array must have 4 points.
		 */
		void drawTrap(Color color, const std::array<Point, 4>& points) const;

		/**
		 * Gets the center of the screen from the platform
		 */
		Point getScreenCenter() const;

		/**
		 * Gets the offset in pixels of the shadow
		 */
		Point getShadowOffset() const;

	private:
		Platform& _platform;

		std::vector<std::unique_ptr<LevelFactory>> _levels;

		std::unique_ptr<Twist> _twister;
		std::unique_ptr<State> _state;

		std::unique_ptr<Audio> _sfxBegin;
		std::unique_ptr<Audio> _sfxHexagon;
		std::unique_ptr<Audio> _sfxOver;
		std::unique_ptr<Audio> _sfxSelect;
		std::unique_ptr<Audio> _sfxLevelUp;

		std::unique_ptr<Font> _small;
		std::unique_ptr<Font> _large;
	};
}

#endif //SUPER_HAXAGON_GAME_HPP
