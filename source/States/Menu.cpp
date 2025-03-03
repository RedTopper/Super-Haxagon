#include "States/Menu.hpp"

#include "Core/Game.hpp"
#include "Core/Metadata.hpp"
#include "Driver/Font.hpp"
#include "Driver/Platform.hpp"
#include "Factories/LevelFactory.hpp"
#include "States/Play.hpp"
#include "States/Title.hpp"
#include "States/Quit.hpp"

#include <algorithm>

namespace SuperHaxagon {
	Menu::Menu(Game& game, LevelFactory& selected, const GameColors& starting, int rotationDirection, float rotation) :
		_game(game),
		_platform(game.getPlatform()) {

		const float side = TAU / 6.0f;
		const float percentRotated = std::fmod(rotation, side) / side;

		_transitionDirection = rotationDirection;
		_frameRotation = percentRotated * FRAMES_PER_TRANSITION;

		const auto& levels = _game.getLevels();
		_selected = std::find_if(levels.begin(), levels.end(), [&selected](const std::unique_ptr<LevelFactory>& pointer) {
			return pointer.get() == &selected;
		});
		
		for (auto i = COLOR_LOCATION_FIRST; i != COLOR_LOCATION_LAST; i++) {
			auto at = static_cast<LocColor>(i);
			_color[at] = starting[at];
			_colorNext[at] = starting[at];
		}
	}

	Menu::~Menu() = default;

	void Menu::enter() {
		_game.playMusic("/bleepingDemo", Location::ROM, false);

		const auto curPos = _game.getCam().currentPos(CameraLayer::MAIN);

		auto& cam = _game.getCam();
		cam.stopAllEffects();
		cam.setMovement(CameraLayer::LOOK_AT, Vec3f{0.0f, 0.1f, 0.0f}, 60.0f);
		cam.setMovement(CameraLayer::MAIN, curPos, 15.0f);
		cam.queueMovement(CameraLayer::MAIN, Vec3f{0.0f, -0.6f + 1.0f/3.0f, 1.5f}, 45.0f);
	}

	std::unique_ptr<State> Menu::update(const float dilation) {
		const auto press = _platform.getPressed();

		if (press.quit) return std::make_unique<Quit>(_game);

		// Prevent transitioning from the main menu from immediately
		// selecting the first level.
		if (!(press.select || press.back)) _justEntered = false;

		if (press.back && !_justEntered) return std::make_unique<Title>(_game);

		if (_transitionDirection == 0) {
			if (press.select && !_justEntered) {
				auto& level = **_selected;
				_game.playMusic(level.getMusic(), level.getLocation(), true);
				_game.playEffect(SoundEffect::BEGIN);
				return std::make_unique<Play>(_game, level, level, 0.0f, 0.0f, 0.0f);
			}

			// Tell the IDE to shut up about an unreachable condition (it's reachable)
			_transitionDirection = 0;

			if (press.right) {
				_transitionDirection = -1;
				++_selected;
				_cameraRotation += TAU / static_cast<float>(_game.getLevels().size());
				if (_selected == _game.getLevels().end()) _selected = _game.getLevels().begin();
			} else if (press.left) {
				_transitionDirection = 1;
				if (_selected == _game.getLevels().begin()) _selected = _game.getLevels().end();
				--_selected;
				_cameraRotation -= TAU / static_cast<float>(_game.getLevels().size());
			}

			if (_transitionDirection != 0) {
				_game.playEffect(SoundEffect::SELECT);

				const auto pos = Vec3f{std::sin(_cameraRotation)/2.0f, -0.6f + std::cos(_cameraRotation)/3.0f, 1.5f};

				_game.getCam().setMovement(CameraLayer::MAIN, pos, FRAMES_PER_TRANSITION * 2.5f);

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

		surface.calculateMatrix(rotation);

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
			shadows->copyMatrix(surface);
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
		if (!_game.getFontLarge() || !_game.getFontSmall()) return;
		auto& large = *_game.getFontLarge();
		auto& small = *_game.getFontSmall();

		// Padding for text
		const auto scale = surface.getScale();
		const auto pad = 3 * scale;
		const auto screen = surface.getScreenDim();

		// Actual text
		auto& level = **_selected;
		const auto scoreTime = "BEST: " + getTime(static_cast<float>(level.getHighScore()));
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

		// Text background for information at top left of screen
		Vec2f infoSize = {std::max({
			large.getWidth(level.getName()),
			small.getWidth(diff),
			small.getWidth(mode),
			small.getWidth(auth),
		}) + pad * 2, posCreator.y + pad + small.getHeight()};
		surface.drawPolyUITopLeft(COLOR_TRANSPARENT, infoSize);

		// Score block with triangle
		Vec2f timeSize = {small.getWidth(scoreTime) + pad * 2, small.getHeight() + pad * 2};
		surface.drawPolyUIBottomLeft(COLOR_TRANSPARENT, timeSize);

		large.draw(COLOR_WHITE, posTitle, Alignment::LEFT, level.getName());
		small.draw(COLOR_GREY, posDifficulty, Alignment::LEFT, diff);
		small.draw(COLOR_GREY, posMode, Alignment::LEFT, mode);
		if (renderCreator) small.draw(COLOR_GREY, posCreator, Alignment::LEFT, auth);
		small.draw(COLOR_WHITE, posTime, Alignment::LEFT, scoreTime);
	}

	void Menu::drawBotUI(SurfaceUI&) {}
}
