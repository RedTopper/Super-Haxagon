#include <switch.h>
#include <sys/stat.h>
#include <iostream>
#include <sstream>

#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <glad/glad.h>

#include "Driver/Switch/FontSwitch.hpp"
#include "Driver/Switch/PlatformSwitch.hpp"

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

/**
 * Helper function used for debugging OpenGL
 */
static void callback(GLenum source, const GLenum type, GLuint id, const GLenum severity, GLsizei, const GLchar* message, const void* userParam) {
	// WCGW?
	auto* platform = const_cast<SuperHaxagon::PlatformSwitch*>(reinterpret_cast<const SuperHaxagon::PlatformSwitch*>(userParam));
	auto error = type == GL_DEBUG_TYPE_ERROR;
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
		romfsInit();
		mkdir("sdmc:/switch", 0777);
		mkdir("sdmc:/switch/SuperHaxagon", 0777);

		_window = nwindowGetDefault();
		_console = std::ofstream("sdmc:/switch/SuperHaxagon/out.log");
		_display = eglGetDisplay(EGL_DEFAULT_DISPLAY);

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
		romfsExit();
		PlatformSwitch::message(SuperHaxagon::Dbg::INFO, "platform", "shutdown ok");
	}

	bool PlatformSwitch::loop() {
		if (!_loaded) return false;

		double width = _width;
		double height = _height;
		switch (appletGetOperationMode()) {
			default:
			case AppletOperationMode_Handheld:
				_width = 1280;
				_height = 720;
				break;
			case AppletOperationMode_Docked:
				_width = 1920;
				_height = 1080;
				break;
		}

		if (width != _width || height != _height) {
			nwindowSetCrop(_window, 0, 0, _width, _height);
			glViewport(0, 1080 - _height, _width, _height);
		}

		return appletMainLoop();
	}

	double PlatformSwitch::getDilation() {
		return 1.0;
	}

	void PlatformSwitch::playSFX(Audio& audio) {

	}

	void PlatformSwitch::playBGM(Audio& audio) {

	}

	void PlatformSwitch::stopBGM() {

	}

	double PlatformSwitch::getBgmVelocity() {
		return 0;
	}

	std::string PlatformSwitch::getPath(const std::string& partial) {
		return "sdmc:/switch/SuperHaxagon" + partial;
	}

	std::string PlatformSwitch::getPathRom(const std::string& partial) {
		return "romfs:" + partial;
	}

	std::unique_ptr<Audio> PlatformSwitch::loadAudio(const std::string& path, Stream stream) {
		return nullptr;
	}

	std::unique_ptr<Font> PlatformSwitch::loadFont(const std::string& path, int size) {
		return std::make_unique<FontSwitch>(*this, path, size);
	}

	std::string PlatformSwitch::getButtonName(const Buttons button) {
		if (button.back) return "B";
		if (button.select) return "A";
		if (button.left) return "LEFT";
		if (button.right) return "RIGHT";
		if (button.quit) return "PLUS";
		return "?";
	}

	Buttons PlatformSwitch::getPressed() {
		hidScanInput();
		u64 kDown = hidKeysDown(CONTROLLER_P1_AUTO);
		u64 kPressed = hidKeysHeld(CONTROLLER_P1_AUTO);
		Buttons buttons{};
		buttons.select = kDown & KEY_A;
		buttons.back = kDown & KEY_B;
		buttons.quit = kDown & KEY_PLUS;
		buttons.left = kPressed & (KEY_L | KEY_ZL | KEY_DLEFT);
		buttons.right = kPressed & (KEY_R | KEY_ZR | KEY_X);
		return buttons;
	}

	Point PlatformSwitch::getScreenDim() const {
		return Point{static_cast<double>(_width), static_cast<double>(_height)};
	}

	void PlatformSwitch::screenBegin() {
		_z = 0.0f;
		glClearColor(0.2f, 0.3f, 0.8f, 1.0f);
		glClearDepth(0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void PlatformSwitch::screenSwap() {
		// Nothing to do, the switch only has one screen
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

	void PlatformSwitch::drawRect(const Color& color, const Point& point, const Point& size) {
		const auto z = getAndIncrementZ();
		auto& buffer = color.a == 0xFF || color.a == 0 ? _opaque : _transparent;
		buffer->insert({point, color, z});
		buffer->insert({{point.x + size.x, point.y}, color, z});
		buffer->insert({{point.x, point.y + size.y}, color, z});
		buffer->insert({{point.x + size.x, point.y + size.y}, color, z});

		buffer->reference(0);
		buffer->reference(1);
		buffer->reference(2);
		buffer->reference(1);
		buffer->reference(2);
		buffer->reference(3);

		buffer->advance(4);
	}

	void PlatformSwitch::drawTriangle(const Color& color, const std::array<Point, 3>& points) {
		auto last = 0;
		const auto z = getAndIncrementZ();
		auto& buffer = color.a == 0xFF || color.a == 0 ? _opaque : _transparent;
		for (const auto& point : points) {
			buffer->insert({point, color, z});
			buffer->reference(last++);
		}

		buffer->advance(last);
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
				hidScanInput();
				const auto kDown = hidKeysDown(CONTROLLER_P1_AUTO);
				if (kDown & KEY_PLUS) break;
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
		static const EGLint framebufferAttributeList[] = {
			EGL_RENDERABLE_TYPE, EGL_OPENGL_BIT,
			EGL_RED_SIZE,       8,
			EGL_GREEN_SIZE,     8,
			EGL_BLUE_SIZE,      8,
			EGL_ALPHA_SIZE,     8,
			EGL_DEPTH_SIZE,     24,
			EGL_STENCIL_SIZE,   8,
			EGL_NONE
		};

		eglChooseConfig(_display, framebufferAttributeList, &config, 1, &numConfigs);
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

		static const EGLint contextAttributeList[] = {
			EGL_CONTEXT_OPENGL_PROFILE_MASK_KHR, EGL_CONTEXT_OPENGL_CORE_PROFILE_BIT_KHR,
			EGL_CONTEXT_MAJOR_VERSION_KHR, 4,
			EGL_CONTEXT_MINOR_VERSION_KHR, 3,
			EGL_NONE
		};

		_context = eglCreateContext(_display, config, EGL_NO_CONTEXT, contextAttributeList);
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
