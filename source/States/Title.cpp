#include "States/Title.hpp"

#include "Core/Configuration.hpp"
#include "Core/Game.hpp"
#include "Core/Metadata.hpp"
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
		_game.playEffect(SoundEffect::HEXAGON);
		_game.setCamera(
				{0.0f, -0.30f, 2.0f},
				{0.0f, 0.25f, 0.0f}
		);
	}

	std::unique_ptr<State> Title::update(const float dilation) {
		const auto press = _platform.getPressed();

		if (press.quit) return std::make_unique<Quit>(_game);
		if (press.select) {
			return std::make_unique<Menu>(_game, *_game.getLevels()[0], _colorCurrent);
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
		surface.calculateMatrix(-_rotation, 1.0f);

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

		Buttons select{};
		select.select = true;

		const auto super = "SUPER";
		const auto haxagon = "HAXAGON";
		const auto press = "PRESS " + _game.getPlatform().getButtonName(select);
		const auto version = VERSION;

		const auto scale = surface.getScale();
		const auto pad = 3 * scale;
		const auto screen = surface.getScreenDim();

		large.setScale(scale);
		small.setScale(scale);

		const Vec2f posSuper = {screen.x/2.0f, screen.y/2.0f - small.getHeight() - large.getHeight()/2.0f - pad};
		const Vec2f posHaxagon = {screen.x/2.0f, screen.y/2.0f - large.getHeight()/2.0f + pad};
		const Vec2f posSelect = {pad, screen.y - small.getHeight() - pad};
		const Vec2f posVersion = {screen.x - pad, screen.y - small.getHeight() - pad};

		Vec2f versionSize = {small.getWidth(version) + pad * 2, small.getHeight() + pad * 2};
		Vec2f presSize = {small.getWidth(press) + pad * 2, small.getHeight() + pad * 2};

		std::vector<Vec2f> versionPoly = {
				{screen.x - versionSize.x, screen.y - versionSize.y},
				{screen.x, screen.y - versionSize.y},
				{screen.x, screen.y},
				{screen.x - versionSize.x - versionSize.y / 2, screen.y},
		};

		std::vector<Vec2f> pressPoly = {
				{0, screen.y - presSize.y},
				{presSize.x,  screen.y - presSize.y},
				{presSize.x + presSize.y / 2, screen.y},
				{0,  screen.y},
		};

		surface.drawPolyUI(COLOR_TRANSPARENT, pressPoly);
		surface.drawPolyUI(COLOR_TRANSPARENT, versionPoly);

		small.draw(COLOR_WHITE, posVersion, Alignment::RIGHT, version);
		small.draw(COLOR_WHITE, posSuper, Alignment::CENTER, super);
		small.draw(COLOR_WHITE, posSelect, Alignment::LEFT, press);
		large.draw(COLOR_WHITE, posHaxagon, Alignment::CENTER, haxagon);
	}

	void Title::drawBotUI(SurfaceUI&) {}
}
