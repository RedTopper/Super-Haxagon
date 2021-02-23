#include "Driver/NSpire/PlatformNSpire.hpp"

#include "Driver/NSpire/AudioNSpire.hpp"
#include "Driver/NSpire/FontNSpire.hpp"
#include "Driver/NSpire/romfs.h"
#include "Driver/NSpire/timer.h"
#include "Core/Twist.hpp"

#include <ctime>
#include <iostream>
#include <libndls.h>

namespace SuperHaxagon {
	struct membuf : std::streambuf {
		membuf(unsigned char* base, const size_t size) {
			auto* const p(reinterpret_cast<char*>(base));
			this->setg(p, p, p + size);
		}
	};
	
	struct imemstream : virtual membuf, std::istream {
		imemstream(unsigned char* base, const size_t size):
			membuf(base, size),
			std::istream(static_cast<std::streambuf*>(this)) {
		}
	};
	
	PlatformNSpire::PlatformNSpire(const Dbg dbg) : Platform(dbg) {
		auto* const gc = gui_gc_global_GC();
		gui_gc_begin(gc);
		gui_gc_setColorRGB(gc, 0, 0, 0);
		gui_gc_fillRect(gc, 0, 0, 320, 240);
		gui_gc_finish(gc);
		gui_gc_blit_to_screen(gc);

		timer_init(0);
		timer_load(0, TIMER_1S);
	}

	bool PlatformNSpire::loop() {
		const auto timer = timer_read(0);
		const auto elapsed = static_cast<float>(TIMER_1S - timer) / TIMER_1S;
		if (_bgm) {
			// Trust me I know what I'm doing with my types, compiler.
			auto* bgm = reinterpret_cast<PlayerNSpire*>(_bgm.get());
			bgm->addTime(elapsed);
			if (bgm->isDone()) bgm->play();
		}

		timer_load(0, TIMER_1S);
		_dilation = elapsed * 60.0f;
		
		return true;
	}

	float PlatformNSpire::getDilation() {
		return _dilation;
	}

	std::string PlatformNSpire::getPath(const std::string& partial, const Location location) {
		switch (location) {
		case Location::ROM:
			return partial;
		case Location::USER:
			return std::string(get_documents_dir()) + "ndless" + partial + std::string(".tns");
		}

		return "";
	}

	std::unique_ptr<std::istream> PlatformNSpire::openFile(const std::string& partial, const Location location) {
		if (location == Location::USER) {
			return std::make_unique<std::ifstream>(getPath(partial, location), std::ios::in | std::ios::binary);
		}

		if (partial == "/levels.haxagon") {
			return std::make_unique<imemstream>(&romfs_levels_haxagon[0], romfs_levels_haxagon_len);
		}
		
		if (partial == "/bgm/callMeKatla.txt") {
			return std::make_unique<imemstream>(&romfs_bgm_callMeKatla_txt[0], romfs_bgm_callMeKatla_txt_len);
		}

		if (partial == "/bgm/captainCool.txt") {
			return std::make_unique<imemstream>(&romfs_bgm_captainCool_txt[0], romfs_bgm_captainCool_txt_len);
		}

		if (partial == "/bgm/commandoSteve.txt") {
			return std::make_unique<imemstream>(&romfs_bgm_commandoSteve_txt[0], romfs_bgm_commandoSteve_txt_len);
		}

		if (partial == "/bgm/drFinkelfracken.txt") {
			return std::make_unique<imemstream>(&romfs_bgm_drFinkelfracken_txt[0], romfs_bgm_drFinkelfracken_txt_len);
		}

		if (partial == "/bgm/esiannoyamFoEzam.txt") {
			return std::make_unique<imemstream>(&romfs_bgm_esiannoyamFoEzam_txt[0], romfs_bgm_esiannoyamFoEzam_txt_len);
		}

		if (partial == "/bgm/jackRussel.txt") {
			return std::make_unique<imemstream>(&romfs_bgm_jackRussel_txt[0], romfs_bgm_jackRussel_txt_len);
		}

		if (partial == "/bgm/screenSaver.txt") {
			return std::make_unique<imemstream>(&romfs_bgm_screenSaver_txt[0], romfs_bgm_screenSaver_txt_len);
		}

		return nullptr;
	}

	std::unique_ptr<Audio> PlatformNSpire::loadAudio(const std::string& partial, Stream, const Location location) {
		return std::make_unique<AudioNSpire>(openFile(partial + ".txt", location));
	}

	std::unique_ptr<Font> PlatformNSpire::loadFont(const std::string&, int size) {
		return std::make_unique<FontNSpire>(size);
	}

	void PlatformNSpire::playBGM(Audio& audio) {
		_bgm = audio.instantiate();
		if (!_bgm) return;
		_bgm->setLoop(true);
		_bgm->play();
	}

	std::string PlatformNSpire::getButtonName(const Buttons& button) {
		if (button.back) return "ESC";
		if (button.select) return "ENTER";
		if (button.left) return "4";
		if (button.right) return "6";
		if (button.quit) return "HOME";
		return "?";
	}

	Buttons PlatformNSpire::getPressed() {
		Buttons buttons{};
		buttons.select = isKeyPressed(KEY_NSPIRE_ENTER) > 0;
		buttons.back = isKeyPressed(KEY_NSPIRE_ESC) > 0;
		buttons.quit = isKeyPressed(KEY_NSPIRE_HOME) > 0 || isKeyPressed(KEY_NSPIRE_MENU) > 0;
		buttons.left = isKeyPressed(KEY_NSPIRE_4) > 0;
		buttons.right = isKeyPressed(KEY_NSPIRE_6)  > 0;
		return buttons;
	}

	Point PlatformNSpire::getScreenDim() const {
		return {320, 240};
	}

	void PlatformNSpire::screenBegin() {
		auto* gc = gui_gc_global_GC();
		gui_gc_begin(gc);
		gui_gc_setColorRGB(gc, 0, 0, 0);
		gui_gc_fillRect(gc, 0, 0, 320, 240);
	}

	void PlatformNSpire::screenFinalize() {
		auto* gc = gui_gc_global_GC();
		gui_gc_blit_to_screen(gc);
	}

	void PlatformNSpire::drawPoly(const Color& color, const std::vector<Point>& points) {
		auto* gc = gui_gc_global_GC();
		gui_gc_setColorRGB(gc, color.r, color.g, color.b);
		const auto pos = std::make_unique<Point2D[]>(points.size());
		for (size_t i = 0; i < points.size(); i++) {
			pos[i] = { points[i].x, points[i].y };
		}
		
		gui_gc_fillPoly(gc, reinterpret_cast<unsigned*>(pos.get()), points.size());
	}

	void PlatformNSpire::shutdown() {
		auto* gc = gui_gc_global_GC();
		gui_gc_finish(gc);
		timer_restore(0);
	}

	void PlatformNSpire::message(const Dbg dbg, const std::string& where, const std::string& message) {
		if (dbg == Dbg::INFO) {
			std::cout << "[ndless:info] " + where + ": " + message << std::endl;
		} else if (dbg == Dbg::WARN) {
			std::cout << "[ndless:warn] " + where + ": " + message << std::endl;
		} else if (dbg == Dbg::FATAL) {
			std::cerr << "[ndless:fatal] " + where + ": " + message << std::endl;
		}
	}

	Supports PlatformNSpire::supports() {
		return Supports::NOTHING;
	}

	std::unique_ptr<Twist> PlatformNSpire::getTwister() {
		auto* a = new std::seed_seq{time(nullptr)};
		return std::make_unique<Twist>(
				std::unique_ptr<std::seed_seq>(a)
		);
	}
}
