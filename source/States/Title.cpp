// SPDX-FileCopyrightText: 2025 AJ Walter
// SPDX-License-Identifier: GPL-3.0

#include "States/Title.hpp"

#include "Core/Game.hpp"
#include "Core/Metadata.hpp"
#include "Driver/Tools/Configuration.hpp"
#include "Driver/Font.hpp"
#include "Driver/Platform.hpp"
#include "Driver/Music.hpp"
#include "Factories/LevelFactory.hpp"
#include "States/Menu.hpp"
#include "States/Quit.hpp"

namespace SuperHaxagon {
	const float TITLE_ROT_SPEED = TAU/500.0f;
	const float TITLE_FRAMES_PER_COLOR = 90.0f;

	Title::Title(Game& game) :
		_game(game),
		_platform(game.getPlatform()),
		_firstLevel(*_game.getLevels()[0]) {

		for (auto i = COLOR_LOCATION_FIRST; i != COLOR_LOCATION_LAST; i++) {
			_color[static_cast<LocColor>(i)] = COLOR_BLACK;
			_colorNext[static_cast<LocColor>(i)] = _firstLevel.getColors().at(static_cast<LocColor>(i))[0];
		}
	}

	Title::~Title() = default;

	void Title::enter() {
		auto* music = _game.getMusic();
		if (music) music->pause();
		_game.playEffect(SoundEffect::HAXAGON);

		auto& cam = _game.getCam();
		cam.reset();
		cam.setPosition(CameraLayer::LOOK_AT, {0.0f, 0.25f, 0.0f});
		cam.setPosition(CameraLayer::MAIN, {0.0f, -0.30f, 2.0f});
	}

	std::unique_ptr<State> Title::update(const float dilation) {
		const auto press = _platform.getPressed();

		if (!_justEntered && press.back) {
			_justEntered = true;
			_showHelp = !_showHelp;
		}

		if (!press.back) _justEntered = false;
		if (press.quit) return std::make_unique<Quit>(_game);
		if (press.select) {
			return std::make_unique<Menu>(_game, *_game.getLevels()[0], _colorCurrent, 1, _rotation);
		}

		_rotation += dilation * TITLE_ROT_SPEED;

		// Next background color logic
		if (_frameBackgroundColor >= TITLE_FRAMES_PER_COLOR) {
			_frameBackgroundColor = 0;
			for (auto i = COLOR_LOCATION_FIRST; i != COLOR_LOCATION_LAST; i++) {
				const auto location = static_cast<LocColor>(i);
				const auto& availableColors = _firstLevel.getColors().at(location);
				_color[location] = _colorNext[location];
				_colorNextIndex[location] = _colorNextIndex[location] + 1 < availableColors.size() ? _colorNextIndex[location] + 1 : 0;
				_colorNext[location] = availableColors[_colorNextIndex[location]];
			}
		} else {
			_frameBackgroundColor += dilation;
		}

		// Colors
		_colorCurrent = GameColors{
				interpolateColor(_color[LocColor::FG], _colorNext[LocColor::FG], _frameBackgroundColor / TITLE_FRAMES_PER_COLOR),
				interpolateColor(_color[LocColor::BG1], _colorNext[LocColor::BG1], _frameBackgroundColor / TITLE_FRAMES_PER_COLOR),
				interpolateColor(_color[LocColor::BG2], _colorNext[LocColor::BG2], _frameBackgroundColor / TITLE_FRAMES_PER_COLOR),
		};

		return nullptr;
	}

	void Title::drawGame(SurfaceGame& surface, SurfaceGame* shadows) {
		surface.calculateMatrix(_rotation);

		surface.drawBackground(_colorCurrent.bg1, _colorCurrent.bg2, 6.0);

		if (shadows) {
			shadows->copyMatrix(surface);
			shadows->setDepth(-0.025f);
			shadows->drawRegular(COLOR_SHADOW, HEX_LENGTH, 6.0);
		}

		// Geometry
		surface.drawRegular(_colorCurrent.fg, HEX_LENGTH, 6.0);
		surface.drawRegular(_colorCurrent.bg2, HEX_LENGTH - HEX_BORDER, 6.0);
	}

	void Title::drawTopUI(SurfaceUI& surface) {
		if (!_game.getFontLarge() || !_game.getFontSmall()) return;
		auto& large = *_game.getFontLarge();
		auto& small = *_game.getFontSmall();

		//const auto pressSelect = "PRESS " + _game.getPlatform().getButtonName(ButtonName::SELECT);
		//const auto pressControls = _game.getPlatform().getButtonName(ButtonName::BACK) + ": KEYS";

		const auto scale = surface.getScale();
		const auto pad = 3 * scale;
		const auto screen = surface.getScreenDim();

		large.setScale(scale);
		small.setScale(scale);

		if (_showHelp) {
			const auto controls = "CONTROLS";

			std::vector<std::pair<std::string, std::string>> keys{
					{"SELECT:", _game.getPlatform().getButtonName(ButtonName::SELECT)},
					{"BACK:", _game.getPlatform().getButtonName(ButtonName::BACK)},
					{"LEFT:", _game.getPlatform().getButtonName(ButtonName::LEFT)},
					{"RIGHT:", _game.getPlatform().getButtonName(ButtonName::RIGHT)},
					{"QUIT:", _game.getPlatform().getButtonName(ButtonName::QUIT)},
			};

			float maxWidthNames = 0.0f;
			for (const auto& key : keys) {
				auto width = small.getWidth(key.first);
				if (width > maxWidthNames) maxWidthNames = width;
			}

			float maxWidthValues = 0.0f;
			for (const auto& key : keys) {
				auto width = small.getWidth(key.second);
				if (width > maxWidthValues) maxWidthValues = width;
			}

			const float maxWidth = maxWidthNames + maxWidthValues + 2*pad;
			const float height = small.getHeight() + pad;
			const Vec2f size = {maxWidth + 2.0f*pad, static_cast<float>(keys.size()) * height + pad};
			const Vec2f pos = {screen.x/2.0f - size.x/2.0f, screen.y/2.0f - size.y/2.0f};
			const Vec2f posControls = {screen.x / 2.0f, screen.y/2.0f - size.y/2.0f - large.getHeight() - pad};

			surface.drawRectUI(COLOR_TRANSPARENT, pos, size);

			int index = 0;
			for (const auto& key : keys) {
				Vec2f posTextName = {pos.x + pad, pos.y + pad + height*static_cast<float>(index)};
				small.draw(COLOR_WHITE, posTextName, Alignment::LEFT, key.first);
				Vec2f posTextValue = {pos.x + maxWidthNames + pad*2.0f, pos.y + pad + height*static_cast<float>(index)};
				small.draw(COLOR_WHITE, posTextValue, Alignment::LEFT, key.second);
				index++;
			}

			large.draw(COLOR_WHITE, posControls, Alignment::CENTER, controls);
		} else {
			const auto super = "SUPER";
			const auto haxagon = "HAXAGON";
			const auto pressSelect = "KEYS: " + _game.getPlatform().getButtonName(ButtonName::BACK);
			const auto version = VERSION;

			const Vec2f posSuper = {screen.x / 2.0f, screen.y / 2.0f - small.getHeight() - large.getHeight() / 2.0f - pad};
			const Vec2f posHaxagon = {screen.x / 2.0f, screen.y / 2.0f - large.getHeight() / 2.0f + pad};
			const Vec2f posSelect = {pad, screen.y - small.getHeight() - pad};
			const Vec2f posVersion = {screen.x - pad, screen.y - small.getHeight() - pad};

			const Vec2f selectSize = {small.getWidth(pressSelect) + pad * 2, small.getHeight() + pad * 2};
			const Vec2f versionSize = {small.getWidth(version) + pad * 2, small.getHeight() + pad * 2};
			surface.drawPolyUIBottomLeft(COLOR_TRANSPARENT, selectSize);
			surface.drawPolyUIBottomRight(COLOR_TRANSPARENT, versionSize);

			small.draw(COLOR_WHITE, posSuper, Alignment::CENTER, super);
			large.draw(COLOR_WHITE, posHaxagon, Alignment::CENTER, haxagon);
			small.draw(COLOR_WHITE, posSelect, Alignment::LEFT, pressSelect);
			small.draw(COLOR_WHITE, posVersion, Alignment::RIGHT, version);

			if (sizeof(PLATFORM_NAME) > 1) {
				const auto platform = PLATFORM_NAME;
				const Vec2f posPlatform = {screen.x/2.0f, pad};
				//const Vec2f platformSize = {small.getWidth(platform) + pad * 2, small.getHeight() + pad * 2};
				//surface.drawPolyUITopLeft(COLOR_TRANSPARENT, platformSize);
				small.draw(COLOR_WHITE, posPlatform, Alignment::CENTER, platform);
			}
		}
	}

	void Title::drawBotUI(SurfaceUI&) {}
}
