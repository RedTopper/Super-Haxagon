#include <array>

#include "Core/Game.hpp"
#include "Factories/Level.hpp"
#include "States/Quit.hpp"
#include "States/Play.hpp"
#include "States/Menu.hpp"

namespace SuperHaxagon {
	Menu::Menu(Game& game, const int levelIndex) :
		_game(game),
		_platform(game.getPlatform()),
		_level(levelIndex)
	{}

	Menu::~Menu() = default;

	void Menu::enter() {
		_bgm = _platform.loadAudio(_platform.getPathRom("/bgm/pamgaea"), SuperHaxagon::Stream::INDIRECT);
		_platform.playSFX(_game.getSfxHexagon());
		_platform.playBGM(*_bgm);
	}

	void Menu::exit() {
		_platform.stopBGM();
	}

	std::unique_ptr<State> Menu::update(const double dilation) {
		const auto press = _platform.getPressed();

		if (press.quit) return std::make_unique<Quit>(_game);

		if (!_transitionDirection) {
			if (press.select) return std::make_unique<Play>(_game, *_game.getLevels()[_level], _level);
			if (press.right) {
				_transitionDirection = 1;
				_level++;
				_platform.playSFX(_game.getSfxSelect());
			}
			if (press.left) {
				_transitionDirection = -1;
				_level--;
				_platform.playSFX(_game.getSfxSelect());
			}
		}


		if (_transitionDirection) {
			_fgIndex = 0;
			_bg1Index = 0;
			_bg2Index = 0;
			_fgIndexLast = 0;
			_bg1IndexLast = 0;
			_bg2IndexLast = 0;
			_colorFrame = FRAMES_PER_COLOR;
		}

		if(_level >=  static_cast<int>(_game.getLevels().size())) _level = 0;
		if(_level < 0) _level = static_cast<int>(_game.getLevels().size()) - 1;
		if(_transitionDirection) _transitionFrame += dilation;
		if(_transitionFrame >= FRAMES_PER_TRANSITION) {
			_transitionFrame = 0;
			_transitionDirection = 0;
		}

		if(!_transitionDirection && _colorFrame >= FRAMES_PER_COLOR) {
			_colorFrame = 0;
			_fgIndexLast = _fgIndex++;
			_bg1IndexLast = _bg1Index++;
			_bg2IndexLast = _bg2Index++;
			auto& levelCur = *_game.getLevels()[_level];
			if (_fgIndex >= levelCur.getColorsFG().size()) _fgIndex = 0;
			if (_bg1Index >= levelCur.getColorsBG1().size()) _bg1Index = 0;
			if (_bg2Index >= levelCur.getColorsBG2().size()) _bg2Index = 0;
		} else {
			_colorFrame += dilation;
		}

		return nullptr;
	}

	void Menu::drawTop(double scale) {
		auto percentRotated = _transitionFrame / FRAMES_PER_TRANSITION;
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
			fg = interpolateColor(_fg, levelCur.getColorsFG()[0], percentRotated);
			bg1 = interpolateColor(_bg1, levelCur.getColorsBG2()[0], percentRotated);
			bg2 = interpolateColor(_bg2, levelCur.getColorsBG1()[0], percentRotated);
			bg3 = interpolateColor(_bg2, levelCur.getColorsBG2()[0], percentRotated); //Real BG2 transition
		} else {
			_fg = interpolateColor(levelCur.getColorsFG()[_fgIndexLast], levelCur.getColorsFG()[_fgIndex], _colorFrame / FRAMES_PER_COLOR);
			_bg1 = interpolateColor(levelCur.getColorsBG1()[_bg1IndexLast], levelCur.getColorsBG1()[_bg1Index], _colorFrame / FRAMES_PER_COLOR);
			_bg2 = interpolateColor(levelCur.getColorsBG2()[_bg2IndexLast], levelCur.getColorsBG2()[_bg2Index], _colorFrame / FRAMES_PER_COLOR);
			fg = _fg;
			bg1 = _bg1;
			bg2 = _bg2;
			bg3 = _bg2;
		}

		auto screen = _platform.getScreenDim();
		auto shadow = _game.getShadowOffset();

		Point focus = {screen.x/2, screen.y/6};
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
		auto renderCreator = levelCur.getLocation() == Location::EXTERNAL;
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
			Point{infoSize.x, _platform.getScreenDim().y - infoSize.y},
			Point{infoSize.x, _platform.getScreenDim().y},
			Point{infoSize.x + infoSize.y/2,  _platform.getScreenDim().y}
		};

		_platform.drawRect(COLOR_TRANSPARENT, infoPos, infoSize);
		_platform.drawTriangle(COLOR_TRANSPARENT, infoTriangle);

		// Score block with triangle
		Point timePos = {0, posTime.y - pad};
		Point timeSize = {small.getWidth(scoreTime) + pad * 2, small.getHeight() + pad * 2};
		std::array<Point, 3> timeTriangle = {
			Point{timeSize.x, timeSize.y},
			Point{timeSize.x, 0},
			Point{timeSize.x + timeSize.y/2, 0}
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
