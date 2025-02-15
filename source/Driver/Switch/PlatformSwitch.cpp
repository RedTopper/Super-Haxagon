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
	typedef std::deque<std::shared_ptr<RenderTarget<Vertex>>> VertexStorage;
	typedef std::deque<std::shared_ptr<RenderTarget<VertexUV>>> VertexUVStorage;

	std::unique_ptr<Music> createMusic(const std::string& path);
	std::unique_ptr<Sound> createSound(const std::string& path);
	std::unique_ptr<Font> createFont(const Platform& platform, const std::string& path, int size, float* z, std::shared_ptr<RenderTarget<VertexUV>>& surface);
	std::unique_ptr<Screen> createScreen(
			const Platform& platform,
			VertexStorage& targetVertex,
			VertexUVStorage& targetVertexUV,
			unsigned int& width,
			unsigned int& height,
			float& z);

	struct Platform::PlatformData {
		bool debugConsole = false;
		bool loaded = false;

		unsigned int width = 1280;
		unsigned int height = 720;

		float z = 0.0f;
		PadState pad{};

		NWindow* window;

		std::ofstream console;
		std::deque<std::pair<Dbg, std::string>> messages{};
		VertexStorage targetVertex{};
		VertexUVStorage targetVertexUV{};

		std::vector<std::pair<SoundEffect, Mix_Chunk*>> sfxBuffers{};
		Mix_Music* musicBuffer{};
	};

	Platform::Platform() : _plat(std::make_unique<Platform::PlatformData>()) {
		romfsInit();
		SDL_Init(SDL_INIT_AUDIO);
		Mix_Init(MIX_INIT_OGG);
		Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 4096);
		Mix_AllocateChannels(16);

		mkdir("sdmc:/switch", 0777);
		mkdir("sdmc:/switch/SuperHaxagon", 0777);

		_plat->debugConsole = DEBUG_CONSOLE;
		if (_plat->debugConsole) _plat->console = std::ofstream("sdmc:/switch/SuperHaxagon/out.log");

		padConfigureInput(8, HidNpadStyleSet_NpadStandard);
		padInitializeAny(&_plat->pad);

		message(Dbg::INFO, "platform", Mix_GetError());

		_screen = createScreen(*this, _plat->targetVertex, _plat->targetVertexUV, _plat->width, _plat->height, _plat->z);

		_plat->loaded = _screen != nullptr;
	}

	Platform::~Platform() {
		for (auto& sfx : _plat->sfxBuffers) Mix_FreeChunk(sfx.second);
		if (_plat->musicBuffer) Mix_FreeMusic(_plat->musicBuffer);
		Mix_Quit();
		SDL_Quit();
		romfsExit();
		message(SuperHaxagon::Dbg::INFO, "platform", "shutdown ok");
	}

	bool Platform::loop() {
		if (!_plat->loaded) return false;

		const auto width = static_cast<float>(_plat->width);
		const auto height = static_cast<float>(_plat->height);
		switch (appletGetOperationMode()) {
			default:
			case AppletOperationMode_Handheld:
				_plat->width = 1280;
				_plat->height = 720;
				break;
			case AppletOperationMode_Console:
				_plat->width = 1920;
				_plat->height = 1080;
				break;
		}

		if (static_cast<unsigned int>(width) != _plat->width || static_cast<unsigned int>(height) != _plat->height) {
			nwindowSetCrop(_plat->window, 0, 0, _plat->width, _plat->height);
			glViewport(0, 1080 - _plat->height, _plat->width, _plat->height);
		}

		return appletMainLoop();
	}

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

	std::unique_ptr<std::istream> Platform::openFile(const std::string& partial, const Location location) const {
		return std::make_unique<std::ifstream>(getPath(partial, location), std::ios::in | std::ios::binary);
	}

	std::unique_ptr<Font> Platform::loadFont(const int size) const {
		std::shared_ptr<RenderTarget<VertexUV>> fontSurface = nullptr;
		auto font = createFont(*this, getPath("/bump-it-up", Location::ROM), size, &_plat->z, fontSurface);
		_plat->targetVertexUV.emplace_back(fontSurface);
		return font;
	}

	std::unique_ptr<Sound> Platform::loadSound(const std::string& base) const {
		return createSound(getPath(base, Location::ROM) + ".wav");
	}

	std::unique_ptr<Music> Platform::loadMusic(const std::string& base, const Location location) const {
		return createMusic(getPath(base, location) + ".ogg");
	}

	std::string Platform::getButtonName(const Buttons& button) {
		if (button.back) return "B";
		if (button.select) return "A";
		if (button.left) return "LEFT";
		if (button.right) return "RIGHT";
		if (button.quit) return "PLUS";
		return "?";
	}

	Buttons Platform::getPressed() const {
		padUpdate(&_plat->pad);
		const auto kDown = padGetButtonsDown(&_plat->pad);
		const auto kPressed = padGetButtons(&_plat->pad);
		Buttons buttons{};
		buttons.select = kDown & HidNpadButton_A;
		buttons.back = kDown & HidNpadButton_B;
		buttons.quit = kDown & HidNpadButton_Plus;
		buttons.left = kPressed & (HidNpadButton_L | HidNpadButton_ZL | HidNpadButton_AnyLeft);
		buttons.right = kPressed & (HidNpadButton_R | HidNpadButton_ZR | HidNpadButton_AnyRight);
		return buttons;
	}
	
	std::unique_ptr<Twist> Platform::getTwister() {
		// ALSO a shitty way to do this, but it's the best I got.
		const auto a = new std::seed_seq{ svcGetSystemTick() };
		return std::make_unique<Twist>(
			std::unique_ptr<std::seed_seq>(a)
		);
	}
	
	void Platform::shutdown() {
		_screen = nullptr;

		auto display = false;
		for (const auto& message : _plat->messages) {
			if (message.first == Dbg::FATAL) {
				display = true;
			}
		}

		if (display) {
			// Need to create console to show user the error
			consoleInit(nullptr);
			std::cout << "Fatal error! START to quit." << std::endl;
			std::cout << "Last messages:" << std::endl << std::endl;
			for (const auto& message : _plat->messages) {
				std::cout << message.second << std::endl;
			}

			while (appletMainLoop()) {
				consoleUpdate(nullptr);
				padUpdate(&_plat->pad);
				const auto kDown = padGetButtonsDown(&_plat->pad);
				if (kDown & HidNpadButton_Plus) break;
			}
		}
	}

	void Platform::message(const Dbg dbg, const std::string& where, const std::string& message) const {
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

		if (_plat->debugConsole) {
			// Write to a file in debug mode
			_plat->console << format << std::endl;
		}

		_plat->messages.emplace_back(dbg, format);
		if (_plat->messages.size() > 32) _plat->messages.pop_front();
	}

	Supports Platform::supports() {
		return Supports::SHADOWS;
	}
}
