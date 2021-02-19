#include "States/Play.hpp"

#include "Core/Game.hpp"
#include "Core/Metadata.hpp"
#include "Driver/Font.hpp"
#include "Driver/Platform.hpp"
#include "Driver/Player.hpp"
#include "Factories/Level.hpp"
#include "States/Over.hpp"
#include "States/Quit.hpp"
#include "States/Transition.hpp"
#include "States/Win.hpp"

#include <cmath>

namespace SuperHaxagon {

	Play::Play(Game& game, LevelFactory& factory, LevelFactory& selected, const float startScore) :
		_game(game),
		_platform(game.getPlatform()),
		_factory(factory),
		_selected(selected),
		_level(factory.instantiate(game.getTwister(), SCALE_BASE_DISTANCE)),
		_score(startScore)
	{}

	Play::~Play() = default;

	void Play::enter() {
		auto* bgm = _platform.getBGM();
		if (bgm) bgm->play();
		_platform.playSFX(_game.getSFXBegin());
		_game.setShadowAuto(true);
	}

	void Play::exit() {
		auto* bgm = _platform.getBGM();
		if (bgm) bgm->pause();
	}

	std::unique_ptr<State> Play::update(const float dilation) {
		const auto maxRenderDistance = SCALE_BASE_DISTANCE * (_game.getScreenDimMax() / 400);

		// Render the level with a skewed 3D look
		auto skewFrameMax = static_cast<float>(_level->getLevelFactory().getSpeedPulse()) * 2.5f;
		skewFrameMax = skewFrameMax < SKEW_MIN_FRAMES ? SKEW_MIN_FRAMES : skewFrameMax;
		_skewFrame += dilation * _skewDirection * (_level->getLevelFactory().getSpeedRotation() > 0 ? 1.0f : 0);
		_game.setSkew((-cos(_skewFrame / skewFrameMax * PI) + 1.0f) / 2.0f * SKEW_MAX);

		// It's technically possible that the BGM metadata was not set
		if (_game.getBGMMetadata()) {

			// Get effect data
			auto& metadata = *_game.getBGMMetadata();
			const auto* bgm = _platform.getBGM();
			const auto time = bgm ? bgm->getTime() : 0.0f;

			// Apply effects. More can be added here if needed.
			if (metadata.getMetadata(time, "S")) _level->spin();
			if (metadata.getMetadata(time, "I")) _level->invertBG();
			if (metadata.getMetadata(time, "BL")) _level->pulse(1.0f);
			if (metadata.getMetadata(time, "BS")) _level->pulse(0.5f);
		}

		// Update level
		const auto previousFrame = _level->getFrame();
		_level->update(_game.getTwister(), SCALE_HEX_LENGTH, maxRenderDistance, dilation);

		// Button presses
		const auto pressed = _platform.getPressed();

		// Check collision
		const auto cursorDistance = SCALE_HEX_LENGTH + SCALE_HUMAN_PADDING + SCALE_HUMAN_HEIGHT;
		const auto hit = _level->collision(cursorDistance, dilation);

		// Keys
		if(pressed.back || hit == Movement::DEAD) {
			if (&_factory != &_selected &&
			    _factory.getCreator() == "REDHAT" && 
			    _factory.getName() == "VOID" &&
			    _factory.getDifficulty() == "???" &&
			    _factory.getMode() == "???") {
				// Play the super special win animation if you are on the last level without selecting it
				// Congrats, you just won the game!
				return std::make_unique<Win>(_game, std::move(_level), _selected, _score, "WONDERFUL");
			}

			if (_factory.getCreator() == "REDHAT" &&
			    _factory.getName() == "CREDITS" &&
			    _factory.getDifficulty() == "SPOILERS" &&
			    _factory.getMode() == "(DUH)") {
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
		    _level->getFrame() > 60.0f * _level->getLevelFactory().getNextTime()) {
			return std::make_unique<Transition>(_game, std::move(_level), _selected, _score);
		}

		// Process movement
		if (pressed.left && hit != Movement::CANNOT_MOVE_LEFT) {
			_level->left(dilation);
		} else if (pressed.right && hit != Movement::CANNOT_MOVE_RIGHT) {
			_level->right(dilation);
		}

		// Make sure the cursor doesn't extend too far
		_level->clamp();

		// Update score
		_score += dilation;

		const auto* lastScoreText = getScoreText(static_cast<int>(previousFrame), false);
		if (lastScoreText != getScoreText(static_cast<int>(_level->getFrame()), false)) {
			_level->increaseMultiplier();
			_platform.playSFX(_game.getSFXLevelUp());
		}

		return nullptr;
	}

	void Play::drawTop(const float scale) {
		_level->draw(_game, scale, 0);
	}

	void Play::drawBot(const float scale) {
		auto& small = _game.getFontSmall();

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
		const auto* levelUp = getScoreText(static_cast<int>(_level->getFrame()), _platform.getScreenDim().x <= 400);
		const Point levelUpPosText = {pad, pad};
		const Point levelUpBkgSize = {
			small.getWidth(levelUp) + pad * 2,
			small.getHeight() + pad * 2
		};

		// Clockwise, from top left
		const std::vector<Point> levelUpBkg = {
			{0, 0},
			{levelUpBkgSize.x + levelUpBkgSize.y / 2, 0},
			{levelUpBkgSize.x, levelUpBkgSize.y},
			{0, levelUpBkgSize.y},
		};

		_platform.drawPoly(COLOR_TRANSPARENT, levelUpBkg);
		small.draw(COLOR_WHITE, levelUpPosText, Alignment::LEFT, levelUp);

		// Draw the current score
		const auto screenWidth = _platform.getScreenDim().x;
		const auto textScore = "TIME: " + getTime(_platform.getBGM()->getTime() * 60.0f);
		const Point scorePosText = {screenWidth - pad - _scoreWidth, pad};
		Point scoreBkgSize = {_scoreWidth + pad * 2, small.getHeight() + pad * 2};

		// Before we draw the score, compute the best time graph.
		auto drawBar = false;
		auto drawHigh = false;
		const auto originalY = scoreBkgSize.y;
		const auto heightBar = 2 * scale;
		const auto highScore = static_cast<float>(_selected.getHighScore());

		// Adjust background size to accommodate for new record or bar
		if (highScore > 0) {
			if (_score < highScore) {
				scoreBkgSize.y += heightBar + pad;
				drawBar = true;
			} else {
				scoreBkgSize.y += small.getHeight() + pad;
				drawHigh = true;
			}
		}

		// Clockwise, from top left
		const std::vector<Point> scoreBkg = {
			{screenWidth - scoreBkgSize.x - scoreBkgSize.y / 2, 0},
			{screenWidth, 0},
			{screenWidth, scoreBkgSize.y},
			{screenWidth - scoreBkgSize.x, scoreBkgSize.y}
		};

		_platform.drawPoly(COLOR_TRANSPARENT, scoreBkg);
		small.draw(COLOR_WHITE, scorePosText, Alignment::LEFT, textScore);

		if (drawBar) {
			const Point barPos = {scorePosText.x, originalY};
			const Point barWidth = {_scoreWidth, heightBar};
			const Point barWidthScore = {_scoreWidth * (_score / highScore), heightBar};
			_game.drawRect(COLOR_BLACK, barPos, barWidth);
			_game.drawRect(COLOR_WHITE, barPos, barWidthScore);
		}

		if (drawHigh) {
			auto textColor = COLOR_WHITE;
			const Point posBest = {screenWidth - pad, originalY};
			if (_score - highScore <= PULSE_TIMES * PULSE_TIME) {
				const auto percent = getPulse(_score, PULSE_TIME, highScore);
				textColor = interpolateColor(PULSE_LOW, PULSE_HIGH, percent);
			}

			small.draw(textColor, posBest, Alignment::RIGHT, "NEW RECORD!");
		}
	}
}
