#include "States/Over.hpp"

#include "Core/Game.hpp"
#include "Core/SurfaceUI.hpp"
#include "Driver/Font.hpp"
#include "Driver/Platform.hpp"
#include "Factories/LevelFactory.hpp"
#include "Objects/Level.hpp"
#include "States/Load.hpp"
#include "States/Menu.hpp"
#include "States/Play.hpp"
#include "States/Quit.hpp"

#include <cstring>
#include <ostream>
#include <fstream>

namespace SuperHaxagon {

	Over::Over(Game& game, std::unique_ptr<Level> level, LevelFactory& selected, const float score, std::string text) :
		_game(game),
		_platform(game.getPlatform()),
		_selected(selected),
		_level(std::move(level)),
		_text(std::move(text)),
		_score(score) {
		_high = _selected.setHighScore(static_cast<int>(score));
	}

	Over::~Over() = default;

	void Over::enter() {
		_game.playEffect(_text == "WONDERFUL" ? SoundEffect::WONDERFUL : SoundEffect::OVER);
		auto& cam = _game.getCam();
		cam.stopAllEffects();
		cam.setMovement(CameraLayer::LOOK_AT, Vec3f{0.0f, 0.035f, 0.0f}, 60.0f);
		cam.setMovement(CameraLayer::MAIN, Vec3f{0, -0.3f, 2.0f}, 60.0f);

		std::ofstream scores(_platform.getPath("/scores.db", Location::USER), std::ios::out | std::ios::binary);

		if (!scores) return;

		scores.write(Load::SCORE_HEADER, strlen(Load::SCORE_HEADER));
		auto levels = static_cast<uint32_t>(_game.getLevels().size());
		scores.write(reinterpret_cast<char*>(&levels), sizeof(levels));

		for (const auto& lev : _game.getLevels()) {
			writeString(scores, lev->getName());
			writeString(scores, lev->getDifficulty());
			writeString(scores, lev->getMode());
			writeString(scores, lev->getCreator());
			uint32_t highSc = lev->getHighScore();
			scores.write(reinterpret_cast<char*>(&highSc), sizeof(highSc));
		}

		scores.write(Load::SCORE_FOOTER, strlen(Load::SCORE_FOOTER));
	}

	std::unique_ptr<State> Over::update(const float dilation) {
		_frames += dilation;
		const float speed = static_cast<float>(_level->getRotationDirection()) * GAME_OVER_ROT_SPEED;
		_level->rotate(speed, dilation);
		_level->clamp();

		const auto press = _platform.getPressed();
		if(press.quit) return std::make_unique<Quit>(_game);

		if(_frames <= FRAMES_PER_GAME_OVER) {
			_offset *= GAME_OVER_ACCELERATION_RATE * dilation + 1.0f;
		}

		if(_frames >= FRAMES_PER_GAME_OVER) {
			_level->clearPatterns();
			if (press.select) {
				// If the level we are playing is not the same as the index, we need to load
				// the original music
				if (_selected.getMusic() != _level->getLevelFactory().getMusic()) {
					_game.playMusic(_selected.getMusic(), _selected.getLocation(), true);
				}

				// Use "GO" instead of "BEGIN" (begin is used on the title)
				_game.playEffect(SoundEffect::GO);

				// Go back to the original level
				return std::make_unique<Play>(
					_game,
					_selected,
					_selected,
					0.0f,
					_level->getCurrentRotation(),
					_level->getCursorPos()
				);
			}

			if (press.back) {
				return std::make_unique<Menu>(
					_game,
					_selected,
					_level->getCurrentColor(),
					_level->getRotationDirection(),
					_level->getCurrentRotation()
				);
			}
		}

		return nullptr;
	}

	void Over::drawGame(SurfaceGame& surface, SurfaceGame* shadows) {
		const float scale = _game.getCam().currentPos(CameraLayer::SCALE).x;
		_level->draw(surface, shadows, (_offset - 1.0f) / 100.0f, scale);
	}

	void Over::drawBotUI(SurfaceUI& surface) {
		const auto scale = surface.getScale();
		if (!_game.getFontLarge() || !_game.getFontSmall()) return;
		auto& large = *_game.getFontLarge();
		auto& small = *_game.getFontSmall();
		large.setScale(scale);
		small.setScale(scale);

		const auto padText = 3 * scale;
		const auto margin = 20 * scale;
		const auto width = surface.getScreenDim().x;
		const auto height = surface.getScreenDim().y;
		const auto heightLarge = large.getHeight();
		const auto heightSmall = small.getHeight();

		const Vec2f posGameOver = {width / 2, margin};
		const Vec2f posTime = {width / 2, posGameOver.y + heightLarge + padText};
		const Vec2f posBest = {width / 2, posTime.y + heightSmall + padText};
		const Vec2f posB = {width / 2, height - margin - heightSmall};
		const Vec2f posA = {width / 2, posB.y - heightSmall - padText};

		const auto textScore = std::string("TIME: ") + getTime(_score);
		large.draw(COLOR_WHITE, posGameOver, Alignment::CENTER,  _text);
		small.draw(COLOR_WHITE, posTime, Alignment::CENTER, textScore);

		if(_high) {
			const auto percent = getPulse(_frames, PULSE_TIME, 0);
			const auto pulse = interpolateColor(PULSE_LOW, PULSE_HIGH, percent);
			small.draw(pulse, posBest, Alignment::CENTER, "NEW RECORD!");
		} else {
			const auto score = _selected.getHighScore();
			const auto textBest = std::string("BEST: ") + getTime(static_cast<float>(score));
			small.draw(COLOR_WHITE, posBest, Alignment::CENTER, textBest);
		}

		if(_frames >= FRAMES_PER_GAME_OVER) {
			small.draw(COLOR_WHITE, posA, Alignment::CENTER, "PRESS " + _platform.getButtonName(ButtonName::SELECT) + " TO PLAY");
			small.draw(COLOR_WHITE, posB, Alignment::CENTER, "PRESS " + _platform.getButtonName(ButtonName::BACK) + " TO QUIT");
		}
	}
}
