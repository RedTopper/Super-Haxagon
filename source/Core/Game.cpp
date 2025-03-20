// SPDX-FileCopyrightText: 2020 AJ Walter
// SPDX-License-Identifier: GPL-3.0-or-later

#include "Game.hpp"

#include "Metadata.hpp"
#include "Driver/Font.hpp"
#include "Driver/Music.hpp"
#include "Driver/Platform.hpp"
#include "Driver/Screen.hpp"
#include "Driver/Sound.hpp"
#include "Factories/LevelFactory.hpp"
#include "States/Load.hpp"

#include <cmath>

namespace SuperHaxagon {

	Game::Game(Platform& platform) :
		_platform(platform),
		_screen(platform.getScreen()),
		_twister(_platform.getTwister()),
		_camera(),
		_surfaceGame(_screen, _camera),
		_surfaceUI(_screen) {
		// Audio loading
		const std::vector<std::pair<SoundEffect, std::string>> sounds{
			{SoundEffect::BEGIN, "/sound/begin"},
			{SoundEffect::GO, "/sound/go"},
			{SoundEffect::HAXAGON, "/sound/haxagon"},
			{SoundEffect::OVER, "/sound/over"},
			{SoundEffect::SELECT, "/sound/select"},
			{SoundEffect::LEVEL_UP, "/sound/level"},
			{SoundEffect::AWESOME, "/sound/awesome"},
			{SoundEffect::WONDERFUL, "/sound/wonderful"},
			{SoundEffect::SHAPE_LINE, "/sound/shapes/line"},
			{SoundEffect::SHAPE_TRIANGLE, "/sound/shapes/triangle"},
			{SoundEffect::SHAPE_SQUARE, "/sound/shapes/square"},
			{SoundEffect::SHAPE_PENTAGON, "/sound/shapes/pentagon"},
			{SoundEffect::SHAPE_HEXAGON, "/sound/shapes/hexagon"},
			{SoundEffect::SHAPE_HEPTAGON, "/sound/shapes/heptagon"},
			{SoundEffect::SHAPE_OCTAGON, "/sound/shapes/octagon"},
		};

		for (const auto& sound : sounds) {
			_soundEffects.emplace_back(sound.first, platform.loadSound(sound.second));
		}

		_fontSmall = platform.loadFont(16);
		_fontLarge = platform.loadFont(32);
		_twister = platform.getTwister();

		if (static_cast<int>(Platform::supports() & Supports::SHADOWS)) {
			_surfaceShadows = std::make_unique<SurfaceGame>(_screen, _camera);
		}
	}

	Game::~Game() {
		// Stop and unload music
		if (_bgm) _bgm->pause();
		_bgm = nullptr;
		_platform.message(Dbg::INFO, "game", "shutdown ok");
	}

	void Game::run() {
		_state = std::make_unique<Load>(*this);
		_state->enter();
		while(_running && _platform.loop()) {
			auto dilation = _platform.getDilation();

			// Throttle how wildly off dilation can be on all platforms.
			// A dilation of 1.0 is 1/60th of a second. Huge dilation spikes can happen
			// when the process is suspended (for example, the 3ds on the home menu)
			dilation = dilation > 5.0f ? 5.0f : (dilation < 0.05f ? 0.05f : dilation);

			// For platforms that need it, tick the BGM.
			if (_bgm) _bgm->update();

			auto next = _state->update(dilation);
			if (!_running) break;
			while (next) {
				_state->exit();
				_state = std::move(next);
				_state->enter();
				next = _state->update(dilation);
			}

			_camera.update(dilation);
			_screen.screenBegin();
			_state->drawGame(_surfaceGame, _surfaceShadows.get());
			_state->drawTopUI(_surfaceUI);
			_screen.screenSwitch();
			_state->drawBotUI(_surfaceUI);
			_screen.screenFinalize();
		}
	}

	void Game::playTitleMusic() {
		// First, try to load from the SD card if the player wants something custom
		_bgm = _platform.loadMusic("/title", Location::USER);

		if (!_bgm) _bgm = _platform.loadMusic("/bgm/bleepingDemo", Location::ROM);

		if (_bgm) {
			_bgm->setLoop(true);
			_bgm->play();
		}
	}

	void Game::playMusic(const std::string& music, const Location location, const bool loadMetadata, const bool loop) {
		const auto base = "/bgm" + music;

		if (loadMetadata) {
			_bgmMetadata = std::make_unique<Metadata>(_platform.openFile(base + ".txt", location));
		}
		
		_bgm = _platform.loadMusic(base, location);

		if (_bgm) {
			_bgm->setLoop(loop);
			_bgm->play();
		}
	}

	void Game::playEffect(SoundEffect effect) const {
		for (auto& soundEffect : _soundEffects) {
			if (effect == soundEffect.first && soundEffect.second) {
				soundEffect.second->play();
			}
		}
	}

	void Game::addLevel(std::unique_ptr<LevelFactory> level) {
		_levels.emplace_back(std::move(level));
	}
}
