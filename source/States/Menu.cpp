#include "States/Menu.hpp"

#include "Core/Game.hpp"
#include "Core/Metadata.hpp"
#include "Driver/Font.hpp"
#include "Driver/Platform.hpp"
#include "Factories/Level.hpp"
#include "States/Play.hpp"
#include "States/Quit.hpp"

#include <array>
#include <algorithm>

namespace SuperHaxagon {
	Menu::Menu(Game& game, const int levelIndex) :
		_game(game),
		_platform(game.getPlatform()),
		_level(levelIndex) {
		for (auto i = COLOR_LOCATION_FIRST; i != COLOR_LOCATION_LAST; i++) {
			_color[static_cast<LocColor>(i)] = COLOR_BLACK;
			_colorNext[static_cast<LocColor>(i)] = COLOR_BLACK;
		}
	}

	Menu::~Menu() = default;

	void Menu::enter() {
		_game.setSkew(0.0);
		_game.setShadowAuto(false);
		_game.setBGMAudio(_platform.loadAudio(_platform.getPathRom("/bgm/pamgaea"), SuperHaxagon::Stream::INDIRECT));
		_platform.playSFX(_game.getSFXHexagon());
		_platform.playBGM(*_game.getBGMAudio());
	}

	std::unique_ptr<State> Menu::update(const double dilation) {
		const auto press = _platform.getPressed();

		if (press.quit) return std::make_unique<Quit>(_game);

		if (!_transitionDirection) {
			if (press.select) {
				auto& levelFactory = *_game.getLevels()[_level];
				_game.loadBGMAudio(levelFactory);
				return std::make_unique<Play>(_game, levelFactory, _level, 0.0);
			}

			if (press.right) {
				_transitionDirection = 1;
				_level++;
			} else if (press.left) {
				_transitionDirection = -1;
				_level--;
			}

			if (_transitionDirection) {
				_platform.playSFX(_game.getSFXSelect());
				for (auto i = COLOR_LOCATION_FIRST; i != COLOR_LOCATION_LAST; i++) {
					const auto location = static_cast<LocColor>(i);

					// Save the current frames color into the current color
					_color[location] = interpolateColor(_color[location], _colorNext[location], _frameBackgroundColor / FRAMES_PER_COLOR);
				}
			}
		}

		// Wrap levels within bounds
		if(_level >=  static_cast<int>(_game.getLevels().size())) _level = 0;
		if(_level < 0) _level = static_cast<int>(_game.getLevels().size()) - 1;

		// The user probably just started transitioning to the next or previous
		// level from the last block, or the screen is still transitioning.
		// We'll hold the background color frame at its max until it's done.
		if (_transitionDirection) {
			auto& levelCur = *_game.getLevels()[_level];
			for (auto i = COLOR_LOCATION_FIRST; i != COLOR_LOCATION_LAST; i++) {
				const auto location = static_cast<LocColor>(i);
				// Set the next color to be the first one of the level we are going to
				_colorNextIndex[location] = 0;
				_colorNext[location] = levelCur.getColors().at(location)[0];
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
			auto& levelCur = *_game.getLevels()[_level];
			for (auto i = COLOR_LOCATION_FIRST; i != COLOR_LOCATION_LAST; i++) {
				const auto location = static_cast<LocColor>(i);
				const auto& availableColors = levelCur.getColors().at(location);
				_color[location] = _colorNext[location];
				_colorNextIndex[location] = _colorNextIndex[location] + 1 < availableColors.size() ? _colorNextIndex[location] + 1 : 0;
				_colorNext[location] = availableColors[_colorNextIndex[location]];
			}
		} else {
			_frameBackgroundColor += dilation;
		}

		return nullptr;
	}

	void Menu::drawTop(double scale) {
		auto percentRotated = _frameRotation / FRAMES_PER_TRANSITION;
		auto rotation = percentRotated * TAU/6.0;

		// If the user is going to the left, flip the radians so the animation plays backwards.
		if(_transitionDirection == -1) {
			rotation *= -1.0;
		}

		// Colors
		Color fg{};
		Color bg1{};
		Color bg2{};
		Color bg3{};
		auto& levelCur = *_game.getLevels()[_level];
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
		// Use a multiplier of 2 because the view is shifted down
		// Note: Draw cursor TAU/4 = Up, no rotation
		_game.drawBackground(bg1, bg2, focus, 2, rotation, 6.0);

		// Shadows
		_game.drawRegular(COLOR_SHADOW, offsetFocus, SCALE_HEX_LENGTH * SCALE_MENU * scale, rotation, 6.0);
		_game.drawCursor(COLOR_SHADOW, offsetFocus, TAU / 4.0, 0, SCALE_HEX_LENGTH + SCALE_HUMAN_PADDING + 4, scale * SCALE_MENU * 0.75);

		// Geometry
		_game.drawRegular(fg, focus,SCALE_HEX_LENGTH * SCALE_MENU * scale, rotation, 6.0);
		_game.drawRegular(bg3, focus, (SCALE_HEX_LENGTH - SCALE_HEX_BORDER / 2) * SCALE_MENU * scale, rotation, 6.0);
		_game.drawCursor(fg, focus, TAU / 4.0, 0, SCALE_HEX_LENGTH + SCALE_HUMAN_PADDING + 4, scale * SCALE_MENU * 0.75);

		auto& large = _game.getFontLarge();
		auto& small = _game.getFontSmall();

		// Padding for text
		auto pad = 3 * scale;

		// Actual text
		auto scoreTime = "BEST: " + getTime(levelCur.getHighScore());
		auto diff = "DIFF: " + levelCur.getDifficulty();
		auto mode = "MODE: " + levelCur.getMode();
		auto auth = "AUTH: " + levelCur.getCreator();
		auto renderCreator = levelCur.getLocation() == LocLevel::EXTERNAL;
		large.setScale(scale);
		small.setScale(scale);

		// Text positions
		Point posTitle = {pad, pad};
		Point posDifficulty = {pad, posTitle.y + large.getHeight() + pad};
		Point posMode = {pad, posDifficulty.y + small.getHeight() + pad};
		Point posCreator = {pad, posMode.y + (renderCreator ? small.getHeight() + pad : 0)};
		Point posTime = {pad, screen.y - small.getHeight() - pad};

		// Triangle positions
		Point infoPos = {0, 0};
		Point infoSize = {std::max({
			large.getWidth(levelCur.getName()),
			small.getWidth(diff),
			small.getWidth(mode),
			small.getWidth(auth),
		}) + pad, posCreator.y + small.getHeight() + pad};

		// It looks like the original game had triangles flipped....
		std::array<Point, 3> infoTriangle = {
			Point{infoSize.x, infoSize.y},
			Point{infoSize.x, 0},
			Point{infoSize.x + infoSize.y/2, 0}
		};

		_platform.drawRect(COLOR_TRANSPARENT, infoPos, infoSize);
		_platform.drawTriangle(COLOR_TRANSPARENT, infoTriangle);

		// Score block with triangle
		Point timePos = {0, posTime.y - pad};
		Point timeSize = {small.getWidth(scoreTime) + pad * 2, small.getHeight() + pad * 2};
		std::array<Point, 3> timeTriangle = {
			Point{timeSize.x,  _platform.getScreenDim().y - timeSize.y},
			Point{timeSize.x,  _platform.getScreenDim().y},
			Point{timeSize.x + timeSize.y/2,  _platform.getScreenDim().y}
		};

		_platform.drawRect(COLOR_TRANSPARENT, timePos, timeSize);
		_platform.drawTriangle(COLOR_TRANSPARENT, timeTriangle);

		large.draw(COLOR_WHITE, posTitle, Alignment::LEFT, levelCur.getName());
		small.draw(COLOR_GREY, posDifficulty, Alignment::LEFT, diff);
		small.draw(COLOR_GREY, posMode, Alignment::LEFT, mode);

		if (renderCreator)
			small.draw(COLOR_GREY, posCreator, Alignment::LEFT, auth);

		small.draw(COLOR_WHITE, posTime, Alignment::LEFT, scoreTime);
	}

	void Menu::drawBot(double) {}
}
