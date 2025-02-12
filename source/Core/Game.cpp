#include "Core/Game.hpp"

#include "Core/Metadata.hpp"
#include "Core/Twist.hpp"
#include "Driver/Font.hpp"
#include "Driver/Sound.hpp"
#include "Driver/Music.hpp"
#include "Driver/Platform.hpp"
#include "Factories/LevelFactory.hpp"
#include "Factories/PatternFactory.hpp"
#include "States/Load.hpp"

#include <cmath>

namespace SuperHaxagon {

	Game::Game(Platform& platform) : _platform(platform) {
		// Audio loading
		const std::vector<std::pair<SoundEffect, std::string>> sounds{
			{SoundEffect::BEGIN, "/sound/begin"},
			{SoundEffect::HEXAGON, "/sound/hexagon"},
			{SoundEffect::OVER, "/sound/over"},
			{SoundEffect::SELECT, "/sound/select"},
			{SoundEffect::LEVEL_UP, "/sound/level"},
			{SoundEffect::WONDERFUL, "/sound/wonderful"},
		};

		for (const auto& sound : sounds) {
			_soundEffects.emplace_back(sound.first, platform.loadSound(sound.second));
		}

		_fontSmall = platform.loadFont(16);
		_fontLarge = platform.loadFont(32);
		_twister = platform.getTwister();
	}

	Game::~Game() {
		// Stop and unload music
		_bgm = nullptr;
		_platform.message(SuperHaxagon::Dbg::INFO, "game", "shutdown ok");
	}

	void Game::run() {
		_state = std::make_unique<Load>(*this);
		_state->enter();
		while(_running && _platform.loop()) {
			// The original game was built with a 3DS in mind, so when
			// drawing we have to scale the game to however many times larger the viewport is.
			const auto scale = getScreenDimMin() / 240.0f;
			auto dilation = _platform.getDilation();

			// Throttle how wildly off dilation can be on all platforms.
			// A dilation of 1.0 is 1/60th of a second. Huge dilation spikes can happen
			// when the process is suspended (for example, the 3ds on the home menu)
			dilation = dilation > 5.0f ? 5.0f : (dilation < 0.05f ? 0.05f : dilation);

			// For platforms that need it, tick the BGM.
			if (_bgm) _bgm->update();

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

	void Game::drawRect(const Color color, const Point position, const Point size) const {
		const std::vector<Point> points{
			{position.x, position.y + size.y},
			{position.x + size.x, position.y + size.y},
			{position.x + size.x, position.y},
			{position.x, position.y},
		};

		_platform.drawPoly(color, points);
	}

	void Game::drawBackground(const Color& color1, const Color& color2, const Point& focus, const float multiplier, const float rotation, const float sides) const {
		// The game used to be based off a 3DS which has a bottom screen of 240px
		const auto maxRenderDistance = SCALE_BASE_DISTANCE * (getScreenDimMax() / 240);
		const auto exactSides = static_cast<size_t>(std::ceil(sides));

		//solid background.
		const Point position = {0,0};
		const auto size = _platform.getScreenDim();
		drawRect(color1, position, size);

		//This draws the main background.
		std::vector<Point> edges;
		edges.resize(exactSides);

		for(size_t i = 0; i < exactSides; i++) {
			edges[i].x = multiplier * maxRenderDistance * cos(rotation + static_cast<float>(i) * TAU / sides) + focus.x;
			edges[i].y = multiplier * maxRenderDistance * sin(rotation + static_cast<float>(i) * TAU / sides + PI) + focus.y;
		}

		std::vector<Point> triangle;
		triangle.resize(3);

		//if the sides is odd we need to "make up a color" to put in the gap between the last and first color
		if(exactSides % 2) {
			triangle[0] = focus;
			triangle[1] = edges[exactSides - 1];
			triangle[2] = edges[0];
			skew(triangle);
			_platform.drawPoly(interpolateColor(color1, color2, 0.5f), triangle);
		}

		//Draw the rest of the triangles
		for(size_t i = 0; i < exactSides - 1; i = i + 2) {
			triangle[0] = focus;
			triangle[1] = edges[i];
			triangle[2] = edges[i + 1];
			skew(triangle);
			_platform.drawPoly(color2, triangle);
		}
	}

	void Game::drawRegular(const Color& color, const Point& focus, const float height, const float rotation, const float sides) const {
		const auto exactSides = static_cast<size_t>(std::ceil(sides));

		std::vector<Point> edges;
		edges.resize(exactSides);

		// Calculate the triangle backwards so it overlaps correctly.
		for(size_t i = 0; i < exactSides; i++) {
			edges[i].x = height * cos(rotation + static_cast<float>(i) * TAU/sides) + focus.x;
			edges[i].y = height * sin(rotation + static_cast<float>(i) * TAU/sides + PI) + focus.y;
		}

		skew(edges);
		_platform.drawPoly(color, edges);
	}

	void Game::drawCursor(const Color& color, const Point& focus, const float cursor, const float rotation, const float offset, const float scale) const {
		// Note: A cursor and rotation of zero points to the left
		std::vector<Point> triangle;
		triangle.resize(3);
		triangle[0] = {offset * scale, -SCALE_HUMAN_WIDTH/2 * scale};
		triangle[1] = {offset * scale, SCALE_HUMAN_WIDTH/2 * scale};
		triangle[2] = {(SCALE_HUMAN_HEIGHT + offset) * scale, 0};
		for (auto& p : triangle) {
			const auto orig = rotateAroundOrigin(p, cursor + rotation);
			p.x = orig.x + focus.x;
			p.y = orig.y + focus.y;
		}

		skew(triangle);
		_platform.drawPoly(color, triangle);
	}

	void Game::drawPatterns(const Color& color, const Point& focus, const std::deque<Pattern>& patterns, const float rotation, const float sides, const float offset, const float scale) const {
		for(const auto& pattern : patterns) {
			for(const auto& wall : pattern.getWalls()) {
				drawWalls(color, focus, wall, rotation, sides, offset, scale);
			}
		}
	}

	void Game::drawWalls(const Color& color, const Point& focus, const Wall& wall, const float rotation, const float sides, const float offset, const float scale) const {
		const auto distance = wall.getDistance() + offset;
		if(distance + wall.getHeight() < SCALE_HEX_LENGTH) return; //TOO_CLOSE;
		if(static_cast<float>(wall.getSide()) >= sides) return; //NOT_IN_RANGE
		auto trap = wall.calcPoints(focus, rotation, sides, offset, scale);

		skew(trap);
		_platform.drawPoly(color, trap);
	}

	Point Game::getScreenCenter() const {
		const auto dim = _platform.getScreenDim();
		return {dim.x/2, dim.y/2};
	}

	Point Game::getShadowOffset() const {
		const auto min = getScreenDimMin();
		if (_shadowAuto) {
			return {0, min/180 + min/15 * _skew};
		}

		return {min/60, min/60};
	}

	void Game::skew(std::vector<Point>& skew) const {
		const auto screen = _platform.getScreenDim();
		for (auto& point : skew) {
			point.y = ((point.y / screen.y - 0.5f) * (1.0f - _skew) + 0.5f) * screen.y;
		}
	}

	Font& Game::getFontSmall() const {
		return *_fontSmall;
	}
	
	Font& Game::getFontLarge() const {
		return *_fontLarge;
	}

	float Game::getScreenDimMax() const {
		const auto size = _platform.getScreenDim();
		return std::max(size.x, size.y);
	}

	float Game::getScreenDimMin() const {
		const auto size = _platform.getScreenDim();
		return std::min(size.x, size.y);
	}

	void Game::playMusic(const std::string& music, const Location location, const bool loadMetadata, const bool loop) {
		const auto base = "/bgm" + music;

		if (loadMetadata) {
			_bgmMetadata = std::make_unique<Metadata>(_platform.openFile(base + ".txt", location));
		}
		
		_bgm = _platform.loadMusic(base, location);

		if (_bgm) {
			_bgm->setLoop(loop);
			_bgm->play();
		}
	}

	void Game::playEffect(SoundEffect effect) const {
		for (auto& soundEffect : _soundEffects) {
			if (effect == soundEffect.first && soundEffect.second) {
				soundEffect.second->play();
			}
		}
	}
}
