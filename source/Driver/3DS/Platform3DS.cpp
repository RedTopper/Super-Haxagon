#include <3ds.h>
#include <string>
#include <memory>
#include <array>
#include <iostream>
#include <sys/stat.h>

#include "Driver/3DS/PlayerOgg3DS.hpp"
#include "Driver/3DS/AudioWav3DS.hpp"
#include "Driver/3DS/AudioOgg3DS.hpp"
#include "Driver/3DS/Font3DS.hpp"
#include "Driver/3DS/Platform3DS.hpp"

namespace SuperHaxagon {
	Platform3DS::Platform3DS(Dbg dbg) : Platform(dbg) {
		romfsInit();
		gfxInitDefault();
		C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
		C2D_Init(C2D_DEFAULT_MAX_OBJECTS);
		C2D_Prepare();

		_buff = C2D_TextBufNew(4096);
		_top = C2D_CreateScreenTarget(GFX_TOP, GFX_LEFT);

		if (_dbg == Dbg::FATAL) {
			// Use the bottom screen for drawing
			_bot = C2D_CreateScreenTarget(GFX_BOTTOM, GFX_LEFT);
		} else {
			// Otherwise create console on bottom if we want more messages
			consoleInit(GFX_BOTTOM, nullptr);
		}

		// Setup NDSP
		ndspInit();
		ndspSetOutputMode(NDSP_OUTPUT_STEREO);
		ndspSetCallback(PlayerOgg3DS::audioCallback, nullptr);
		LightEvent_Init(&PlayerOgg3DS::_event, RESET_ONESHOT);

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
		C2D_Fini();
		C3D_Fini();
		gfxExit();
		ndspExit();
		romfsExit();
	}

	bool Platform3DS::loop() {
		return aptMainLoop();
	}

	double Platform3DS::getDilation() {
		return 1.0;
	}

	std::string Platform3DS::getPath(const std::string& partial) {
		return std::string("sdmc:/3ds/data/haxagon") + partial;
	}

	std::string Platform3DS::getPathRom(const std::string& partial) {
		return std::string("romfs:") + partial;
	}

	std::unique_ptr<Audio> Platform3DS::loadAudio(const std::string& path, const Stream stream) {
		if (stream == Stream::DIRECT) {
			return std::make_unique<AudioWav3DS>(path);
		}
		
		return std::make_unique<AudioOgg3DS>(path);
	}

	std::unique_ptr<Font> Platform3DS::loadFont(const std::string& path, int size) {
		return std::make_unique<Font3DS>(path, size, _buff);
	}

	// Note: If there are no available channels the audio is silently discarded
	void Platform3DS::playSFX(Audio& audio) {
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

	void Platform3DS::playBGM(Audio& audio) {
		_bgm = audio.instantiate();
		if (!_bgm) return;
		_bgm->setChannel(0);
		_bgm->setLoop(true);
		_bgm->play();
	}

	void Platform3DS::stopBGM() {
		_bgm = nullptr;
	}

	double Platform3DS::getBgmVelocity() {
		if (_bgm) return _bgm->getVelocity();
		return 0;
	}

	std::string Platform3DS::getButtonName(const Buttons button) {
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
		return {static_cast<double>(_drawingOnTop ? 400 : 320), 240};
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

	void Platform3DS::drawRect(const Color& color, const Point& point, const Point& size) {
		const auto c = C2D_Color32(color.r, color.g, color.b, color.a);
		C2D_DrawRectangle(
			static_cast<float>(point.x), 
			static_cast<float>(point.y), 0, 
			static_cast<float>(size.x), 
			static_cast<float>(size.y), 
			c, c, c, c
		);
	}

	void Platform3DS::drawTriangle(const Color& color, const std::array<Point, 3>& points) {
		const auto c = C2D_Color32(color.r, color.g, color.b, color.a);
		const auto height = getScreenDim().y;
		C2D_DrawTriangle(
			static_cast<float>(points[0].x), static_cast<float>(height - points[0].y), c,
			static_cast<float>(points[1].x), static_cast<float>(height - points[1].y), c,
			static_cast<float>(points[2].x), static_cast<float>(height - points[2].y), c,
			0
		);
	}

	std::unique_ptr<Twist> Platform3DS::getTwister() {
		// Kind of a shitty way to do this but it's the best I got.
		const auto a = new std::seed_seq{svcGetSystemTick()};
		return std::make_unique<Twist>(
			std::unique_ptr<std::seed_seq>(a)
		);
	}

	void Platform3DS::show() {
		bool display = false;
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
				u32 kDown = hidKeysDown();
				if (kDown & KEY_START) break;
			}
		}
	}

	void Platform3DS::message(Dbg dbg, const std::string& where, const std::string& message) {
		std::string format;
		if (dbg == Dbg::INFO) {
			format = "[3ds:info] " + where + ": " + message;
		} else if (dbg == Dbg::WARN) {
			format = "[3ds:warn] " + where + ": " + message;
		} else if (dbg == Dbg::FATAL){
			format = "[3ds:fatal] " + where + ": " + message;
		}

		if (_dbg != Dbg::FATAL) {
			// If we are in non FATAL mode, there's a console to print to
			std::cout << format << std::endl;
		}

		_messages.emplace_back(dbg, format);
		if (_messages.size() > 32) _messages.pop_front();
	}
}
