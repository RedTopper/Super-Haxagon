#include "States/Transition.hpp"

#include "Core/Game.hpp"
#include "Core/SurfaceUI.hpp"
#include "Driver/Font.hpp"
#include "Driver/Platform.hpp"
#include "Factories/LevelFactory.hpp"
#include "Objects/Level.hpp"
#include "States/Play.hpp"
#include "States/Quit.hpp"

namespace SuperHaxagon {

	Transition::Transition(Game& game, std::unique_ptr<Level> level, LevelFactory& selected, const float score) :
		_game(game),
		_platform(game.getPlatform()),
		_selected(selected),
		_level(std::move(level)),
		_score(score)
	{}

	Transition::~Transition() = default;

	void Transition::enter() {
		_game.playEffect(SoundEffect::WONDERFUL);
	}

	std::unique_ptr<State> Transition::update(const float dilation) {
		_frames += dilation;
		_level->rotate(_level->getLevelFactory().getSpeedRotation(), dilation);
		_level->clamp();

		const auto press = _platform.getPressed();
		if (press.quit) return std::make_unique<Quit>(_game);

		if (_frames <= TRANSITION_FRAMES) {
			_offset *= TRANSITION_ACCELERATION_RATE * dilation + 1.0f;
		}

		if (_frames >= TRANSITION_FRAMES) {
			// In order to get here _game.getLevels()[next] must be valid,
			// as the previous state verified this. If it does not exist, the
			// game just keeps going
			const auto next = _level->getLevelFactory().getNextIndex();
			auto& factory = *_game.getLevels()[next];

			// We want to keep the current music playing if we are going to start
			// the next level with the same music
			if (_selected.getMusic() != factory.getMusic()) {
				_game.playMusic(factory.getMusic(), factory.getLocation(), true);
			}
			
			return std::make_unique<Play>(_game, factory, _selected, _score);
		}

		return nullptr;
	}

	void Transition::drawGame(SurfaceGame& surface, SurfaceGame* shadows) {
		_level->draw(surface, shadows, (_offset - 1.0f) / 100.0f, 0.0f);
	}

	void Transition::drawBotUI(SurfaceUI& surface) {
		const auto scale = surface.getScale();
		auto& large = _game.getFontLarge();
		large.setScale(scale);

		const auto* const text = "WONDERFUL";
		const auto pad = 6 * scale;
		const auto width = large.getWidth(text);
		const auto center = surface.getScreenDim().x / 2;

		const Vec2f posText = {center, pad};
		const Vec2f bkgSize = {width + pad * 2, large.getHeight() + pad * 2};
		const std::vector<Vec2f> trap = {
			{center - bkgSize.x/2 - bkgSize.y/2, 0},
			{center + bkgSize.x/2 + bkgSize.y/2, 0},
			{center + bkgSize.x/2, bkgSize.y},
			{center - bkgSize.x/2, bkgSize.y},
		};

		const auto percent = getPulse(_frames, Play::PULSE_TIME, 0);
		const auto pulse = interpolateColor(PULSE_LOW, PULSE_HIGH, percent);
		surface.drawPolyUI(COLOR_TRANSPARENT, trap);
		large.draw(pulse, posText, Alignment::CENTER, text);
	}
}
