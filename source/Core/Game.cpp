#include <memory>

#include "Core/Game.hpp"
#include "Driver/Audio.hpp"
#include "Driver/Font.hpp"
#include "Factories/Wall.hpp"
#include "Factories/Pattern.hpp"
#include "States/State.hpp"
#include "States/Quit.hpp"
#include "States/Load.hpp"

namespace SuperHaxagon {

	Game::Game(Platform& platform) : platform(platform) {
		// Audio loading
		sfxBegin = platform.loadAudio(platform.getPathRom("/sound/begin.wav"));
		sfxHexagon = platform.loadAudio(platform.getPathRom("/sound/hexagon.wav"));
		sfxOver = platform.loadAudio(platform.getPathRom("/sound/over.wav"));
		sfxSelect = platform.loadAudio(platform.getPathRom("/sound/select.wav"));
		sfxLevelUp = platform.loadAudio(platform.getPathRom("/sound/level.wav"));
		bgmMenu = platform.loadAudio(platform.getPathRom("/bgm/pamgaea.wav"));

		small = platform.loadFont(platform.getPathRom("/bump-it-up"), 16);
		large = platform.loadFont(platform.getPathRom("/bump-it-up"), 32);

		twister = platform.getTwister();
	}

	Game::~Game() = default;

	int Game::run() {
		state = std::make_unique<Load>(*this);
		state->enter();
		while(platform.loop() && !dynamic_cast<Quit*>(state.get())) {
			// The original game was built with a 3DS in mind, so when
			// drawing we have to scale the game to however many times larger the viewport is.
			double scale = getScreenDimMin() / 240.0;

			std::unique_ptr<State> next = state->update(platform.getDilation());
			if (next) {
				state->exit();
				state = std::move(next);
				state->enter();
				state->update(platform.getDilation());
			}

			platform.screenBegin();
			state->drawTop(scale);
			platform.screenSwap();
			state->drawBot(scale);
			platform.screenFinalize();
		}

		return 0;
	}

	void Game::addLevel(std::unique_ptr<LevelFactory> level) {
		levels.emplace_back(std::move(level));
	}

	void Game::drawBackground(const Color& color1, const Color& color2, const Point& focus, double multiplier, double rotation, double sides) const {
		// The game used to be based off a 3DS which has a bottom screen of 240px
		double maxRenderDistance = SCALE_BASE_DISTANCE * (getScreenDimMax() / 240);
		int exactSides = std::ceil(sides);

		//solid background.
		Point position = {0,0};
		Point size = platform.getScreenDim();
		platform.drawRect(color1, position, size);

		//This draws the main background.
		std::vector<Point> edges;
		edges.reserve(exactSides);

		for(int i = 0; i < exactSides; i++) {
			edges[i].x = multiplier * maxRenderDistance * cos(rotation + (double)i * TAU / sides) + (double)(focus.x);
			edges[i].y = multiplier * maxRenderDistance * sin(rotation + (double)i * TAU / sides) + (double)(focus.y);
		}

		std::array<Point, 3> triangle{};
		triangle[0] = focus;

		//if the sides is odd we need to "make up a color" to put in the gap between the last and first color
		if(exactSides % 2) {
			triangle[1] = edges[exactSides - 1];
			triangle[2] = edges[0];
			platform.drawTriangle(interpolateColor(color1, color2, 0.5f), triangle);
		}

		//Draw the rest of the triangles
		for(int i = 0; i < exactSides - 1; i = i + 2) {
			triangle[1] = edges[i];
			triangle[2] = edges[i + 1];
			platform.drawTriangle(color2, triangle);
		}
	}

	void Game::drawRegular(const Color& color, const Point& focus, double height, double rotation, double sides) const {
		int exactSides = std::ceil(sides);

		std::vector<Point> edges;
		edges.reserve(exactSides);

		// Calculate the triangle backwards so it overlaps correctly.
		for(int i = 0; i < exactSides; i++) {
			edges[i].x = height * cos(rotation + (double)i * TAU/sides) + (double)(focus.x);
			edges[i].y = height * sin(rotation + (double)i * TAU/sides) + (double)(focus.y);
		}

		std::array<Point, 3> triangle{};
		triangle[0] = focus;

		// Connect last triangle edge to first
		triangle[1] = edges[exactSides - 1];
		triangle[2] = edges[0];
		platform.drawTriangle(color, triangle);

		// Draw rest of regular polygon
		for(int i = 0; i < exactSides - 1; i++) {
			triangle[1] = edges[i];
			triangle[2] = edges[i + 1];
			platform.drawTriangle(color, triangle);
		}
	}

	void Game::drawCursor(const Color& color, const Point& focus, double cursor, double rotation, double scale) const {
		std::array<Point, 3> triangle{};
		triangle[0] = calcPoint(focus, cursor + rotation, 0.0, (SCALE_HEX_LENGTH + SCALE_HUMAN_PADDING + SCALE_HUMAN_HEIGHT) * scale);
		triangle[1] = calcPoint(focus, cursor + rotation, HUMAN_WIDTH_RAD/2, (SCALE_HEX_LENGTH + SCALE_HUMAN_PADDING) * scale);
		triangle[2] = calcPoint(focus, cursor + rotation, -HUMAN_WIDTH_RAD/2, (SCALE_HEX_LENGTH + SCALE_HUMAN_PADDING) * scale);
		platform.drawTriangle(color, triangle);
	}

	void Game::drawPatterns(const Color& color, const Point& focus, const std::deque<std::unique_ptr<Pattern>>& patterns, double rotation, double sides, double offset, double scale) const {
		for(const auto& pattern : patterns) {
			for(const auto& wall : pattern->getWalls()) {
				drawWalls(color, focus, *wall, rotation, sides, offset, scale);
			}
		}
	}

	void Game::drawWalls(const Color& color, const Point& focus, const Wall& wall, double rotation, double sides, double offset, double scale) const {
		double distance = wall.getDistance() + offset;
		if(distance + wall.getHeight() < SCALE_HEX_LENGTH) return; //TOO_CLOSE;
		if(wall.getSide() >= sides) return; //NOT_IN_RANGE
		drawTrap(color, wall.calcPoints(focus, rotation, sides, offset, scale));
	}

	void Game::drawTrap(Color color, const std::array<Point, 4>& points) const {
		std::array<Point, 3> triangle{};
		triangle[0] = points[0];
		triangle[1] = points[1];
		triangle[2] = points[2];
		platform.drawTriangle(color, triangle);
		triangle[1] = points[2];
		triangle[2] = points[3];
		platform.drawTriangle(color, triangle);
	}

	Point Game::getScreenCenter() const {
		Point dim = platform.getScreenDim();
		return {dim.x/2, dim.y/2};
	}

	Point Game::getShadowOffset() const {
		double min = getScreenDimMin();
		return {min/60, -min/60};
	}
}


