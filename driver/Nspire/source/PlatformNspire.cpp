// SPDX-FileCopyrightText: 2021 AJ Walter
// SPDX-License-Identifier: GPL-3.0-or-later OR MIT

#include "Driver/Platform.hpp"

#include "timer.h"
#include "MemoryFS.hpp"
#include "Driver/Tools/Random.hpp"
#include "Core/Metadata.hpp"
#include "Driver/Font.hpp"
#include "Driver/Music.hpp"
#include "Driver/Screen.hpp"
#include "Driver/Sound.hpp"

#include <libndls.h>
#include <ngc.h>

#include <ctime>
#include <iostream>

namespace SuperHaxagon {
	std::unique_ptr<Font> createFont(Gc& gc, int size);
	std::unique_ptr<Music> createMusic(float max, float* timeSinceLastGet);
	Screen createScreen(Gc& gc);

	struct Platform::PlatformImpl {
		PlatformImpl() : gc(gui_gc_global_GC()), screen(createScreen(gc)){
			timer_init(0);
			timer_load(0, TIMER_1S);
		}

		bool loop() {
			const auto timer = timer_read(0);
			const auto elapsed = static_cast<float>(TIMER_1S - timer) / TIMER_1S;

			timer_load(0, TIMER_1S);

			// The platform will keep increasing bgmDelta until the game runs Music::loop(),
			// at which point _plat->bgmDelta will be added to the music's internal timer and reset to zero
			bgmDelta += elapsed;
			delta = elapsed;

			return true;
		}

		Gc gc;
		Screen screen;
		float bgmDelta = 0.0f;
		float delta = 0.0f;
	};

	Platform::Platform() : _impl(std::make_unique<PlatformImpl>()) {}

	Platform::~Platform() = default;

	bool Platform::loop() {
		return _impl->loop();
	}

	float Platform::getDilation() const {
		return _impl->delta / (1.0f / 60.0f);
	}

	std::string Platform::getPath(const std::string& partial, const Location location) const {
		switch (location) {
		case Location::ROM:
			return partial;
		case Location::USER:
			return std::string(get_documents_dir()) + "ndless" + partial + std::string(".tns");
		}

		return "";
	}

	std::unique_ptr<std::istream> Platform::openFile(const std::string& partial, const Location location) const {
		if (location == Location::USER) {
			return std::make_unique<std::ifstream>(getPath(partial, location), std::ios::in | std::ios::binary);
		}

		return MemoryFS::openFile(partial);
	}

	std::unique_ptr<Font> Platform::loadFont(const int size) const {
		return createFont(_impl->gc, size);
	}

	// nspire doesn't support sound effects
	std::unique_ptr<Sound> Platform::loadSound(const std::string&) const {
		return nullptr;
	}

	std::unique_ptr<Music> Platform::loadMusic(const std::string& base, const Location location) const {
		Metadata metadata(openFile(base + ".txt", location));
		return createMusic(metadata.getMaxTime(), &_impl->bgmDelta);
	}

	Screen& Platform::getScreen() {
		return _impl->screen;
	}

	std::vector<std::pair<Location, std::string>> Platform::loadUserLevels() {
		std::vector<std::pair<Location, std::string>> levels;
		auto file = openFile("/levels.haxagon", Location::USER);
		if (file->good()) {
			levels.emplace_back(Location::USER, "/levels.haxagon");
		}

		return levels;
	}

	std::string Platform::getButtonName(ButtonName buttonName) {
		switch (buttonName) {
			case ButtonName::BACK: return "DEL | ESC";
			case ButtonName::SELECT: return "CTRL | ENTER";
			case ButtonName::LEFT: return "SHIFT | 7,4,1";
			case ButtonName::RIGHT: return "VAR | 9,6,3";
			case ButtonName::QUIT: return "MENU | HOME";
			default: return "?";
		}
	}

	Buttons Platform::getPressed() const {
		Buttons buttons{};
		buttons.select = isKeyPressed(KEY_NSPIRE_ENTER) > 0 || isKeyPressed(KEY_NSPIRE_CTRL) > 0;
		buttons.back = isKeyPressed(KEY_NSPIRE_ESC) > 0 || isKeyPressed(KEY_NSPIRE_DEL) > 0;
		buttons.quit = isKeyPressed(KEY_NSPIRE_HOME) > 0 || isKeyPressed(KEY_NSPIRE_MENU) > 0;
		buttons.left = isKeyPressed(KEY_NSPIRE_7) > 0 || isKeyPressed(KEY_NSPIRE_4)  > 0 || isKeyPressed(KEY_NSPIRE_1)  > 0  || isKeyPressed(KEY_NSPIRE_SHIFT)  > 0;
		buttons.right = isKeyPressed(KEY_NSPIRE_9) > 0 || isKeyPressed(KEY_NSPIRE_6)  > 0 || isKeyPressed(KEY_NSPIRE_3)  > 0  || isKeyPressed(KEY_NSPIRE_VAR)  > 0;
		return buttons;
	}

	void Platform::shutdown() {
		gui_gc_finish(_impl->gc);
		timer_restore(0);
	}

	void Platform::message(const Dbg dbg, const std::string& where, const std::string& message) const {
		if (dbg == Dbg::INFO) {
			std::cout << "[ndless:info] " + where + ": " + message << std::endl;
		} else if (dbg == Dbg::WARN) {
			std::cout << "[ndless:warn] " + where + ": " + message << std::endl;
		} else if (dbg == Dbg::FATAL) {
			std::cerr << "[ndless:fatal] " + where + ": " + message << std::endl;
		}
	}

	Supports Platform::supports() {
		return Supports::NOTHING;
	}

	Random Platform::getRandom() {
		std::unique_ptr<std::seed_seq> seq;
		seq.reset(new std::seed_seq{time(nullptr)});
		return Random(std::move(seq));
	}
}
