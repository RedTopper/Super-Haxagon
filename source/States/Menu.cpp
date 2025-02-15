#include "States/Menu.hpp"

#include "Core/Configuration.hpp"
#include "Core/Game.hpp"
#include "Core/Metadata.hpp"
#include "Driver/Font.hpp"
#include "Driver/Platform.hpp"
#include "Factories/LevelFactory.hpp"
#include "States/Play.hpp"
#include "States/Quit.hpp"

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
		_game.playMusic("/werq", Location::ROM, false);
		_game.playEffect(SoundEffect::HEXAGON);
	}

	std::unique_ptr<State> Menu::update(const float dilation) {
		const auto press = _platform.getPressed();

		if (press.quit) return std::make_unique<Quit>(_game);

		if (_transitionDirection == 0) {
			if (press.select) {
				auto& level = **_selected;
				_game.playMusic(level.getMusic(), level.getLocation(), true);
				return std::make_unique<Play>(_game, level, level, 0.0f);
			}

			if (press.right) {
				_transitionDirection = -1;
				++_selected;
				if (_selected == _game.getLevels().end()) _selected = _game.getLevels().begin();
			} else if (press.left) {
				_transitionDirection = 1;
				if (_selected == _game.getLevels().begin()) _selected = _game.getLevels().end();
				--_selected;
			}

			if (_transitionDirection != 0) {
				_game.playEffect(SoundEffect::SELECT);
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
		if (_frameRotation >= FRAMES_PER_TRANSITION) {
			_frameRotation = 0;
			_transitionDirection = 0;
		}

		// Next background color logic
		if (!_transitionDirection && _frameBackgroundColor >= FRAMES_PER_COLOR) {
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

	void Menu::drawGame(SurfaceGame& surface, SurfaceGame* shadows) {
		const auto percentRotated = _frameRotation / FRAMES_PER_TRANSITION;
		auto rotation = percentRotated * TAU / 6.0f;

		// If the user is going to the left, flip the radians so the animation plays backwards.
		if (_transitionDirection == -1) {
			rotation *= -1.0f;
		}

		surface.calculateMatrix(-rotation);
		//surface.setTranslate({0, -1.0f / 3.0f});
		//surface.setPitch(15 * TAU / 16);
		//surface.setZoom(SCALE_MENU);

		// Colors
		Color fg{};
		Color bg1{};
		Color bg2{};
		Color bg3{};
		if (_transitionDirection) {
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

		// Home screen always has 6 sides.
		// Use a multiplier of 1.5 because the view is shifted down
		// Note: Draw cursor TAU/4 = Up, no rotation
		surface.drawBackground(bg1, bg2, 6.0);

		if (shadows) {
			shadows->calculateMatrix(-rotation);
			shadows->setDepth(-0.025f);
			shadows->drawRegular(COLOR_SHADOW, HEX_LENGTH, 6.0);
			shadows->drawCursor(COLOR_SHADOW, HEX_LENGTH + PLAYER_PADDING_BETWEEN_HEX, 3.0f * TAU / 4.0f - rotation);
		}

		// Geometry
		surface.drawRegular(fg, HEX_LENGTH, 6.0);
		surface.drawRegular(bg3, HEX_LENGTH - HEX_BORDER, 6.0);
		surface.drawCursor(fg, HEX_LENGTH + PLAYER_PADDING_BETWEEN_HEX, 3.0f * TAU / 4.0f - rotation);

		// To help debugging
		// surface.drawDebugTriangles();
	}

	void Menu::drawTopUI(SurfaceUI& surface) {

		auto& large = _game.getFontLarge();
		auto& small = _game.getFontSmall();

		// Padding for text
		const auto scale = surface.getScale();
		const auto pad = 3 * scale;
		const auto screen = surface.getScreenDim();

		// Actual text
		auto& level = **_selected;
		const auto scoreTime = "BEST: " + getTime(static_cast<float>(level.getHighScore()));
		const auto version = VERSION;
		const auto diff = "DIFF: " + level.getDifficulty();
		const auto mode = "MODE: " + level.getMode();
		const auto auth = "AUTH: " + level.getCreator();
		auto renderCreator = level.getCreator() != "REDHAT";
		large.setScale(scale);
		small.setScale(scale);

		// Text positions
		const Vec2f posTitle = {pad, pad};
		const Vec2f posDifficulty = {pad, posTitle.y + pad + large.getHeight()};
		const Vec2f posMode = {pad, posDifficulty.y + pad + small.getHeight()};
		const Vec2f posCreator = {pad, posMode.y + (renderCreator ? pad + small.getHeight() : 0)};
		const Vec2f posTime = {pad, screen.y - small.getHeight() - pad};
		const Vec2f posVersion = {screen.x - pad, screen.y - small.getHeight() - pad};

		// Text background for information at top left of screen
		Vec2f infoSize = {std::max({
			large.getWidth(level.getName()),
			small.getWidth(diff),
			small.getWidth(mode),
			small.getWidth(auth),
		}) + pad * 2, posCreator.y + pad + small.getHeight()};

		// Clockwise, from Top Left
		std::vector<Vec2f> info{
			{0, 0},
			{infoSize.x + infoSize.y / 2, 0},
			{infoSize.x, infoSize.y},
			{0, infoSize.y}
		};

		surface.drawPolyUI(COLOR_TRANSPARENT, info);

		// Score block with triangle
		Vec2f timeSize = {small.getWidth(scoreTime) + pad * 2, small.getHeight() + pad * 2};

		// Clockwise, from Top Left
		const auto screenHeight = surface.getScreenDim().y;
		std::vector<Vec2f> time = {
			{0, screenHeight - timeSize.y},
			{timeSize.x,  screenHeight - timeSize.y},
			{timeSize.x + timeSize.y / 2, screenHeight},
			{0,  screenHeight},
		};

		surface.drawPolyUI(COLOR_TRANSPARENT, time);

		Vec2f versionSize = {small.getWidth(version) + pad * 2, small.getHeight() + pad * 2};

		const auto screenWidth = surface.getScreenDim().x;
		std::vector<Vec2f> versionPoly = {
			{screenWidth - versionSize.x, screenHeight - versionSize.y},
			{screenWidth, screenHeight - versionSize.y},
			{screenWidth, screenHeight},
			{screenWidth - versionSize.x - versionSize.y / 2, screenHeight},
		};

		surface.drawPolyUI(COLOR_TRANSPARENT, versionPoly);

		large.draw(COLOR_WHITE, posTitle, Alignment::LEFT, level.getName());
		small.draw(COLOR_GREY, posDifficulty, Alignment::LEFT, diff);
		small.draw(COLOR_GREY, posMode, Alignment::LEFT, mode);
		if (renderCreator) small.draw(COLOR_GREY, posCreator, Alignment::LEFT, auth);
		small.draw(COLOR_WHITE, posTime, Alignment::LEFT, scoreTime);
		small.draw(COLOR_WHITE, posVersion, Alignment::RIGHT, version);
	}

	void Menu::drawBotUI(SurfaceUI&) {}
}
