#ifndef SUPER_HAXAGON_GAME_HPP
#define SUPER_HAXAGON_GAME_HPP

#include <deque>
#include <memory>
#include <vector>
#include <string>

namespace SuperHaxagon {
	// Maybe I went a bit overboard with PImpl...
	struct Point;
	struct Color;
	class LevelFactory;
	class State;
	class Pattern;
	class Wall;
	class Platform;
	class Twist;
	class Font;
	class Metadata;
	enum class Location;

	class Game {
	public:
		explicit Game(Platform& platform);
		Game(const Game&) = delete;
		~Game();

		const std::vector<std::unique_ptr<LevelFactory>>& getLevels() const {return _levels;}

		Platform& getPlatform() const {return _platform;}
		Twist& getTwister() const {return *_twister;}
		Metadata* getBGMMetadata() const {return _bgmMetadata.get();}
		Font& getFontSmall() const;
		Font& getFontLarge() const;
		float getScreenDimMax() const;
		float getScreenDimMin() const;
		void loadBGMAudio(const std::string& music, Location location, bool loadMetadata);

		void setRunning(const bool running) {_running = running;}
		void setSkew(const float skew) {_skew = skew;}
		void setShadowAuto(const bool shadowAuto) {_shadowAuto = shadowAuto;}

		/**
		 * Runs the game
		 */
		void run();

		/**
		 * Loads a level into the game
		 */
		void addLevel(std::unique_ptr<LevelFactory> level);

		/**
		 * Draws a rectangle at position with the size of size.
		 * Position is the top left.
		 */
		void drawRect(Color color, Point position, Point size) const;

		/**
		 * Draws the background of the screen (the radiating colors part)
		 */
		void drawBackground(const Color& color1, const Color& color2, const Point& focus, float multiplier, float rotation, float sides) const;

		/**
		 * Draws a regular polygon at some point focus. Useful for generating
		 * the regular polygon in the center of the screen.
		 */
		void drawRegular(const Color& color, const Point& focus, float height, float rotation, float sides) const;

		/**
		 * Draws the little cursor in the center of the screen controlled by a human.
		 */
		void drawCursor(const Color& color, const Point& focus, float cursor, float rotation, float offset, float scale) const;

		/**
		 * Completely draws all patterns in a live level. Can also be used to create
		 * an "Explosion" effect if you use "offset". (for game overs)
		 */
		void drawPatterns(const Color& color, const Point& focus, const std::deque<Pattern>& patterns, float rotation, float sides, float offset, float scale) const;

		/**
		 * Draws a single moving wall based on a live wall, a color, some rotational value, and the total
		 * amount of sides that appears.
		 */
		void drawWalls(const Color& color, const Point& focus, const Wall& wall, float rotation, float sides, float offset, float scale) const;

		/**
		 * Gets the center of the screen from the platform
		 */
		Point getScreenCenter() const;

		/**
		 * Gets the offset in pixels of the shadow
		 */
		Point getShadowOffset() const;

		/**
		 * Skews the screen to give a 3D effect. Modifies the incoming triangle
		 */
		void skew(std::vector<Point>& skew) const;

	private:
		Platform& _platform;

		std::vector<std::unique_ptr<LevelFactory>> _levels;

		std::unique_ptr<Twist> _twister;
		std::unique_ptr<State> _state;

		std::unique_ptr<Metadata> _bgmMetadata;

		bool _running = true;
		bool _shadowAuto = false;
		float _skew = 0.0;
	};
}

#endif //SUPER_HAXAGON_GAME_HPP
