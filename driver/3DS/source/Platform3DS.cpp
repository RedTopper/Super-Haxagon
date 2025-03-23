// SPDX-FileCopyrightText: 2020 AJ Walter
// SPDX-License-Identifier: GPL-3.0-or-later OR MIT

#include "Driver/Platform.hpp"

#include "Core/Configuration.hpp"
#include "Core/Structs.hpp"
#include "Core/Twist.hpp"
#include "Driver/Font.hpp"
#include "Driver/Music.hpp"
#include "Driver/Screen.hpp"
#include "Driver/Sound.hpp"

#include <3ds.h>
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
	std::unique_ptr<Screen> createScreen(bool wide, bool debugConsole);
	std::unique_ptr<Sound> createSound(const std::string& path);

	extern void audioCallback(void*);
	extern LightEvent _event;

	struct Platform::PlatformImpl {
		PlatformImpl(bool debug) {
			debugConsole = debug;

			gfxInitDefault();
			romfsInit();

			if (debugConsole) {
				// Create console on bottom if we want live debug output
				consoleInit(GFX_BOTTOM, nullptr);
			}

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

			bool wide = consoleModel != CFG_MODEL_2DS && !emulated;

			// Setup NDSP
			ndspInit();
			ndspSetOutputMode(NDSP_OUTPUT_STEREO);
			ndspSetCallback(audioCallback, nullptr);
			LightEvent_Init(&_event, RESET_ONESHOT);

			mkdir("sdmc:/3ds", 0777);
			mkdir("sdmc:/3ds/data", 0777);
			mkdir("sdmc:/3ds/data/haxagon", 0777);

			screen = createScreen(wide, debug);

			// Output the username for debugging
			message(Dbg::INFO, "username", username);
		}

		~PlatformImpl() {
			screen = nullptr;
			ndspExit();
			romfsExit();
			gfxExit();
		}

		bool loop() {
			delta = (svcGetSystemTick() - last) / CPU_TICKS_PER_MSEC / 1000.0f;
			last = svcGetSystemTick();
			return aptMainLoop();
		}

		void shutdown() {
			auto display = false;
			for (const auto& message : messages) {
				if (message.first == Dbg::FATAL) {
					display = true;
				}
			}

			if (display) {
				if (debugConsole) {
					// We already have a debug console, so clear the screen
					// and show a message to press START to quit.
					screen->screenBegin();
					screen->clear(COLOR_BLACK);
					screen->screenSwitch();
					screen->screenFinalize();

					std::cout << std::endl << "[3ds] Fatal error! START to quit." << std::endl;
				} else {
					// Create a new console since we don't have one.
					gfxSetWide(false);
					consoleInit(GFX_TOP, nullptr);
					std::cout << "Fatal error! START to quit." << std::endl;
					std::cout << "Last messages:" << std::endl << std::endl;
					for (const auto& message : messages) {
						std::cout << message.second << std::endl;
					}
				}

				// Hold in a loop.
				while (aptMainLoop()) {
					gspWaitForVBlank();
					gfxSwapBuffers();
					hidScanInput();
					const auto kDown = hidKeysDown();
					if (kDown & KEY_START) break;
				}
			}
		}

		void message(const Dbg dbg, const std::string& where, const std::string& message) {
			std::string format;
			if (dbg == Dbg::INFO) {
				format = "[3ds:info] ";
			} else if (dbg == Dbg::WARN) {
				format = "[3ds:warn] ";
			} else if (dbg == Dbg::FATAL) {
				format = "[3ds:fatal] ";
			}

			format += where + ": " + message;

			if (debugConsole) {
				std::cout << format << std::endl;
			}

			messages.emplace_back(dbg, format);
			if (messages.size() > 32) messages.pop_front();
		}

		std::unique_ptr<Screen> screen{};
		std::deque<std::pair<Dbg, std::string>> messages{};
		uint64_t last = 0;
		float delta = 0.0f;
		bool debugConsole = false;
	};

	Platform::Platform() : _impl(std::make_unique<PlatformImpl>(DEBUG_CONSOLE)) {}

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
			return std::string("romfs:") + partial;
		case Location::USER:
			return std::string("sdmc:/3ds/data/haxagon") + partial;
		}

		return "";
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

	Screen& Platform::getScreen() {
		return *_impl->screen;
	}

	std::string Platform::getButtonName(ButtonName buttonName) {
		switch (buttonName) {
			case ButtonName::BACK: return "B";
			case ButtonName::SELECT: return "A";
			case ButtonName::LEFT: return "L | PAD L | JOY L | Y";
			case ButtonName::RIGHT: return "R | PAD R | JOY R | X";
			case ButtonName::QUIT: return "START";
			default: return "?";
		}
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

	Twist Platform::getTwister() {
		// Kind of a shitty way to do this, but it's the best I got.
		std::unique_ptr<std::seed_seq> seq;
		seq.reset(new std::seed_seq{svcGetSystemTick(), static_cast<u64>(time(nullptr))});
		return Twist(std::move(seq));
	}

	void Platform::shutdown() {
		_impl->shutdown();
	}

	void Platform::message(const Dbg dbg, const std::string& where, const std::string& message) const {
		_impl->message(dbg, where, message);
	}

	Supports Platform::supports() {
		return Supports::SHADOWS;
	}
}
