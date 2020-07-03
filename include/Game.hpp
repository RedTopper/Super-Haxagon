#ifndef SUPER_HAXAGON_GAME_HPP
#define SUPER_HAXAGON_GAME_HPP

#include <cmath>
#include <vector>
#include <memory>

#include "Driver/Platform.hpp"
namespace SuperHaxagon {
	struct Point;
	struct Color;
	class Level;
	class Audio;
	class State;
	class Game {
	public:
		explicit Game(Platform& platform);
		Game(const Game&) = delete;

		const std::vector<std::unique_ptr<Level>>& getLevels() const {return levels;}

		Platform& getPlatform() const {return platform;}
		Twist& getTwister() const {return *twister;}
		Audio& getSfxBegin() const {return *sfxBegin;}
		Audio& getSfxHexagon() const {return *sfxHexagon;}
		Audio& getSfxOver() const {return *sfxOver;}
		Audio& getSfxSelect() const {return *sfxSelect;}
		Audio& getSfxLevelUp() const {return *sfxLevelUp;}
		Audio& getBgmMenu() const {return *bgmMenu;}

		int getRenderDistance() const {return getScreenDimMax();}
		int getHexLength() const {return getScreenDimMin() / 10;}
		int getHexLengthBorder() const {return getScreenDimMin() / 60;}
		int getHumanPadding() const {return getScreenDimMin() / 48;}
		int getHumanHeight() const {return getScreenDimMin() / 48;}
		double getHumanWidth() const {return TAU/30.0;}

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
		void drawBackground(const Color& color1, const Color& color2, const Point& focus, double height, double rotation, double sides);

		/**
		 * Draws a regular polygon at some point focus. Useful for generating
		 * the regular polygon in the center of the screen.
		 */
		void drawRegular(const Color& color, const Point& focus, int height, double rotation, double sides);

		/**
		 * Draws the little cursor in the center of the screen controlled by a human.
		 */
		void drawHumanCursor(const Color& color, const Point& focus, double cursor, double rotation);

		/**
		 * Linear interpolation between two colors
		 */
		static Color interpolateColor(const Color& one, const Color& two, double percent);

		/**
		 * Linear interpolation between two doubles
		 */
		static double linear(double start, double end, double percent);

		/**
		 * Calculates a point (either for a wall or a general point) based on the running level's rotation, some offset
		 * so it renders correctly, some distance the point should be from the center, the side the point
		 * should be rendered on, based on a total number of sides the shape has.
		 */
		static Point calcPoint(const Point& focus, double rotation, double offset, double distance);

		/**
		 * Kind of like getScoreText(int score), this method converts a score
		 * into a string. The format of this method looks like such:
		 *
		 * TTT:%% where  TTT is the current time in seconds, and %% is  the percent
		 * through the current second.
		 *
		 * getBestTime(int score) is the same, but it says BEST: instead of TIME:
		 */
		static std::string getBestTime(int score);

	private:
		Platform& platform;

		std::vector<std::unique_ptr<Level>> levels;
		std::unique_ptr<Twist> twister;
		std::unique_ptr<State> state;
		std::unique_ptr<Audio> sfxBegin;
		std::unique_ptr<Audio> sfxHexagon;
		std::unique_ptr<Audio> sfxOver;
		std::unique_ptr<Audio> sfxSelect;
		std::unique_ptr<Audio> sfxLevelUp;
		std::unique_ptr<Audio> bgmMenu;
	};
}

#endif //SUPER_HAXAGON_GAME_HPP
