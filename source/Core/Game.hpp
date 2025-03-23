#ifndef SUPER_HAXAGON_GAME_HPP
#define SUPER_HAXAGON_GAME_HPP

// SPDX-FileCopyrightText: 2020 AJ Walter
// SPDX-License-Identifier: GPL-3.0-or-later

#include "Core/SurfaceGame.hpp"
#include "Core/SurfaceUI.hpp"

#include <memory>
#include <vector>
#include <string>

namespace SuperHaxagon {
	// Maybe I went a bit overboard with PImpl...
	class LevelFactory;
	class State;
	class Platform;
	class Random;
	class Font;
	class Metadata;
	class Sound;
	class Music;
	class Screen;
	enum class Location;
	enum class SoundEffect;

	class Game {
	public:
		explicit Game(Platform& platform);
		Game(const Game&) = delete;
		~Game();

		void run();
		void playTitleMusic();
		void playMusic(const std::string& music, Location location, bool loadMetadata, bool loop = true);
		void playEffect(SoundEffect effect) const;
		void addLevel(std::unique_ptr<LevelFactory> level);

		const std::vector<std::unique_ptr<LevelFactory>>& getLevels() const {return _levels;}
		Music* getMusic() const {return _bgm.get();}
		Metadata* getBGMMetadata() const {return _bgmMetadata.get();}
		Platform& getPlatform() const {return _platform;}
		Random& getRandom() {return _twister;}
		Font* getFontSmall() const {return _fontSmall.get();}
		Font* getFontLarge() const {return _fontLarge.get();}

		Camera& getCam() {return _camera;}

		void setRunning(const bool running) {_running = running;}

	private:
		Platform& _platform;
		Screen& _screen;
		Random _twister;
		Camera _camera;

		SurfaceGame _surfaceGame;
		SurfaceUI _surfaceUI;

		// Some platforms do not support shadows,
		// so optionally initialize this one
		std::unique_ptr<SurfaceGame> _surfaceShadows;

		std::unique_ptr<Font> _fontSmall;
		std::unique_ptr<Font> _fontLarge;
		std::unique_ptr<Music> _bgm;
		std::vector<std::pair<SoundEffect, std::unique_ptr<Sound>>> _soundEffects;
		std::vector<std::unique_ptr<LevelFactory>> _levels;

		std::unique_ptr<State> _state;

		std::unique_ptr<Metadata> _bgmMetadata;

		bool _running = true;
	};
}

#endif //SUPER_HAXAGON_GAME_HPP
