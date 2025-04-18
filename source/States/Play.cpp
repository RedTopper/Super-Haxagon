// SPDX-FileCopyrightText: 2020 AJ Walter
// SPDX-License-Identifier: GPL-3.0

#include "States/Play.hpp"

#include "Core/Game.hpp"
#include "Core/Metadata.hpp"
#include "Driver/Font.hpp"
#include "Driver/Music.hpp"
#include "Factories/LevelFactory.hpp"
#include "Objects/Level.hpp"
#include "States/Over.hpp"
#include "States/Quit.hpp"
#include "States/Transition.hpp"
#include "States/Win.hpp"

namespace SuperHaxagon {

	Play::Play(Game& game, LevelFactory& factory, LevelFactory& selected, const float startScore, const float rotation, const float cursorPos) :
		_game(game),
		_platform(game.getPlatform()),
		_factory(factory),
		_selected(selected),
		_level(factory.instantiate(game.getRandom(), BASE_DISTANCE, rotation, cursorPos)),
		_score(startScore)
	{}

	Play::~Play() = default;

	void Play::enter() {
		auto* bgm = _game.getMusic();
		if (bgm && !_factory.isCreditsLevel()) bgm->play();

		const auto at = Vec3f{0.0f, 0.0f, 0.0f};
		const auto pos = calculateTilt(at, SKEW_MIN, 4.0f);

		auto& cam = _game.getCam();
		cam.setMovement(CameraLayer::LOOK_AT, at, 60.0f);
		cam.setMovement(CameraLayer::MAIN, pos, 60.0f);
	}

	void Play::exit() {
		auto* bgm = _game.getMusic();
		if (bgm) bgm->pause();
	}

	std::unique_ptr<State> Play::update(const float dilation) {
		// It's technically possible that the BGM metadata was not set
		auto& cam = _game.getCam();
		if (_game.getBGMMetadata()) {

			// Get effect data
			auto& metadata = *_game.getBGMMetadata();
			const auto* bgm = _game.getMusic();
			const auto time = bgm ? bgm->getTime() : 0.0f;

			// Apply effects. More can be added here if needed.
			if (metadata.getMetadata(time, "S")) _level->spin();
			if (metadata.getMetadata(time, "I")) _level->invertBG();

			if (metadata.getMetadata(time, "BL")) {
				cam.setMovement(CameraLayer::SCALE, Vec3f{MAX_PULSE_DISTANCE, MAX_PULSE_DISTANCE, 0.0f}, FRAMES_PER_PULSE_LEAD_UP);
				cam.queueMovement(CameraLayer::SCALE, Vec3f{0.0f, 0.0f, 0.0f}, FRAMES_PER_PULSE_LEAD_OUT);
			}

			if (metadata.getMetadata(time, "BS")) {
				cam.setMovement(CameraLayer::SCALE, Vec3f{MAX_PULSE_DISTANCE * 0.66f, MAX_PULSE_DISTANCE * 0.66f, 0.0f}, FRAMES_PER_PULSE_LEAD_UP);
				cam.queueMovement(CameraLayer::SCALE, Vec3f{0.0f, 0.0f, 0.0f}, FRAMES_PER_PULSE_LEAD_OUT);
			}

			// Tilt Left
			if (metadata.getMetadata(time, "TL")) {
				cam.setMovement(CameraLayer::TILTS, Vec3f{-1.75f, 0.0f, 0.0f}, 15.0f);
				cam.queueMovement(CameraLayer::TILTS, Vec3f{-1.25f, 0.0f, 0.0f}, 90.0f);
				cam.queueMovement(CameraLayer::TILTS, Vec3f{0.0f, 0.0f, 0.0f}, 30.0f);
			}

			// Tilt Right
			if (metadata.getMetadata(time, "TR")) {
				cam.setMovement(CameraLayer::TILTS, Vec3f{1.75f, 0.0f, 0.0f}, 15.0f);
				cam.queueMovement(CameraLayer::TILTS, Vec3f{1.25f, 0.0f, 0.0f}, 90.0f);
				cam.queueMovement(CameraLayer::TILTS, Vec3f{0.0f, 0.0f, 0.0f}, 30.0f);
			}

			// Big Zoom
			if (metadata.getMetadata(time, "Z")) {
				cam.setMovement(CameraLayer::ZOOMS, Vec3f{0.0f, 0.0f, -1.5f}, 20.0f);
				cam.queueMovement(CameraLayer::ZOOMS, Vec3f{0.0f, 0.0f, 4.0f}, 40.0f);
				cam.queueMovement(CameraLayer::ZOOMS,Vec3f{0.0f, 0.0f, 3.5f}, 10.0f);
				cam.queueMovement(CameraLayer::ZOOMS, Vec3f{0.0f, 0.0f, -0.5f}, 30.0f);
				cam.queueMovement(CameraLayer::ZOOMS, Vec3f{0.0f, 0.0f, 0.0f}, 10.0f);
			}
		}

		// Camera
		if (!cam.isMoving(CameraLayer::MAIN)) {
			_skewing = !_skewing;
			const auto skewFrameMax = std::max(
					SKEW_MIN_FRAMES,
					static_cast<float>(_level->getLevelFactory().getSpeedPulse()) * 2.5f
			);

			const auto anchor = Vec3f{0.0f};
			const auto rotation = _skewing ? SKEW_MAX : SKEW_MIN;
			cam.setMovementRotation(CameraLayer::MAIN, anchor, rotation, 4.0f, skewFrameMax);
		}

		// Update level
		const auto previousScore = _level->getScore();
		_level->update(_game.getRandom(), dilation);

		// Button presses, rotate this frame to last.
		_lastPressed = _currentlyPressed;
		_currentlyPressed = _platform.getPressed();
		const auto pressed = _currentlyPressed;

		// Check collision
		const auto cursorDistance = HEX_LENGTH + PLAYER_PADDING_BETWEEN_HEX + PLAYER_TRI_HEIGHT;
		const auto hit = _level->collision(cursorDistance, dilation);

		// Keys
		if(pressed.back || hit == Movement::DEAD) {
			if (&_factory != &_selected &&
			    _factory.getCreator() == "REDHAT" && 
			    (_factory.getName() == "VOID" || _factory.getName() == "NULL") &&
			    _factory.getDifficulty() == "???" &&
			    (_factory.getMode() == "???" || _factory.getMode() == "ORIGINAL")) {
				// Play the super special win animation if you are on the last level without selecting it
				// Congrats, you just won the game!
				_game.playEffect(SoundEffect::AWESOME);
				return std::make_unique<Win>(_game, std::move(_level), _selected, _score, "WONDERFUL");
			}

			if (_factory.isCreditsLevel()) {
				// Cheater
				return std::make_unique<Win>(_game, std::move(_level), _selected, 0.0f, "CHEATER");
			}

			return std::make_unique<Over>(_game, std::move(_level), _selected, _score, "GAME OVER");
		}

		if (pressed.quit) {
			return std::make_unique<Quit>(_game);
		}

		// Go to the next level
		const auto next = _factory.getNextIndex();
		if (next >= 0 && 
		    static_cast<size_t>(next) < _game.getLevels().size() &&
		    _level->getScore() > 60.0f * _level->getLevelFactory().getNextTime()) {
			return std::make_unique<Transition>(_game, std::move(_level), _selected, _score);
		}

		// Keys are being released. If we are still holding the opposite key, resume moving in that direction.
		if (_lastPressed.left && !pressed.left) _moving = pressed.right ? Moving::RIGHT : Moving::STATIONARY;
		if (_lastPressed.right && !pressed.right) _moving = pressed.left ? Moving::LEFT : Moving::STATIONARY;

		// Keys are being pressed, set movement to that direction, even if the other key is still held,
		// so the most recent keypress takes priority.
		// (While rare, pressing both keys down at EXACTLY the same time will favor Moving::RIGHT)
		if (!_lastPressed.left && pressed.left)  _moving = Moving::LEFT;
		if (!_lastPressed.right && pressed.right) _moving = Moving::RIGHT;

		// Process movement
		if (_moving == Moving::LEFT && hit != Movement::CANNOT_MOVE_LEFT) {
			_level->left(dilation);
		} else if (_moving == Moving::RIGHT && hit != Movement::CANNOT_MOVE_RIGHT) {
			_level->right(dilation);
		}

		// Make sure the cursor doesn't extend too far
		_level->clamp();

		// Update score if not delaying
		if (!_level->isDelaying()) {
			_score += dilation;
		}

		const auto* lastScoreText = getScoreText(static_cast<int>(previousScore), false);
		if (lastScoreText != getScoreText(static_cast<int>(_level->getScore()), false)) {
			auto effect = getEffect(static_cast<int>(_level->getScore()));
			_level->increaseRotationMultiplier();
			_game.playEffect(effect);

			if (effect == SoundEffect::SHAPE_HEXAGON) {
				_level->queueDifficultyIncrease();
			}
		}

		return nullptr;
	}

	void Play::drawGame(SurfaceGame& surface, SurfaceGame* shadows) {
		const float scale = _game.getCam().currentPos(CameraLayer::SCALE).x;
		_level->draw(surface, shadows, 0, scale);
	}

	void Play::drawBotUI(SurfaceUI& surface) {
		const auto scale = surface.getScale();
		if (!_game.getFontSmall()) return;
		auto& small = *_game.getFontSmall();

		// Makes it so the score text doesn't freak out
		// if getWidth returns slightly different values
		// each frame
		if (_scalePrev != scale) {
			_scalePrev = scale;
			small.setScale(scale);
			_scoreWidth = small.getWidth("TIME: " + getTime(0));
		}

		const auto pad = 3 * scale;

		small.setScale(scale);

		// Draw the top left POINT/LINE thing
		// Note, 400 is kind of arbitrary. Perhaps it's needed to update this later.
		const auto* levelUp = getScoreText(static_cast<int>(_level->getScore()), surface.getScreenDim().x <= 400);
		const Vec2f levelUpPosText = {pad, pad};
		const Vec2f levelUpBkgSize = {
			small.getWidth(levelUp) + pad * 2,
			small.getHeight() + pad * 2
		};

		surface.drawPolyUITopLeft(COLOR_TRANSPARENT, levelUpBkgSize);
		small.draw(COLOR_WHITE, levelUpPosText, Alignment::LEFT, levelUp);

		// Draw the current score
		const auto screenWidth = surface.getScreenDim().x;
		const auto textScore = "TIME: " + getTime(_score);
		const Vec2f scorePosText = {screenWidth - pad - _scoreWidth, pad};
		Vec2f scoreBkgSize = {_scoreWidth + pad * 2, small.getHeight() + pad * 2};

		// Before we draw the score, compute the best time graph.
		auto drawBar = false;
		auto drawHigh = false;
		const auto originalY = scoreBkgSize.y;
		const auto heightBar = 2 * scale;
		const auto highScore = static_cast<float>(_selected.getHighScore());

		// Adjust background size to accommodate for new record or bar
		const auto* const recordText = "NEW RECORD!";
		if (highScore > 0) {
			if (_score < highScore) {
				scoreBkgSize.y += heightBar + pad;
				drawBar = true;
			} else {
				// Makes sure that if the "New Record" text is longer than the
				// score then we increase its width
				const auto recordTextWidth = small.getWidth(recordText);
				scoreBkgSize.y += small.getHeight() + pad;
				scoreBkgSize.x += recordTextWidth > _scoreWidth ? recordTextWidth - _scoreWidth : 0;
				drawHigh = true;
			}
		}

		surface.drawPolyUITopRight(COLOR_TRANSPARENT, scoreBkgSize);
		small.draw(COLOR_WHITE, scorePosText, Alignment::LEFT, textScore);

		if (drawBar) {
			const Vec2f barPos = {scorePosText.x, originalY};
			const Vec2f barWidth = {_scoreWidth, heightBar};
			const Vec2f barWidthScore = {_scoreWidth * (_score / highScore), heightBar};
			surface.drawRectUI(COLOR_BLACK, barPos, barWidth);
			surface.drawRectUI(COLOR_WHITE, barPos, barWidthScore);
		}

		if (drawHigh) {
			auto textColor = COLOR_WHITE;
			const Vec2f posBest = {screenWidth - pad, originalY};
			if (_score - highScore <= PULSE_TIMES * PULSE_TIME) {
				const auto percent = getPulse(_score, PULSE_TIME, highScore);
				textColor = interpolateColor(PULSE_LOW, PULSE_HIGH, percent);
			}

			small.draw(textColor, posBest, Alignment::RIGHT, recordText);
		}
	}
}
