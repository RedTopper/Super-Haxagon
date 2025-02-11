#include "States/Menu.hpp"

#include "Core/Configuration.hpp"
#include "Core/Game.hpp"
#include "Core/Metadata.hpp"
#include "Driver/Font.hpp"
#include "Driver/Platform.hpp"
#include "Factories/LevelFactory.hpp"
#include "States/Play.hpp"
#include "States/Quit.hpp"

#include <array>
#include <algorithm>

namespace SuperHaxagon {
	Menu::Menu(Game& game, LevelFactory& selected) :
		_game(game),
		_platform(game.getPlatform()) {

		const auto& levels = _game.getLevels();
		_selected = std::find_if(levels.begin(), levels.end(), [&selected](const std::unique_ptr<LevelFactory>& pointer) {
			return pointer.get() == &selected;
		});
		
		for (auto i = COLOR_LOCATION_FIRST; i != COLOR_LOCATION_LAST; i++) {
			_color[static_cast<LocColor>(i)] = COLOR_BLACK;
			_colorNext[static_cast<LocColor>(i)] = COLOR_BLACK;
		}
	}

	Menu::~Menu() = default;

	void Menu::enter() {
		_game.setSkew(0.0);
		_game.setShadowAuto(false);
		_game.loadBGMAudio("/werq", Location::ROM, false);
		_platform.playSFX(SoundEffect::HEXAGON);
	}

	std::unique_ptr<State> Menu::update(const float dilation) {
		const auto press = _platform.getPressed();

		if (press.quit) return std::make_unique<Quit>(_game);

		if (!_transitionDirection) {
			if (press.select) {
				auto& level = **_selected;
				_game.loadBGMAudio(level.getMusic(), level.getLocation(), true);
				return std::make_unique<Play>(_game, level, level, 0.0f);
			}

			if (press.right) {
				_transitionDirection = 1;
				++_selected;
				if (_selected == _game.getLevels().end()) _selected = _game.getLevels().begin();
			} else if (press.left) {
				_transitionDirection = -1;
				if (_selected == _game.getLevels().begin()) _selected = _game.getLevels().end();
				--_selected;
			}

			if (_transitionDirection) {
				_platform.playSFX(SoundEffect::SELECT);
				for (auto i = COLOR_LOCATION_FIRST; i != COLOR_LOCATION_LAST; i++) {
					const auto location = static_cast<LocColor>(i);

					// Save the current frames color into the current color
					_color[location] = interpolateColor(_color[location], _colorNext[location], _frameBackgroundColor / FRAMES_PER_COLOR);
				}
			}
		}

		// The user probably just started transitioning to the next or previous
		// level from the last block, or the screen is still transitioning.
		// We'll hold the background color frame at its max until it's done.
		if (_transitionDirection) {
			for (auto i = COLOR_LOCATION_FIRST; i != COLOR_LOCATION_LAST; i++) {
				const auto location = static_cast<LocColor>(i);
				// Set the next color to be the first one of the level we are going to
				_colorNextIndex[location] = 0;
				_colorNext[location] = (*_selected)->getColors().at(location)[0];
			}

			_frameBackgroundColor = FRAMES_PER_COLOR;
			_frameRotation += dilation;
		}

		// End transition
		if(_frameRotation >= FRAMES_PER_TRANSITION) {
			_frameRotation = 0;
			_transitionDirection = 0;
		}

		// Next background color logic
		if(!_transitionDirection && _frameBackgroundColor >= FRAMES_PER_COLOR) {
			_frameBackgroundColor = 0;
			for (auto i = COLOR_LOCATION_FIRST; i != COLOR_LOCATION_LAST; i++) {
				const auto location = static_cast<LocColor>(i);
				const auto& availableColors = (*_selected)->getColors().at(location);
				_color[location] = _colorNext[location];
				_colorNextIndex[location] = _colorNextIndex[location] + 1 < availableColors.size() ? _colorNextIndex[location] + 1 : 0;
				_colorNext[location] = availableColors[_colorNextIndex[location]];
			}
		} else {
			_frameBackgroundColor += dilation;
		}

		return nullptr;
	}

	void Menu::drawTop(float scale) {
		auto percentRotated = _frameRotation / FRAMES_PER_TRANSITION;
		auto rotation = percentRotated * TAU/6.0f;

		// If the user is going to the left, flip the radians so the animation plays backwards.
		if(_transitionDirection == -1) {
			rotation *= -1.0f;
		}

		// Colors
		Color fg{};
		Color bg1{};
		Color bg2{};
		Color bg3{};
		if(_transitionDirection) {
			fg = interpolateColor(_color[LocColor::FG], _colorNext[LocColor::FG], percentRotated);
			bg1 = interpolateColor(_color[LocColor::BG1], _colorNext[LocColor::BG2], percentRotated); // Note: BG1 to BG2
			bg2 = interpolateColor(_color[LocColor::BG2], _colorNext[LocColor::BG1], percentRotated); // Note: BG2 to BG1
			bg3 = interpolateColor(_color[LocColor::BG2], _colorNext[LocColor::BG2], percentRotated); // Note: BG2 transition
		} else {
			fg = interpolateColor(_color[LocColor::FG], _colorNext[LocColor::FG], _frameBackgroundColor / FRAMES_PER_COLOR);
			bg1 = interpolateColor(_color[LocColor::BG1], _colorNext[LocColor::BG1], _frameBackgroundColor / FRAMES_PER_COLOR);
			bg2 = interpolateColor(_color[LocColor::BG2], _colorNext[LocColor::BG2], _frameBackgroundColor / FRAMES_PER_COLOR);
			bg3 = bg2;
		}

		auto screen = _platform.getScreenDim();
		auto shadow = _game.getShadowOffset();

		Point focus = {screen.x/2, screen.y/6 * 5};
		Point offsetFocus = {focus.x + shadow.x, focus.y + shadow.y};

		// Home screen always has 6 sides.
		// Use a multiplier of 1.5 because the view is shifted down
		// Note: Draw cursor TAU/4 = Up, no rotation
		_game.drawBackground(bg1, bg2, focus, 1.5, rotation, 6.0);

		
		// Shadows, if supported
		if (static_cast<int>(_platform.supports() & Supports::SHADOWS)) {
			_game.drawRegular(COLOR_SHADOW, offsetFocus, SCALE_HEX_LENGTH * SCALE_MENU * scale, rotation, 6.0);
			_game.drawCursor(COLOR_SHADOW, offsetFocus, TAU / 4.0f, 0, SCALE_HEX_LENGTH + SCALE_HUMAN_PADDING + 4, scale * SCALE_MENU * 0.75f);
		}

		// Geometry
		_game.drawRegular(fg, focus,SCALE_HEX_LENGTH * SCALE_MENU * scale, rotation, 6.0);
		_game.drawRegular(bg3, focus, (SCALE_HEX_LENGTH - SCALE_HEX_BORDER / 2) * SCALE_MENU * scale, rotation, 6.0);
		_game.drawCursor(fg, focus, TAU / 4.0f, 0, SCALE_HEX_LENGTH + SCALE_HUMAN_PADDING + 4, scale * SCALE_MENU * 0.75f);

		auto& large = _game.getFontLarge();
		auto& small = _game.getFontSmall();

		// Padding for text
		auto pad = 3 * scale;

		// Actual text
		auto& level = **_selected;
		auto scoreTime = "BEST: " + getTime(static_cast<float>(level.getHighScore()));
		auto version = VERSION;
		auto diff = "DIFF: " + level.getDifficulty();
		auto mode = "MODE: " + level.getMode();
		auto auth = "AUTH: " + level.getCreator();
		auto renderCreator = level.getCreator() != "REDHAT";
		large.setScale(scale);
		small.setScale(scale);

		// Text positions
		const Point posTitle = {pad, pad};
		const Point posDifficulty = {pad, posTitle.y + pad + large.getHeight()};
		const Point posMode = {pad, posDifficulty.y + pad + small.getHeight()};
		const Point posCreator = {pad, posMode.y + (renderCreator ? pad + small.getHeight() : 0)};
		const Point posTime = {pad, screen.y - small.getHeight() - pad};
		const Point posVersion = {screen.x - pad, screen.y - small.getHeight() - pad};

		// Text background for information at top left of screen
		Point infoSize = {std::max({
			large.getWidth(level.getName()),
			small.getWidth(diff),
			small.getWidth(mode),
			small.getWidth(auth),
		}) + pad * 2, posCreator.y + pad + small.getHeight()};

		// Clockwise, from Top Left
		std::vector<Point> info{
			{0, 0},
			{infoSize.x + infoSize.y / 2, 0},
			{infoSize.x, infoSize.y},
			{0, infoSize.y}
		};

		_platform.drawPoly(COLOR_TRANSPARENT, info);

		// Score block with triangle
		Point timeSize = {small.getWidth(scoreTime) + pad * 2, small.getHeight() + pad * 2};

		// Clockwise, from Top Left
		const auto screenHeight = _platform.getScreenDim().y;
		std::vector<Point> time = {
			{0, screenHeight - timeSize.y},
			{timeSize.x,  screenHeight - timeSize.y},
			{timeSize.x + timeSize.y / 2, screenHeight},
			{0,  screenHeight},
		};

		_platform.drawPoly(COLOR_TRANSPARENT, time);

		Point versionSize = {small.getWidth(version) + pad * 2, small.getHeight() + pad * 2};

		const auto screenWidth = _platform.getScreenDim().x;
		std::vector<Point> versionPoly = {
			{screenWidth - versionSize.x, screenHeight - versionSize.y},
			{screenWidth, screenHeight - versionSize.y},
			{screenWidth, screenHeight},
			{screenWidth - versionSize.x - versionSize.y / 2, screenHeight},
		};

		_platform.drawPoly(COLOR_TRANSPARENT, versionPoly);

		large.draw(COLOR_WHITE, posTitle, Alignment::LEFT, level.getName());
		small.draw(COLOR_GREY, posDifficulty, Alignment::LEFT, diff);
		small.draw(COLOR_GREY, posMode, Alignment::LEFT, mode);
		if (renderCreator) small.draw(COLOR_GREY, posCreator, Alignment::LEFT, auth);
		small.draw(COLOR_WHITE, posTime, Alignment::LEFT, scoreTime);
		small.draw(COLOR_WHITE, posVersion, Alignment::RIGHT, version);
	}

	void Menu::drawBot(float) {}
}
