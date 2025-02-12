#include "Driver/Platform.hpp"

#include "Wav3DS.hpp"
#include "Core/Configuration.hpp"
#include "Core/Structs.hpp"
#include "Core/Twist.hpp"
#include "Driver/Font.hpp"
#include "Driver/Music.hpp"
#include "Driver/Sound.hpp"

#include <citro2d.h>

#include <array>
#include <iostream>
#include <sstream>
#include <locale>
#include <codecvt>
#include <memory>
#include <deque>
#include <sys/stat.h>

namespace SuperHaxagon {
	std::unique_ptr<Font> createFont(const std::string& path, int size);
	std::unique_ptr<Music> createMusic(const std::string& path);
	std::unique_ptr<Sound> createSound(const std::string& path);

	extern void audioCallback(void*);
	extern LightEvent _event;

	struct Platform::PlatformData {
		std::deque<std::pair<Dbg, std::string>> messages{};

		C3D_RenderTarget* top = nullptr;
		C3D_RenderTarget* bot = nullptr;
		C2D_TextBuf buff;

		bool drawingOnTop = true;
		bool debugConsole = false;
		uint64_t last = 0;
	};

	Platform::Platform() : _plat(std::make_unique<PlatformData>()) {
		_plat->debugConsole = DEBUG_CONSOLE;

		romfsInit();
		gfxInitDefault();

		// Metadata to gather from CFGU
		std::string username;
		u8 consoleModel = 0;
		auto emulated = false;

		const auto res = cfguInit();
		if (R_SUCCEEDED(res)) {
			const auto usernameBlockID = 0x000A0000;
			const auto usernameBlockSize = 0x1C; // in bytes
			const auto buffer = std::make_unique<char16_t[]>(usernameBlockSize / sizeof(char16_t));
			CFGU_GetSystemModel(&consoleModel);
			CFGU_GetConfigInfoBlk2(usernameBlockSize, usernameBlockID, buffer.get());
			cfguExit();

			// Lime3DS (previously Citra) cannot use wide mode, so to make emulation of the game more accessible we'll check for it
			// This is a shitty hack, either the Lime3DS developers should fix wide mode or this should be done right.
			std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> convert;
			username = convert.to_bytes(buffer.get());
			emulated =
				username == "LIME3DS" ||
				username == "ENCORE" ||
				username == "CITRA";
		}

		gfxSetWide(consoleModel != CFG_MODEL_2DS && !emulated);

		C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
		C2D_Init(C2D_DEFAULT_MAX_OBJECTS);
		C2D_Prepare();

		_plat->top = C2D_CreateScreenTarget(GFX_TOP, GFX_LEFT);

		if (!_plat->debugConsole) {
			// Use the bottom screen for drawing
			_plat->bot = C2D_CreateScreenTarget(GFX_BOTTOM, GFX_LEFT);
		} else {
			// Otherwise create console on bottom if we want more messages
			consoleInit(GFX_BOTTOM, nullptr);
		}

		// Output the username for debugging
		Platform::message(Dbg::INFO, "username", username);

		// Setup NDSP
		ndspInit();
		ndspSetOutputMode(NDSP_OUTPUT_STEREO);
		ndspSetCallback(audioCallback, nullptr);
		LightEvent_Init(&_event, RESET_ONESHOT);

		mkdir("sdmc:/3ds", 0777);
		mkdir("sdmc:/3ds/data", 0777);
		mkdir("sdmc:/3ds/data/haxagon", 0777);

		// Clear screen
		C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
		C2D_TargetClear(_plat->top, C2D_Color32(0x00, 0x00, 0x00, 0xff));
		C2D_SceneBegin(_plat->top);

		if (!_plat->debugConsole) {
			C2D_TargetClear(_plat->bot, C2D_Color32(0x00, 0x00, 0x00, 0xff));
			C2D_SceneBegin(_plat->bot);
		}

		C3D_FrameEnd(0);
	}

	Platform::~Platform() {
		C2D_Fini();
		C3D_Fini();
		gfxExit();
		ndspExit();
		romfsExit();
	}

	bool Platform::loop() {
		_delta = (svcGetSystemTick() - _plat->last) / CPU_TICKS_PER_MSEC / 1000.0f;
		_plat->last = svcGetSystemTick();
		return aptMainLoop();
	}

	float Platform::getDilation() const {
		return _delta / (1.0f / 60.0f);
	}

	std::string Platform::getPath(const std::string& partial, const Location location) const {
		switch (location) {
		case Location::ROM:
			return std::string("romfs:") + partial;
		case Location::USER:
			return std::string("sdmc:/3ds/data/haxagon") + partial;
		}

		return "";
	}

	std::unique_ptr<std::istream> Platform::openFile(const std::string& partial, const Location location) const {
		return std::make_unique<std::ifstream>(getPath(partial, location), std::ios::in | std::ios::binary);
	}

	std::unique_ptr<Font> Platform::loadFont(int size) const {
		std::stringstream s;
		s << "/bump-it-up-" << size << ".bcfnt";
		return createFont(getPath(s.str(), Location::ROM), size);
	}

	std::unique_ptr<Sound> Platform::loadSound(const std::string& base) const {
		return createSound(getPath(base, Location::ROM));
	}

	std::unique_ptr<Music> Platform::loadMusic(const std::string& base, Location location) const {
		return createMusic(getPath(base, location));
	}

	std::string Platform::getButtonName(const Buttons& button) {
		if (button.back) return "B";
		if (button.select) return "A";
		if (button.left) return "LEFT";
		if (button.right) return "RIGHT";
		if (button.quit) return "START";
		return "?";
	}

	Buttons Platform::getPressed() const {
		hidScanInput();
		const auto input = hidKeysHeld();
		Buttons buttons{};
		buttons.select = (input & KEY_A) > 0;
		buttons.back = (input & KEY_B) > 0;
		buttons.quit = (input & KEY_START) > 0;
		buttons.left = (input & (KEY_L | KEY_ZL | KEY_CSTICK_LEFT | KEY_CPAD_LEFT | KEY_DLEFT | KEY_Y)) > 0;
		buttons.right = (input & (KEY_R | KEY_ZR | KEY_CSTICK_RIGHT | KEY_CPAD_RIGHT | KEY_DRIGHT | KEY_X)) > 0;
		return buttons;
	}

	Point Platform::getScreenDim() const {
		return {static_cast<float>(_plat->drawingOnTop ? 400 : 320), 240};
	}

	void Platform::screenBegin() const {
		_plat->drawingOnTop = true;
		C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
		C2D_TargetClear(_plat->top, C2D_Color32(0x00, 0x00, 0x00, 0xff));
		C2D_SceneBegin(_plat->top);
	}

	void Platform::screenSwap() {
		if (!_plat->debugConsole) {
			// Allowed to draw bottom screen if in fatal mode
			_plat->drawingOnTop = false;
			C2D_TargetClear(_plat->bot, C2D_Color32(0x00, 0x00, 0x00, 0xff));
			C2D_SceneBegin(_plat->bot);
		}
	}

	void Platform::screenFinalize() const {
		C3D_FrameEnd(0);
	}

	void Platform::drawPoly(const Color& color, const std::vector<Point>& points) const {
		const auto c = C2D_Color32(color.r, color.g, color.b, color.a);
		for (size_t i = 1; i < points.size() - 1; i++) {
			C2D_DrawTriangle(
				static_cast<float>(points[0].x), static_cast<float>(points[0].y), c,
				static_cast<float>(points[i].x), static_cast<float>(points[i].y), c,
				static_cast<float>(points[i + 1].x), static_cast<float>(points[i + 1].y), c,
				0
			);
		}
	}

	std::unique_ptr<Twist> Platform::getTwister() {
		// Kind of a shitty way to do this, but it's the best I got.
		const auto a = new std::seed_seq{svcGetSystemTick(), static_cast<u64>(time(nullptr))};
		return std::make_unique<Twist>(
			std::unique_ptr<std::seed_seq>(a)
		);
	}

	void Platform::shutdown() {
		auto display = false;
		for (const auto& message : _plat->messages) {
			if (message.first == Dbg::FATAL) {
				display = true;
			}
		}

		if (display) {
			if (_plat->debugConsole) {
				// Need to create console to show user the error
				consoleInit(GFX_TOP, nullptr);
				std::cout << "Fatal error! START to quit." << std::endl;
				std::cout << "Last messages:" << std::endl << std::endl;
				for (const auto& message : _plat->messages) {
					std::cout << message.second << std::endl;
				}
			} else {
				// Otherwise the console exists and just needs to show
				// the message for a bit longer
				C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
				C2D_TargetClear(_plat->top, C2D_Color32(0x00, 0x00, 0x00, 0xff));
				C2D_SceneBegin(_plat->top);
				C3D_FrameEnd(0);

				std::cout << std::endl << "[3ds] Fatal error! START to quit." << std::endl;
			}

			while (aptMainLoop()) {
				gspWaitForVBlank();
				gfxSwapBuffers();
				hidScanInput();
				const auto kDown = hidKeysDown();
				if (kDown & KEY_START) break;
			}
		}
	}

	void Platform::message(const Dbg dbg, const std::string& where, const std::string& message) const {
		std::string format;
		if (dbg == Dbg::INFO) {
			format = "[3ds:info] ";
		} else if (dbg == Dbg::WARN) {
			format = "[3ds:warn] ";
		} else if (dbg == Dbg::FATAL) {
			format = "[3ds:fatal] ";
		}

		format += where + ": " + message;

		if (_plat->debugConsole) {
			std::cout << format << std::endl;
		}

		_plat->messages.emplace_back(dbg, format);
		if (_plat->messages.size() > 32) _plat->messages.pop_front();
	}

	Supports Platform::supports() {
		return Supports::SHADOWS;
	}
}
