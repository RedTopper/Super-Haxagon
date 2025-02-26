#include "Driver/Platform.hpp"

#include "RenderTarget.hpp"
#include "Core/Configuration.hpp"
#include "Core/Twist.hpp"
#include "Driver/Font.hpp"
#include "Driver/Music.hpp"
#include "Driver/Screen.hpp"
#include "Driver/Sound.hpp"

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <switch.h>

#include <iostream>
#include <deque>
#include <fstream>
#include <sys/stat.h>

namespace SuperHaxagon {
	std::unique_ptr<Music> createMusic(const std::string& path);
	std::unique_ptr<Sound> createSound(const std::string& path);
	std::unique_ptr<Font> createFont(const Platform& platform, const std::string& path, int size, float* z, std::shared_ptr<RenderTarget<VertexUV>>& surface);
	std::unique_ptr<Screen> createScreen(const Platform& platform, VertexStorage& targetVertex, VertexUVStorage& targetVertexUV, float& z);

	struct Platform::PlatformImpl {
		PlatformImpl(bool debug) {
			debugConsole = debug;

			romfsInit();
			SDL_Init(SDL_INIT_AUDIO);
			Mix_Init(MIX_INIT_OGG);
			Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 4096);
			Mix_AllocateChannels(16);

			mkdir("sdmc:/switch", 0777);
			mkdir("sdmc:/switch/SuperHaxagon", 0777);

			if (debugConsole) console = std::ofstream("sdmc:/switch/SuperHaxagon/out.log");

			message(Dbg::INFO, "platform", "mix says: " + std::string(Mix_GetError()));

			padConfigureInput(8, HidNpadStyleSet_NpadStandard);
			padInitializeAny(&pad);

			message(Dbg::INFO, "platform", "platform init ok");
		}

		~PlatformImpl() {
			for (auto& sfx : sfxBuffers) Mix_FreeChunk(sfx.second);
			if (musicBuffer) Mix_FreeMusic(musicBuffer);
			Mix_Quit();
			SDL_Quit();
			romfsExit();
			message(SuperHaxagon::Dbg::INFO, "platform", "shutdown ok");
		}

		void initScreen(const Platform& platform) {
			// Do anything that requires a valid handle to platform here
			// instead of in the constructor.
			screen = createScreen(platform, targetVertex, targetVertexUV, z);
		}

		bool loop() {
			return appletMainLoop();
		}

		Buttons getPressed() {
			padUpdate(&pad);
			const auto kDown = padGetButtonsDown(&pad);
			const auto kPressed = padGetButtons(&pad);
			Buttons buttons{};
			buttons.select = kDown & HidNpadButton_A;
			buttons.back = kDown & HidNpadButton_B;
			buttons.quit = kDown & HidNpadButton_Plus;
			buttons.left = kPressed & (HidNpadButton_L | HidNpadButton_ZL | HidNpadButton_AnyLeft | HidNpadButton_Y);
			buttons.right = kPressed & (HidNpadButton_R | HidNpadButton_ZR | HidNpadButton_AnyRight | HidNpadButton_X);
			return buttons;
		}

		void shutdown() {
			// Shutdown openGL first
			screen = nullptr;

			auto display = false;
			for (const auto& message : messages) {
				if (message.first == Dbg::FATAL) {
					display = true;
				}
			}

			if (display) {
				// Need to create console to show user the error
				consoleInit(nullptr);
				std::cout << "Fatal error! + to quit." << std::endl;
				std::cout << "Last messages:" << std::endl << std::endl;
				for (const auto& message : messages) {
					std::cout << message.second << std::endl;
				}

				while (appletMainLoop()) {
					consoleUpdate(nullptr);
					padUpdate(&pad);
					const auto kDown = padGetButtonsDown(&pad);
					if (kDown & HidNpadButton_Plus) break;
				}
			}
		}

		void message(const Dbg dbg, const std::string& where, const std::string& message) {
			std::string format;
			if (dbg == Dbg::INFO) {
				format = "[switch:info] ";
			}
			else if (dbg == Dbg::WARN) {
				format = "[switch:warn] ";
			}
			else if (dbg == Dbg::FATAL) {
				format = "[switch:fatal] ";
			}

			format += where + ": " + message;

			if (debugConsole) {
				// Write to a file in debug mode
				console << format << std::endl << std::flush;
			}

			messages.emplace_back(dbg, format);
			if (messages.size() > 32) messages.pop_front();
		}

		bool debugConsole = false;

		float z = 0.0f;
		PadState pad{};

		std::ofstream console;
		std::deque<std::pair<Dbg, std::string>> messages{};
		VertexStorage targetVertex{};
		VertexUVStorage targetVertexUV{};

		std::vector<std::pair<SoundEffect, Mix_Chunk*>> sfxBuffers{};
		Mix_Music* musicBuffer{};

		std::unique_ptr<Screen> screen = nullptr;
	};

	Platform::Platform() : _impl(std::make_unique<Platform::PlatformImpl>(DEBUG_CONSOLE)) {
		// Now that we have a valid _impl pointer (initialized above)
		// we can do any initialization that requires this platform's methods to work
		// (for example, Platform::message(...) needing a valid _impl pointer)
		_impl->initScreen(*this);
	}

	Platform::~Platform() = default;

	bool Platform::loop() {
		return _impl->loop();
	}

	// We assume that the Switch keeps up with the game at all times
	// since it should be the only thing running.
	float Platform::getDilation() const {
		return 1.0;
	}

	std::string Platform::getPath(const std::string& partial, const Location location) const {
		switch (location) {
		case Location::ROM:
			return "romfs:" + partial;
		case Location::USER:
			return "sdmc:/switch/SuperHaxagon" + partial;
		}

		return "";
	}

	std::unique_ptr<Font> Platform::loadFont(const int size) const {
		std::shared_ptr<RenderTarget<VertexUV>> fontSurface = nullptr;
		auto font = createFont(*this, getPath("/bump-it-up", Location::ROM), size, &_impl->z, fontSurface);
		_impl->targetVertexUV.emplace_back(fontSurface);
		return font;
	}

	std::unique_ptr<Sound> Platform::loadSound(const std::string& base) const {
		return createSound(getPath(base, Location::ROM) + ".wav");
	}

	std::unique_ptr<Music> Platform::loadMusic(const std::string& base, const Location location) const {
		return createMusic(getPath(base, location) + ".ogg");
	}

	Screen& Platform::getScreen() {
		return *_impl->screen;
	}

	std::string Platform::getButtonName(ButtonName buttonName) {
		switch (buttonName) {
			case ButtonName::BACK: return "B";
			case ButtonName::SELECT: return "A";
			case ButtonName::LEFT: return "L | ZL | PAD L | Y";
			case ButtonName::RIGHT: return "R | ZR | PAD R | X";
			case ButtonName::QUIT: return "PLUS";
			default: return "?";
		}
	}

	Buttons Platform::getPressed() const {
		return _impl->getPressed();
	}
	
	Twist Platform::getTwister() {
		// ALSO a shitty way to do this, but it's the best I got.
		std::unique_ptr<std::seed_seq> seq;
		seq.reset(new std::seed_seq{ svcGetSystemTick() });
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
