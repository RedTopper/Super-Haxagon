#include "Driver/Nspire/PlatformNspire.hpp"

#include "Core/Twist.hpp"
#include "Driver/Nspire/AudioLoaderNspire.hpp"
#include "Driver/Nspire/FontNspire.hpp"
#include "Driver/Nspire/MemoryFS.hpp"
#include "Driver/Nspire/timer.h"

#include <ctime>
#include <iostream>
#include <libndls.h>

namespace SuperHaxagon {
	PlatformNspire::PlatformNspire(const Dbg dbg) : Platform(dbg) {
		_gc = gui_gc_global_GC();
		gui_gc_begin(_gc);
		gui_gc_setColorRGB(_gc, 0, 0, 0);
		gui_gc_fillRect(_gc, 0, 0, 320, 240);
		gui_gc_finish(_gc);
		gui_gc_blit_to_screen(_gc);

		timer_init(0);
		timer_load(0, TIMER_1S);
	}

	bool PlatformNspire::loop() {
		const auto timer = timer_read(0);
		const auto elapsed = static_cast<float>(TIMER_1S - timer) / TIMER_1S;
		if (_bgm) {
			// Trust me I know what I'm doing with my types, compiler.
			auto* bgm = reinterpret_cast<AudioPlayerNspire*>(_bgm.get());
			bgm->addTime(elapsed);
			if (bgm->isDone()) bgm->play();
		}

		timer_load(0, TIMER_1S);
		_dilation = elapsed * 60.0f;
		
		return true;
	}

	float PlatformNspire::getDilation() {
		return _dilation;
	}

	std::string PlatformNspire::getPath(const std::string& partial, const Location location) {
		switch (location) {
		case Location::ROM:
			return partial;
		case Location::USER:
			return std::string(get_documents_dir()) + "ndless" + partial + std::string(".tns");
		}

		return "";
	}

	std::unique_ptr<std::istream> PlatformNspire::openFile(const std::string& partial, const Location location) {
		if (location == Location::USER) {
			return std::make_unique<std::ifstream>(getPath(partial, location), std::ios::in | std::ios::binary);
		}

		return MemoryFS::openFile(partial);
	}

	std::unique_ptr<AudioLoader> PlatformNspire::loadAudio(const std::string& partial, Stream, const Location location) {
		return std::make_unique<AudioLoaderNspire>(openFile(partial + ".txt", location));
	}

	std::unique_ptr<Font> PlatformNspire::loadFont(const std::string&, int size, const Location) {
		return std::make_unique<FontNspire>(_gc, size);
	}

	void PlatformNspire::playBGM(AudioLoader& audio) {
		_bgm = audio.instantiate();
		if (!_bgm) return;
		_bgm->setLoop(true);
		_bgm->play();
	}

	std::string PlatformNspire::getButtonName(const Buttons& button) {
		if (button.back) return "ESC";
		if (button.select) return "ENTER";
		if (button.left) return "4";
		if (button.right) return "6";
		if (button.quit) return "MENU";
		return "?";
	}

	Buttons PlatformNspire::getPressed() {
		Buttons buttons{};
		buttons.select = isKeyPressed(KEY_NSPIRE_ENTER) > 0;
		buttons.back = isKeyPressed(KEY_NSPIRE_ESC) > 0;
		buttons.quit = isKeyPressed(KEY_NSPIRE_HOME) > 0 || isKeyPressed(KEY_NSPIRE_MENU) > 0;
		buttons.left = isKeyPressed(KEY_NSPIRE_4) > 0;
		buttons.right = isKeyPressed(KEY_NSPIRE_6)  > 0;
		return buttons;
	}

	Point PlatformNspire::getScreenDim() const {
		return {320, 240};
	}

	void PlatformNspire::screenBegin() {
		gui_gc_begin(_gc);
		gui_gc_setColorRGB(_gc, 0, 0, 0);
		gui_gc_fillRect(_gc, 0, 0, 320, 240);
	}

	void PlatformNspire::screenFinalize() {
		gui_gc_blit_to_screen(_gc);
	}

	void PlatformNspire::drawPoly(const Color& color, const std::vector<Point>& points) {
		const auto pos = std::make_unique<Point2D[]>(points.size());
		for (size_t i = 0; i < points.size(); i++) {
			pos[i] = { points[i].x, points[i].y };
		}

		gui_gc_setColorRGB(_gc, color.r, color.g, color.b);
		gui_gc_fillPoly(_gc, reinterpret_cast<unsigned*>(pos.get()), points.size());
	}

	void PlatformNspire::shutdown() {
		gui_gc_finish(_gc);
		timer_restore(0);
	}

	void PlatformNspire::message(const Dbg dbg, const std::string& where, const std::string& message) {
		if (dbg == Dbg::INFO) {
			std::cout << "[ndless:info] " + where + ": " + message << std::endl;
		} else if (dbg == Dbg::WARN) {
			std::cout << "[ndless:warn] " + where + ": " + message << std::endl;
		} else if (dbg == Dbg::FATAL) {
			std::cerr << "[ndless:fatal] " + where + ": " + message << std::endl;
		}
	}

	Supports PlatformNspire::supports() {
		return Supports::NOTHING;
	}

	std::unique_ptr<Twist> PlatformNspire::getTwister() {
		auto* a = new std::seed_seq{time(nullptr)};
		return std::make_unique<Twist>(
				std::unique_ptr<std::seed_seq>(a)
		);
	}
}
