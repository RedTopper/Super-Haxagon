#include "Driver/3DS/Platform3DS.hpp"

#include "Core/Structs.hpp"
#include "Core/Twist.hpp"
#include "Driver/3DS/AudioLoaderOgg3DS.hpp"
#include "Driver/3DS/AudioLoaderWav3DS.hpp"
#include "Driver/3DS/AudioPlayerOgg3DS.hpp"
#include "Driver/3DS/Font3DS.hpp"

#include <array>
#include <iostream>
#include <locale>
#include <codecvt>
#include <memory>
#include <sys/stat.h>

namespace SuperHaxagon {
	Platform3DS::Platform3DS(const Dbg dbg) : Platform(dbg) {
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
			emulated = username == "LIME3DS";
		}

		gfxSetWide(consoleModel != CFG_MODEL_2DS && !emulated);

		C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
		C2D_Init(C2D_DEFAULT_MAX_OBJECTS);
		C2D_Prepare();

		_top = C2D_CreateScreenTarget(GFX_TOP, GFX_LEFT);

		if (dbg == Dbg::FATAL) {
			// Use the bottom screen for drawing
			_bot = C2D_CreateScreenTarget(GFX_BOTTOM, GFX_LEFT);
		} else {
			// Otherwise create console on bottom if we want more messages
			consoleInit(GFX_BOTTOM, nullptr);
		}

		// Output the username for debugging
		Platform3DS::message(Dbg::INFO, "username", username);

		// Setup NDSP
		ndspInit();
		ndspSetOutputMode(NDSP_OUTPUT_STEREO);
		ndspSetCallback(AudioPlayerOgg3DS::audioCallback, nullptr);
		LightEvent_Init(&AudioPlayerOgg3DS::_event, RESET_ONESHOT);

		mkdir("sdmc:/3ds", 0777);
		mkdir("sdmc:/3ds/data", 0777);
		mkdir("sdmc:/3ds/data/haxagon", 0777);

		// Clear screen
		C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
		C2D_TargetClear(_top, C2D_Color32(0x00, 0x00, 0x00, 0xff));
		C2D_SceneBegin(_top);

		if (_dbg == Dbg::FATAL) {
			// Allowed to draw bottom screen if in fatal mode
			C2D_TargetClear(_bot, C2D_Color32(0x00, 0x00, 0x00, 0xff));
			C2D_SceneBegin(_bot);
		}

		C3D_FrameEnd(0);
	}

	Platform3DS::~Platform3DS() {
		// Call d'tors before filesystem and audio shutdown.
		for (auto& track : _sfx) track = nullptr;
		_bgm = nullptr;
		C2D_Fini();
		C3D_Fini();
		gfxExit();
		ndspExit();
		romfsExit();
	}

	bool Platform3DS::loop() {
		_delta = (svcGetSystemTick() - _last) / CPU_TICKS_PER_MSEC / 1000.0f;
		_last = svcGetSystemTick();
		return aptMainLoop();
	}

	float Platform3DS::getDilation() {
		return _delta / (1.0f / 60.0f);
	}

	std::string Platform3DS::getPath(const std::string& partial, const Location location) {
		switch (location) {
		case Location::ROM:
			return std::string("romfs:") + partial;
		case Location::USER:
			return std::string("sdmc:/3ds/data/haxagon") + partial;
		}

		return "";
	}

	std::unique_ptr<AudioLoader> Platform3DS::loadAudio(const std::string& partial, const Stream stream, const Location location) {
		if (stream == Stream::DIRECT) {
			return std::make_unique<AudioLoaderWav3DS>(getPath(partial, location));
		}
		
		return std::make_unique<AudioLoaderOgg3DS>(getPath(partial, location));
	}

	std::unique_ptr<Font> Platform3DS::loadFont(const std::string& partial, int size, Location location) {
		return std::make_unique<Font3DS>(getPath(partial, location), size);
	}

	// Note: If there are no available channels the audio is silently discarded
	void Platform3DS::playSFX(AudioLoader& audio) {
		auto channel = 1;
		for (auto& player : _sfx) {
			if (!player || player->isDone()) {
				player = audio.instantiate();
				if (!player) return;
				player->setChannel(channel);
				player->setLoop(false);
				player->play();
				break;
			}

			channel++;
		}
	}

	void Platform3DS::playBGM(AudioLoader& audio) {
		_bgm = audio.instantiate();
		if (!_bgm) return;
		_bgm->setChannel(0);
		_bgm->setLoop(true);
		_bgm->play();
	}

	std::string Platform3DS::getButtonName(const Buttons& button) {
		if (button.back) return "B";
		if (button.select) return "A";
		if (button.left) return "LEFT";
		if (button.right) return "RIGHT";
		if (button.quit) return "START";
		return "?";
	}

	Buttons Platform3DS::getPressed() {
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

	Point Platform3DS::getScreenDim() const {
		return {static_cast<float>(_drawingOnTop ? 400 : 320), 240};
	}

	void Platform3DS::screenBegin() {
		_drawingOnTop = true;
		C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
		C2D_TargetClear(_top, C2D_Color32(0x00, 0x00, 0x00, 0xff));
		C2D_SceneBegin(_top);
	}

	void Platform3DS::screenSwap() {
		if (_dbg == Dbg::FATAL) {
			// Allowed to draw bottom screen if in fatal mode
			_drawingOnTop = false;
			C2D_TargetClear(_bot, C2D_Color32(0x00, 0x00, 0x00, 0xff));
			C2D_SceneBegin(_bot);
		}
	}

	void Platform3DS::screenFinalize() {
		C3D_FrameEnd(0);
	}

	void Platform3DS::drawPoly(const Color& color, const std::vector<Point>& points) {
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

	std::unique_ptr<Twist> Platform3DS::getTwister() {
		// Kind of a shitty way to do this but it's the best I got.
		const auto a = new std::seed_seq{svcGetSystemTick(), static_cast<u64>(time(nullptr))};
		return std::make_unique<Twist>(
			std::unique_ptr<std::seed_seq>(a)
		);
	}

	void Platform3DS::shutdown() {
		auto display = false;
		for (const auto& message : _messages) {
			if (message.first == Dbg::FATAL) {
				display = true;
			}
		}

		if (display) {
			if (_dbg == Dbg::FATAL) {
				// Need to create console to show user the error
				consoleInit(GFX_TOP, nullptr);
				std::cout << "Fatal error! START to quit." << std::endl;
				std::cout << "Last messages:" << std::endl << std::endl;
				for (const auto& message : _messages) {
					std::cout << message.second << std::endl;
				}
			} else {
				// Otherwise the console exists and just needs to show
				// the message for a bit longer
				C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
				C2D_TargetClear(_top, C2D_Color32(0x00, 0x00, 0x00, 0xff));
				C2D_SceneBegin(_top);
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

	void Platform3DS::message(const Dbg dbg, const std::string& where, const std::string& message) {
		std::string format;
		if (dbg == Dbg::INFO) {
			format = "[3ds:info] ";
		} else if (dbg == Dbg::WARN) {
			format = "[3ds:warn] ";
		} else if (dbg == Dbg::FATAL) {
			format = "[3ds:fatal] ";
		}

		format += where + ": " + message;

		if (_dbg != Dbg::FATAL) {
			// If we are in non FATAL mode, there's a console to print to
			std::cout << format << std::endl;
		}

		_messages.emplace_back(dbg, format);
		if (_messages.size() > 32) _messages.pop_front();
	}
}
