#include <array>

#include "Core/Game.hpp"
#include "Driver/Font.hpp"
#include "Driver/Audio.hpp"
#include "Factories/Level.hpp"
#include "States/Play.hpp"
#include "States/Quit.hpp"
#include "States/Over.hpp"

namespace SuperHaxagon {

	Play::Play(Game& game, LevelFactory& factory, int levelIndex) :
		_game(game),
		_platform(game.getPlatform()),
		_factory(factory),
		_level(factory.instantiate(game.getTwister(), SCALE_BASE_DISTANCE)),
		_levelIndex(levelIndex)
	{}

	Play::~Play() = default;

	void Play::enter() {
		std::string path;
		if (_factory.getLocation() == Location::INTERNAL) {
			path = _platform.getPathRom("/bgm" + _factory.getMusic());
		} else if (_factory.getLocation() == Location::EXTERNAL) {
			path = _platform.getPath("/bgm" + _factory.getMusic());
		}

		_bgm = _platform.loadAudio(path, Stream::INDIRECT);
		_platform.playSFX(_game.getSfxBegin());
		_platform.playBGM(*_bgm);
	}

	void Play::exit() {
		_platform.stopBGM();
	}

	std::unique_ptr<State> Play::update(const double dilation) {
		const auto maxRenderDistance = SCALE_BASE_DISTANCE * (_game.getScreenDimMax() / 240);
		_level->update(_game.getTwister(), SCALE_HEX_LENGTH, maxRenderDistance, dilation);

		// Button presses
		const auto pressed = _platform.getPressed();

		// Check collision
		const auto cursorDistance = SCALE_HEX_LENGTH + SCALE_HUMAN_PADDING + SCALE_HUMAN_HEIGHT;
		const auto hit = _level->collision(cursorDistance, dilation);
		if(pressed.back || hit == Movement::DEAD) {
			return std::make_unique<Over>(_game, _factory, std::move(_level), _score, _levelIndex);
		}

		if (pressed.quit) {
			return std::make_unique<Quit>(_game);
		}

		if (pressed.left && hit != Movement::CANNOT_MOVE_LEFT) {
			_level->left(dilation);
		} else if (pressed.right && hit != Movement::CANNOT_MOVE_RIGHT) {
			_level->right(dilation);
		}

		// Make sure the cursor doesn't extend too far
		_level->clamp();

		// Update score
		const auto* lastScoreText = getScoreText(static_cast<int>(_score), false);
		_score += dilation;

		if(lastScoreText != getScoreText(static_cast<int>(_score), false)) {
			_level->increaseMultiplier();
			_platform.playSFX(_game.getSfxLevelUp());
		}

		return nullptr;
	}

	void Play::drawTop(const double scale) {
		_level->draw(_game, scale, 0, _platform.getBgmVelocity() * SCALE_PULSE_MAX);
	}

	void Play::drawBot(const double scale) {
		auto& small = _game.getFontSmall();

		// Makes it so the score text doesn't freak out
		// if getWidth returns slightly different values
		// each frame
		if (_scalePrev != scale) {
			_scalePrev = scale;
			small.setScale(scale);
			_scoreWidth = small.getWidth("TIME: " + getTime(0));
		}

		const auto width = _platform.getScreenDim().x;
		const auto pad = 3 * scale;

		small.setScale(scale);

		// Draw the top left POINT/LINE thing
		// Note, 400 is kind of arbitrary. Perhaps it's needed to update this later.
		const auto* levelUp = getScoreText(static_cast<int>(_score), _platform.getScreenDim().x <= 400);
		const Point levelUpPosText = {pad, pad};
		const Point levelUpBkgPos = {0, 0};
		const Point levelUpBkgSize = {
			levelUpPosText.x + small.getWidth(levelUp) + pad,
			levelUpPosText.y + small.getHeight() + pad
		};

		const std::array<Point, 3> levelUpBkgTri = {
			Point{levelUpBkgSize.x, levelUpBkgSize.y},
			Point{levelUpBkgSize.x, 0},
			Point{levelUpBkgSize.x + levelUpBkgSize.y/2, 0}
		};

		_platform.drawRect(COLOR_TRANSPARENT, levelUpBkgPos, levelUpBkgSize);
		_platform.drawTriangle(COLOR_TRANSPARENT, levelUpBkgTri);
		small.draw(COLOR_WHITE, levelUpPosText, Alignment::LEFT, levelUp);

		// Draw the current score
		const auto textScore = "TIME: " + getTime(_score);
		const Point scorePosText = { width - pad - _scoreWidth, pad};
		const Point scoreBkgPos = {scorePosText.x - pad, 0};
		Point scoreBkgSize = {
			width - scoreBkgPos.x,
			scorePosText.y + small.getHeight() + pad
		};

		// Before we draw the score, compute the best time graph.
		auto drawBar = false;
		auto drawHigh = false;
		const auto originalY = scoreBkgSize.y;
		const auto heightBar = 2 * scale;
		if (_factory.getHighScore() > 0) {
			if (_score < _factory.getHighScore()) {
				scoreBkgSize.y += heightBar + pad;
				drawBar = true;
			} else {
				scoreBkgSize.y += small.getHeight() + pad;
				drawHigh = true;
			}
		}

		const std::array<Point, 3> scoreBkgTri = {
			Point{scoreBkgPos.x, scoreBkgSize.y},
			Point{scoreBkgPos.x, 0},
			Point{scoreBkgPos.x - scoreBkgSize.y/2, 0}
		};

		_platform.drawTriangle(COLOR_TRANSPARENT, scoreBkgTri);
		_platform.drawRect(COLOR_TRANSPARENT, scoreBkgPos, scoreBkgSize);
		small.draw(COLOR_WHITE, scorePosText, Alignment::LEFT, textScore);

		if (drawBar) {
			const Point barPos = {scorePosText.x, originalY};
			const Point barWidth = {_scoreWidth, heightBar};
			const Point barWidthScore = {_scoreWidth * (_score / _factory.getHighScore()), heightBar};
			_platform.drawRect(COLOR_BLACK, barPos, barWidth);
			_platform.drawRect(COLOR_WHITE, barPos, barWidthScore);
		}

		if (drawHigh) {
			auto textColor = COLOR_WHITE;
			const Point posBest = {width - pad, originalY};
			if (_score - _factory.getHighScore() <= PULSE_TIMES * PULSE_TIME) {
				const auto percent = getPulse(_score, PULSE_TIME, _factory.getHighScore());
				textColor = interpolateColor(PULSE_LOW, PULSE_HIGH, percent);
			}

			small.draw(textColor, posBest, Alignment::RIGHT,  "NEW RECORD!");
		}
	}
}
