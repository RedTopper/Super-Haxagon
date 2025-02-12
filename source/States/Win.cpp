#include "States/Win.hpp"

#include "Core/Game.hpp"
#include "Core/Metadata.hpp"
#include "Driver/Font.hpp"
#include "Driver/Music.hpp"
#include "Driver/Platform.hpp"
#include "Factories/LevelFactory.hpp"
#include "Objects/Level.hpp"
#include "States/Over.hpp"
#include "States/Quit.hpp"

namespace SuperHaxagon {
	static constexpr int LEVEL_VOID = 6;
	static constexpr int LEVEL_HARD = 0;
	static constexpr float CREDITS_TIMER = 60.0 * 3.0;
	
	Win::Win(Game& game, std::unique_ptr<Level> level, LevelFactory& selected, const float score, std::string text) :
		_game(game),
		_platform(game.getPlatform()),
		_selected(selected),
		_level(std::move(level)),
		_score(score),
		_text(std::move(text)) {

		// First make sure that all of the levels exist
		for (auto i = LEVEL_HARD; i <= LEVEL_VOID; i++) {
			if (_game.getLevels()[i] == nullptr) return;
		}

		const auto sides = 6;
		std::vector<Wall> walls;
		walls.reserve(sides);
		for (auto i = 0; i < sides; i++) walls.emplace_back(0.0f, 16.0f, i);
		_surround = std::make_unique<Pattern>(walls, sides);

		// Create our game over level
		_level->getPatterns().emplace_back(*_surround);
		_level->setWinMultiplierWalls(-0.5);
		_level->setWinSides(sides);
		_level->setWinShowCursor(false);
		_level->setWinRotationToZero();

		_credits = {
			{},
			{"CREDITS", "", ""},
			{"T.CAVANAGH", "ORIGINAL", "GAME AND IDEA"},
			{"AJ WALTER", "REDTOPPER", "CREATOR"},

			{"ASSETS", "", ""},
			{"V. ROMEO", "OPEN HEXAGON", "SFX"},
			{"k. MACLEOD", "BGM", "TITLE AND VOID"},
			{"BOSSFIGHT", "BGM", "LEVELS"},
			{"AND", "DUNDERPATRULLEN", "BGM LEVELS"},
			{"AARONAMAR", "FONT", "AT FONTSTRUCT"},

			{"LIBRARIES", "", ""},
			{"DEVKITPRO", "RUNTIME", "PORTLIBS"},
			{"SMEA", "3DS CTRULIB", "LIBRARIES"},
			{"S.BREW", "SWITCHBREW", "LIBRARIES"},
			{"SFML", "DESKTOP", "LIBRARIES"},
			{"FINCS", "CITRO2D", "CITRO3D"},
			{"STEVEICE", "", "BUILDTOOLS"},
			{"S. BARRETT", "STB", "OGG DECODE"},
			{"VOGTINATOR", "NDLESS", "TI-NSPIRE"},

			{"AND", "", ""},
			{"YOU", "THANKS FOR", "PLAYING!"},
			{}
		};
	}

	void Win::enter() {
		_game.playMusic("/esiannoyamFoEzam", Location::ROM, true, false);
		_game.setShadowAuto(true);
	}
	
	std::unique_ptr<State> Win::update(const float dilation) {
		if (!_level) {
			_platform.message(Dbg::FATAL, "win", "not all levels exist");
			return std::make_unique<Quit>(_game);
		}

		if (!_game.getBGMMetadata()) {
			_platform.message(Dbg::FATAL, "win", "metadata failed to load");
			return std::make_unique<Quit>(_game);
		}

		const auto pressed = _platform.getPressed();
		if (pressed.quit) return std::make_unique<Quit>(_game);

		// Get effect data
		auto& metadata = *_game.getBGMMetadata();
		const auto& bgm = _game.getMusic();
		const auto time = bgm ? bgm->getTime() : 0.0f;
		if ((bgm && bgm->isDone()) || pressed.back) {
			_level->setWinShowCursor(true);
			if (bgm) bgm->pause();
			return std::make_unique<Over>(_game, std::move(_level), _selected, _score, _text);
		}

		// Check for level transition labels
		for (auto i = LEVEL_HARD; i <= LEVEL_VOID; i++) {
			if (!metadata.getMetadata(time, "L" + std::to_string(i))) continue;

			const auto& factory = _game.getLevels()[i].get();
			_level->setWinFactory(factory);
			_level->setWinMultiplierRot(1.0);
			_level->setWinMultiplierWalls(factory->getSpeedWall() / 2.0f * -1.0f);
			_level->setWinAutoPatternCreate(i != LEVEL_VOID);
			_level->setWinFrame(0);
			_level->resetColors();
			if (i == LEVEL_VOID) {
				_level->setWinRotationToZero();
				_level->setWinMultiplierWalls(-0.5);
				_level->setWinSides(6);
			} else {
				_level->spin();
			}
		}

		// Apply effects. More can be added here if needed.
		if (metadata.getMetadata(time, "HYPER")) {
			_level->setWinFrame(60.0 * 60.0);
			_level->spin();
		}

		if (metadata.getMetadata(time, "PSURROUND")) _level->getPatterns().emplace_front(*_surround);
		if (metadata.getMetadata(time, "BL")) _level->pulse(1.0);
		if (metadata.getMetadata(time, "BS")) _level->pulse(0.5);
		if (metadata.getMetadata(time, "I")) _level->invertBG();
		if (metadata.getMetadata(time, "C")) {
			_index++;
			_timer = CREDITS_TIMER;
			if (_index >= _credits.size()) _index = _credits.size() - 1;
		}

		_timer -= dilation;

		const auto maxRenderDistance = SCALE_BASE_DISTANCE * (_game.getScreenDimMax() / 400);
		auto& level = *_level;
		level.update(_game.getTwister(), maxRenderDistance, 0, dilation);
		
		return nullptr;
	}

	void Win::drawTop(const float scale) {
		_level->draw(_game, scale, 0);
	}

	void Win::drawBot(const float scale) {
		auto& large = _game.getFontLarge();
		auto& small = _game.getFontSmall();
		large.setScale(scale);
		small.setScale(scale);

		const auto padText = 3 * scale;
		const auto margin = 20 * scale;
		const auto width = _platform.getScreenDim().x;
		const auto height = _platform.getScreenDim().y;
		const auto heightSmall = small.getHeight();

		const Point posCredits = { width / 2, margin };
		const Point posB = { width / 2, height - margin - heightSmall };
		const Point posA = { width / 2, posB.y - heightSmall - padText };

		if (_timer > 0.0f) {
			large.draw(COLOR_WHITE, posCredits, Alignment::CENTER, _credits[_index].name);
			small.draw(COLOR_WHITE, posA, Alignment::CENTER, _credits[_index].line1);
			small.draw(COLOR_WHITE, posB, Alignment::CENTER, _credits[_index].line2);
		}
	}
}
