#include "Driver/Switch/PlatformSwitch.hpp"

#include "Core/Twist.hpp"
#include "Driver/Switch/AudioLoaderSwitch.hpp"
#include "Driver/Switch/FontSwitch.hpp"
#include "Driver/Switch/AudioPlayerMusSwitch.hpp"

#include <EGL/eglext.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <glad/glad.h>

#include <iostream>
#include <sstream>
#include <sys/stat.h>

static const char* vertex_shader = R"text(
#version 330 core

layout(location = 0) in vec2 v_position;
layout(location = 1) in vec4 v_color;
layout(location = 2) in float v_z;

out vec4 f_color;

uniform float s_width;
uniform float s_height;

void main() {
	float x_norm = (v_position.x / s_width - 0.5) * 2.0;
	float y_norm = (v_position.y / s_height - 0.5) * -2.0;

	gl_Position = vec4(x_norm, y_norm, v_z, 1.0);
	f_color = v_color;
}
)text";

static const char* fragment_shader = R"text(
#version 330 core

layout(location = 0) out vec4 color;

in vec4 f_color;

void main() {
	color = f_color;
}
)text";

static const EGLint FRAMEBUFFER_ATTRIBUTE_LIST[] = {
	EGL_RENDERABLE_TYPE, EGL_OPENGL_BIT,
	EGL_RED_SIZE,       8,
	EGL_GREEN_SIZE,     8,
	EGL_BLUE_SIZE,      8,
	EGL_ALPHA_SIZE,     8,
	EGL_DEPTH_SIZE,     24,
	EGL_STENCIL_SIZE,   8,
	EGL_NONE
};

static const EGLint CONTEXT_ATTRIBUTE_LIST[] = {
	EGL_CONTEXT_OPENGL_PROFILE_MASK_KHR, EGL_CONTEXT_OPENGL_CORE_PROFILE_BIT_KHR,
	EGL_CONTEXT_MAJOR_VERSION_KHR, 4,
	EGL_CONTEXT_MINOR_VERSION_KHR, 3,
	EGL_NONE
};

/**
 * Helper function used for debugging OpenGL
 */
static void callback(const GLenum source, const GLenum type, const GLuint id, const GLenum severity, GLsizei, const GLchar* message, const void* userParam) {
	// WCGW casting away const-ness?
	auto* platform = const_cast<SuperHaxagon::PlatformSwitch*>(static_cast<const SuperHaxagon::PlatformSwitch*>(userParam));
	const auto error = type == GL_DEBUG_TYPE_ERROR;
	std::stringstream out;
	out << std::hex << "Message from OpenGL:" << std::endl;
	out << "Source: 0x" << source << std::endl;
	out << "Type: 0x" << type << (error ? " (GL ERROR)" : "") << std::endl;
	out << "ID: 0x" << id << std::endl;
	out << "Severity: 0x" << severity << std::endl;
	out << message;
	platform->message(error ? SuperHaxagon::Dbg::FATAL : SuperHaxagon::Dbg::INFO, "opengl", out.str());
}

namespace SuperHaxagon {
	PlatformSwitch::PlatformSwitch(const Dbg dbg): Platform(dbg) {
		padConfigureInput(8, HidNpadStyleSet_NpadStandard);
		padInitializeAny(&_pad);
		
		romfsInit();
		SDL_Init(SDL_INIT_AUDIO);
		Mix_Init(MIX_INIT_OGG);
		Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 4096);
		Mix_AllocateChannels(16);

		mkdir("sdmc:/switch", 0777);
		mkdir("sdmc:/switch/SuperHaxagon", 0777);

		_window = nwindowGetDefault();
		_console = std::ofstream("sdmc:/switch/SuperHaxagon/out.log");
		_display = eglGetDisplay(EGL_DEFAULT_DISPLAY);

		PlatformSwitch::message(Dbg::INFO, "platform", "booting");
		PlatformSwitch::message(Dbg::INFO, "platform", Mix_GetError());

		// Create window as 1080p, but _width and _height start at 720p.
		// Window is then cropped.
		nwindowSetDimensions(_window, 1920, 1080);
		nwindowSetCrop(_window, 0, 0, _width, _height);

		if (!_display) {
			PlatformSwitch::message(Dbg::FATAL, "display", "error " + std::to_string(eglGetError()));
			return;
		}

		if (!initEGL()) {
			PlatformSwitch::message(Dbg::FATAL, "egl", "there was a fatal error creating an opengl context");
			return;
		}

		gladLoadGL();

		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_FRAMEBUFFER_SRGB);
		glDepthMask(GL_TRUE);
		glDepthFunc(GL_GREATER);
		glDepthRange(0.0f, 1.0f);
		glDebugMessageCallback(callback, this);
		glViewport(0, 1080 - _height, _width, _height);

		_opaque = std::make_shared<RenderTarget<Vertex>>(*this, false, vertex_shader, fragment_shader, "platform opaque");
		_transparent = std::make_shared<RenderTarget<Vertex>>(*this, true, vertex_shader, fragment_shader, "platform transparent");
		addRenderTarget(_opaque);
		addRenderTarget(_transparent);

		_loaded = true;

		PlatformSwitch::message(Dbg::INFO, "platform",  "opengl ok");
	}

	PlatformSwitch::~PlatformSwitch() {
		Mix_Quit();
		SDL_Quit();
		romfsExit();
		PlatformSwitch::message(SuperHaxagon::Dbg::INFO, "platform", "shutdown ok");
	}

	bool PlatformSwitch::loop() {
		if (!_loaded) return false;

		// Check up on the audio status
		if (_bgm && _bgm->isDone()) _bgm->play();

		const auto width = static_cast<float>(_width);
		const auto height = static_cast<float>(_height);
		switch (appletGetOperationMode()) {
			default:
			case AppletOperationMode_Handheld:
				_width = 1280;
				_height = 720;
				break;
			case AppletOperationMode_Console:
				_width = 1920;
				_height = 1080;
				break;
		}

		if (static_cast<unsigned int>(width) != _width || static_cast<unsigned int>(height) != _height) {
			nwindowSetCrop(_window, 0, 0, _width, _height);
			glViewport(0, 1080 - _height, _width, _height);
		}

		return appletMainLoop();
	}

	float PlatformSwitch::getDilation() {
		return 1.0;
	}

	void PlatformSwitch::playSFX(AudioLoader& audio) {
		auto sfx = audio.instantiate();
		if (!sfx) return;
		sfx->play();
	}

	void PlatformSwitch::playBGM(AudioLoader& audio) {
		_bgm = audio.instantiate();
		if (!_bgm) return;
		_bgm->setLoop(true);
		_bgm->play();
	}

	std::string PlatformSwitch::getPath(const std::string& partial, const Location location) {
		switch (location) {
		case Location::ROM:
			return "romfs:" + partial;
		case Location::USER:
			return "sdmc:/switch/SuperHaxagon" + partial;
		}

		return "";
	}

	std::unique_ptr<AudioLoader> PlatformSwitch::loadAudio(const std::string& partial, Stream stream, const Location location) {
		return std::make_unique<AudioLoaderSwitch>(*this, getPath(partial, location), stream);
	}

	std::unique_ptr<Font> PlatformSwitch::loadFont(const std::string& partial, int size) {
		return std::make_unique<FontSwitch>(*this, getPath(partial, Location::ROM), size);
	}

	std::string PlatformSwitch::getButtonName(const Buttons& button) {
		if (button.back) return "B";
		if (button.select) return "A";
		if (button.left) return "LEFT";
		if (button.right) return "RIGHT";
		if (button.quit) return "PLUS";
		return "?";
	}

	Buttons PlatformSwitch::getPressed() {
		padUpdate(&_pad);
		const auto kDown = padGetButtonsDown(&_pad);
		const auto kPressed = padGetButtons(&_pad);
		Buttons buttons{};
		buttons.select = kDown & HidNpadButton_A;
		buttons.back = kDown & HidNpadButton_B;
		buttons.quit = kDown & HidNpadButton_Plus;
		buttons.left = kPressed & (HidNpadButton_L | HidNpadButton_ZL | HidNpadButton_AnyLeft);
		buttons.right = kPressed & (HidNpadButton_R | HidNpadButton_ZR | HidNpadButton_AnyRight);
		return buttons;
	}

	Point PlatformSwitch::getScreenDim() const {
		return Point{static_cast<float>(_width), static_cast<float>(_height)};
	}

	void PlatformSwitch::screenBegin() {
		_z = 0.0f;
		glClearColor(0.2f, 0.3f, 0.8f, 1.0f);
		glClearDepth(0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void PlatformSwitch::screenFinalize() {
		// Want to render opaque first, then transparent
		render(_targetVertex, false);
		render(_targetVertexUV, false);
		render(_targetVertex, true);
		render(_targetVertexUV, true);

		// Present
		eglSwapBuffers(_display, _surface);
	}

	void PlatformSwitch::drawPoly(const Color& color, const std::vector<Point>& points) {
		const auto z = getAndIncrementZ();
		auto& buffer = color.a == 0xFF || color.a == 0 ? _opaque : _transparent;
		for (const auto& point : points) {
			buffer->insert({point, color, z});
		}

		for (size_t i = 1; i < points.size() - 1; i++) {
			buffer->reference(0);
			buffer->reference(i);
			buffer->reference(i + 1);
		}

		buffer->advance(points.size());
	}
	
	std::unique_ptr<Twist> PlatformSwitch::getTwister() {
		// ALSO a shitty way to do this but it's the best I got.
		const auto a = new std::seed_seq{ svcGetSystemTick() };
		return std::make_unique<Twist>(
			std::unique_ptr<std::seed_seq>(a)
		);
	}
	
	void PlatformSwitch::shutdown() {
		if (_display) {
			eglMakeCurrent(_display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);

			if (_context) {
				eglDestroyContext(_display, _context);
			}

			if (_surface) {
				eglDestroySurface(_display, _surface);
			}

			eglTerminate(_display);
		}

		auto display = false;
		for (const auto& message : _messages) {
			if (message.first == Dbg::FATAL) {
				display = true;
			}
		}

		if (display) {
			// Need to create console to show user the error
			consoleInit(nullptr);
			std::cout << "Fatal error! START to quit." << std::endl;
			std::cout << "Last messages:" << std::endl << std::endl;
			for (const auto& message : _messages) {
				std::cout << message.second << std::endl;
			}

			while (appletMainLoop()) {
				consoleUpdate(nullptr);
				padUpdate(&_pad);
				const auto kDown = padGetButtonsDown(&_pad);
				if (kDown & HidNpadButton_Plus) break;
			}
		}
	}

	void PlatformSwitch::message(const Dbg dbg, const std::string& where, const std::string& message) {
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

		if (_dbg != Dbg::FATAL) {
			// If we are in non FATAL mode, write to a file
			_console << format << std::endl;
		}

		_messages.emplace_back(dbg, format);
		if (_messages.size() > 32) _messages.pop_front();
	}

	bool PlatformSwitch::initEGL() {
		eglInitialize(_display, nullptr, nullptr);

		if (eglBindAPI(EGL_OPENGL_API) == EGL_FALSE) {
			message(Dbg::FATAL, "api", "error " + std::to_string(eglGetError()));
			eglTerminate(_display);
			return false;
		}

		EGLConfig config;
		EGLint numConfigs;

		eglChooseConfig(_display, FRAMEBUFFER_ATTRIBUTE_LIST, &config, 1, &numConfigs);
		if (numConfigs == 0) {
			message(Dbg::FATAL, "config", "error " + std::to_string(eglGetError()));
			eglTerminate(_display);
			return false;
		}

		_surface = eglCreateWindowSurface(_display, config, reinterpret_cast<EGLNativeWindowType>(_window), nullptr);
		if (!_surface) {
			message(Dbg::FATAL, "surface", "error " + std::to_string(eglGetError()));
			eglTerminate(_display);
			return false;
		}

		_context = eglCreateContext(_display, config, EGL_NO_CONTEXT, CONTEXT_ATTRIBUTE_LIST);
		if (!_context)
		{
			message(Dbg::FATAL, "context", "error " + std::to_string(eglGetError()));
			eglDestroySurface(_display, _surface);
			eglTerminate(_display);
			return false;
		}

		eglMakeCurrent(_display, _surface, _surface, _context);
		return true;
	}

	float PlatformSwitch::getAndIncrementZ() {
		const auto step = 0.00001f;
		const auto z = _z;
		_z += step;
		return z;
	}

	template<class T>
	void PlatformSwitch::render(std::deque<std::shared_ptr<RenderTarget<T>>> targets, bool transparent) {
		for (const auto& target : targets) {
			if (target->isTransparent() != transparent) continue;
			target->draw(*this);
		}
	}
}
