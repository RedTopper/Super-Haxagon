#include <memory>

#include "Core/Game.hpp"
#include "Factories/Wall.hpp"
#include "Factories/Pattern.hpp"
#include "States/State.hpp"
#include "States/Load.hpp"

namespace SuperHaxagon {

	Game::Game(Platform& platform) : _platform(platform) {
		// Audio loading
		_sfxBegin = platform.loadAudio(platform.getPathRom("/sound/begin"), Stream::DIRECT);
		_sfxHexagon = platform.loadAudio(platform.getPathRom("/sound/hexagon"), Stream::DIRECT);
		_sfxOver = platform.loadAudio(platform.getPathRom("/sound/over"), Stream::DIRECT);
		_sfxSelect = platform.loadAudio(platform.getPathRom("/sound/select"), Stream::DIRECT);
		_sfxLevelUp = platform.loadAudio(platform.getPathRom("/sound/level"), Stream::DIRECT);

		_small = platform.loadFont(platform.getPathRom("/bump-it-up"), 16);
		_large = platform.loadFont(platform.getPathRom("/bump-it-up"), 32);

		_twister = platform.getTwister();
	}

	Game::~Game() {
		_platform.message(SuperHaxagon::Dbg::INFO, "game", "shutdown ok");
	}

	void Game::run() {
		_state = std::make_unique<Load>(*this);
		_state->enter();
		while(_running && _platform.loop()) {
			// The original game was built with a 3DS in mind, so when
			// drawing we have to scale the game to however many times larger the viewport is.
			const auto scale = getScreenDimMin() / 240.0;
			const auto dilation = _platform.getDilation();

			auto next = _state->update(dilation);
			if (!_running) break;
			while (next) {
				_state->exit();
				_state = std::move(next);
				_state->enter();
				next = _state->update(dilation);
			}

			_platform.screenBegin();
			_state->drawTop(scale);
			_platform.screenSwap();
			_state->drawBot(scale);
			_platform.screenFinalize();
		}
	}

	void Game::addLevel(std::unique_ptr<LevelFactory> level) {
		_levels.emplace_back(std::move(level));
	}

	void Game::drawBackground(const Color& color1, const Color& color2, const Point& focus, double multiplier, double rotation, double sides) const {
		// The game used to be based off a 3DS which has a bottom screen of 240px
		const auto maxRenderDistance = SCALE_BASE_DISTANCE * (getScreenDimMax() / 240);
		const auto exactSides = static_cast<int>(std::ceil(sides));

		//solid background.
		const Point position = {0,0};
		const auto size = _platform.getScreenDim();
		_platform.drawRect(color1, position, size);

		//This draws the main background.
		std::vector<Point> edges;
		edges.resize(exactSides);

		for(auto i = 0; i < exactSides; i++) {
			edges[i].x = multiplier * maxRenderDistance * cos(rotation + i * TAU / sides) + focus.x;
			edges[i].y = multiplier * maxRenderDistance * sin(rotation + i * TAU / sides + PI) + focus.y;
		}

		std::array<Point, 3> triangle{};
		triangle[0] = focus;

		//if the sides is odd we need to "make up a color" to put in the gap between the last and first color
		if(exactSides % 2) {
			triangle[1] = edges[static_cast<size_t>(exactSides) - 1];
			triangle[2] = edges[0];
			_platform.drawTriangle(interpolateColor(color1, color2, 0.5f), triangle);
		}

		//Draw the rest of the triangles
		for(auto i = 0; i < exactSides - 1; i = i + 2) {
			triangle[1] = edges[i];
			triangle[2] = edges[static_cast<size_t>(i) + 1];
			_platform.drawTriangle(color2, triangle);
		}
	}

	void Game::drawRegular(const Color& color, const Point& focus, const double height, const double rotation, const double sides) const {
		const auto exactSides = static_cast<int>(std::ceil(sides));

		std::vector<Point> edges;
		edges.resize(exactSides);

		// Calculate the triangle backwards so it overlaps correctly.
		for(auto i = 0; i < exactSides; i++) {
			edges[i].x = height * cos(rotation + i * TAU/sides) + focus.x;
			edges[i].y = height * sin(rotation + i * TAU/sides + PI) + focus.y;
		}

		std::array<Point, 3> triangle{};
		triangle[0] = focus;

		// Connect last triangle edge to first
		triangle[1] = edges[static_cast<size_t>(exactSides) - 1];
		triangle[2] = edges[0];
		_platform.drawTriangle(color, triangle);

		// Draw rest of regular polygon
		for(auto i = 0; i < exactSides - 1; i++) {
			triangle[1] = edges[i];
			triangle[2] = edges[static_cast<size_t>(i) + 1];
			_platform.drawTriangle(color, triangle);
		}
	}

	void Game::drawCursor(const Color& color, const Point& focus, const double cursor, const double rotation, const double offset, const double scale) const {
		// Note: A cursor and rotation of zero points to the left
		std::array<Point, 3> triangle{};
		triangle[0] = {offset * scale, -SCALE_HUMAN_WIDTH/2 * scale};
		triangle[1] = {offset * scale, SCALE_HUMAN_WIDTH/2 * scale};
		triangle[2] = {(SCALE_HUMAN_HEIGHT + offset) * scale, 0};
		for (auto& p : triangle) {
			const auto orig = rotateAroundOrigin(p, cursor + rotation);
			p.x = orig.x + focus.x;
			p.y = orig.y + focus.y;
		}

		_platform.drawTriangle(color, triangle);
	}

	void Game::drawPatterns(const Color& color, const Point& focus, const std::deque<std::unique_ptr<Pattern>>& patterns, const double rotation, const double sides, const double offset, const double scale) const {
		for(const auto& pattern : patterns) {
			for(const auto& wall : pattern->getWalls()) {
				drawWalls(color, focus, *wall, rotation, sides, offset, scale);
			}
		}
	}

	void Game::drawWalls(const Color& color, const Point& focus, const Wall& wall, const double rotation, const double sides, const double offset, const double scale) const {
		const auto distance = wall.getDistance() + offset;
		if(distance + wall.getHeight() < SCALE_HEX_LENGTH) return; //TOO_CLOSE;
		if(wall.getSide() >= sides) return; //NOT_IN_RANGE
		drawTrap(color, wall.calcPoints(focus, rotation, sides, offset, scale));
	}

	void Game::drawTrap(const Color color, const std::array<Point, 4>& points) const {
		std::array<Point, 3> triangle{};
		triangle[0] = points[0];
		triangle[1] = points[1];
		triangle[2] = points[2];
		_platform.drawTriangle(color, triangle);
		triangle[1] = points[2];
		triangle[2] = points[3];
		_platform.drawTriangle(color, triangle);
	}

	Point Game::getScreenCenter() const {
		const auto dim = _platform.getScreenDim();
		return {dim.x/2, dim.y/2};
	}

	Point Game::getShadowOffset() const {
		const auto min = getScreenDimMin();
		return {min/60, min/60};
	}
}


