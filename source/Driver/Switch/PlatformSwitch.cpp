#include "Driver/Platform.hpp"

#include "RenderTarget.hpp"
#include "Core/Configuration.hpp"
#include "Core/Twist.hpp"
#include "Driver/Font.hpp"
#include "Driver/Music.hpp"
#include "Driver/Sound.hpp"

#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <glad/glad.h>
#include <switch.h>
#include <switch/display/native_window.h>

#include <iostream>
#include <sstream>
#include <deque>
#include <fstream>
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
	const auto* platform = static_cast<const SuperHaxagon::Platform*>(userParam);
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

	std::unique_ptr<Music> createMusic(const std::string& path);
	std::unique_ptr<Sound> createSound(const std::string& path);
	std::unique_ptr<Font> createFont(const Platform& platform, const std::string& path, int size, float* z, std::shared_ptr<RenderTarget<VertexUV>>& surface);

	struct Platform::PlatformData {
		bool initEGL(const Platform& platform) {
			eglInitialize(display, nullptr, nullptr);

			if (eglBindAPI(EGL_OPENGL_API) == EGL_FALSE) {
				platform.message(Dbg::FATAL, "api", "error " + std::to_string(eglGetError()));
				eglTerminate(display);
				return false;
			}

			EGLConfig config;
			EGLint numConfigs;

			eglChooseConfig(display, FRAMEBUFFER_ATTRIBUTE_LIST, &config, 1, &numConfigs);
			if (numConfigs == 0) {
				platform.message(Dbg::FATAL, "config", "error " + std::to_string(eglGetError()));
				eglTerminate(display);
				return false;
			}

			surface = eglCreateWindowSurface(display, config, reinterpret_cast<EGLNativeWindowType>(window), nullptr);
			if (!surface) {
				platform.message(Dbg::FATAL, "surface", "error " + std::to_string(eglGetError()));
				eglTerminate(display);
				return false;
			}

			context = eglCreateContext(display, config, EGL_NO_CONTEXT, CONTEXT_ATTRIBUTE_LIST);
			if (!context)
			{
				platform.message(Dbg::FATAL, "context", "error " + std::to_string(eglGetError()));
				eglDestroySurface(display, surface);
				eglTerminate(display);
				return false;
			}

			eglMakeCurrent(display, surface, surface, context);
			return true;
		}

		void addRenderTarget(std::shared_ptr<RenderTarget<Vertex>>& target) {
			targetVertex.emplace_back(target);
		}

		void addRenderTarget(std::shared_ptr<RenderTarget<VertexUV>>& target) {
			targetVertexUV.emplace_back(target);
		}

		bool debugConsole = false;
		bool loaded = false;

		unsigned int width = 1280;
		unsigned int height = 720;

		float z = 0.0f;
		PadState pad{};

		std::shared_ptr<RenderTarget<Vertex>> opaque;
		std::shared_ptr<RenderTarget<Vertex>> transparent;

		NWindow* window;
		EGLDisplay display;
		EGLContext context{};
		EGLSurface surface{};

		std::ofstream console;
		std::deque<std::pair<Dbg, std::string>> messages{};
		std::deque<std::shared_ptr<RenderTarget<Vertex>>> targetVertex{};
		std::deque<std::shared_ptr<RenderTarget<VertexUV>>> targetVertexUV{};

		std::vector<std::pair<SoundEffect, Mix_Chunk*>> sfxBuffers{};
		Mix_Music* musicBuffer{};
	};

	template<class T>
	void render(const Platform& platform, std::deque<std::shared_ptr<RenderTarget<T>>> targets, bool transparent) {
		for (const auto& target : targets) {
			if (target->isTransparent() != transparent) continue;
			target->draw(platform);
		}
	}

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

		_plat->window = nwindowGetDefault();
		_plat->display = eglGetDisplay(EGL_DEFAULT_DISPLAY);

		message(Dbg::INFO, "platform", "booting");
		message(Dbg::INFO, "platform", Mix_GetError());

		// Create window as 1080p, but _width and _height start at 720p.
		// Window is then cropped.
		nwindowSetDimensions(_plat->window, 1920, 1080);
		nwindowSetCrop(_plat->window, 0, 0, _plat->width, _plat->height);

		if (!_plat->display) {
			message(Dbg::FATAL, "display", "error " + std::to_string(eglGetError()));
			return;
		}

		if (!_plat->initEGL(*this)) {
			message(Dbg::FATAL, "egl", "there was a fatal error creating an opengl context");
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
		glViewport(0, 1080 - _plat->height, _plat->width, _plat->height);

		_plat->opaque = std::make_shared<RenderTarget<Vertex>>(*this, false, vertex_shader, fragment_shader, "platform opaque");
		_plat->transparent = std::make_shared<RenderTarget<Vertex>>(*this, true, vertex_shader, fragment_shader, "platform transparent");
		_plat->addRenderTarget(_plat->opaque);
		_plat->addRenderTarget(_plat->transparent);

		_plat->loaded = true;

		message(Dbg::INFO, "platform",  "opengl ok");
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
		_plat->addRenderTarget(fontSurface);
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

	Point Platform::getScreenDim() const {
		return Point{static_cast<float>(_plat->width), static_cast<float>(_plat->height)};
	}

	void Platform::screenBegin() const {
		_plat->z = 0.0f;
		glClearColor(0.2f, 0.3f, 0.8f, 1.0f);
		glClearDepth(0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	// Do nothing since we don't have two screens
	void Platform::screenSwap() {}

	void Platform::screenFinalize() const {
		// Want to render opaque first, then transparent
		render(*this, _plat->targetVertex, false);
		render(*this, _plat->targetVertexUV, false);
		render(*this, _plat->targetVertex, true);
		render(*this, _plat->targetVertexUV, true);

		// Present
		eglSwapBuffers(_plat->display, _plat->surface);
	}

	void Platform::drawPoly(const Color& color, const std::vector<Point>& points) const {
		const auto z = _plat->z;
		_plat->z += Z_STEP;

		auto& buffer = color.a == 0xFF || color.a == 0 ? _plat->opaque : _plat->transparent;
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
	
	std::unique_ptr<Twist> Platform::getTwister() {
		// ALSO a shitty way to do this, but it's the best I got.
		const auto a = new std::seed_seq{ svcGetSystemTick() };
		return std::make_unique<Twist>(
			std::unique_ptr<std::seed_seq>(a)
		);
	}
	
	void Platform::shutdown() {
		if (_plat->display) {
			eglMakeCurrent(_plat->display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);

			if (_plat->context) {
				eglDestroyContext(_plat->display, _plat->context);
			}

			if (_plat->surface) {
				eglDestroySurface(_plat->display, _plat->surface);
			}

			eglTerminate(_plat->display);
		}

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
