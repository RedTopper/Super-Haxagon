#include <switch.h>
#include <sys/stat.h>
#include <iostream>
#include <sstream>

#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <glad/glad.h>

#include "Driver/Switch/FontSwitch.hpp"
#include "Driver/Switch/PlatformSwitch.hpp"

static constexpr int BUFFER_RESIZE_STEP = 500;
static constexpr float Z_STEP = 0.0001f;

static const char * vertex_shader = R"text(
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

static const char * fragment_shader = R"text(
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
	out << message << std::endl;
	platform->message(error ? SuperHaxagon::Dbg::FATAL : SuperHaxagon::Dbg::INFO, "switch,opengl", out.str());
}

/**
 * Helper function used to compile shaders
 */
static GLuint compile(GLenum type, const char* source, SuperHaxagon::Platform& platform) {
	GLint success;
	GLchar msg[512];

	GLuint handle = glCreateShader(type);
	if (!handle) {
		platform.message(SuperHaxagon::Dbg::INFO, "switch,shader",  "failed to create shader");
		return 0;
	}

	glShaderSource(handle, 1, &source, nullptr);
	glCompileShader(handle);
	glGetShaderiv(handle, GL_COMPILE_STATUS, &success);

	if (!success) {
		glGetShaderInfoLog(handle, sizeof(msg), nullptr, msg);
		platform.message(SuperHaxagon::Dbg::INFO, "switch,shader",  msg);
		glDeleteShader(handle);
		return 0;
	}

	return handle;
}

/**
 * Helper function used to resize OpenGL buffers
 */
template<typename T>
static void resize(GLuint type, unsigned int& size, std::vector<T>& vector, SuperHaxagon::PlatformSwitch& platform, const std::string& label) {
	if (size < vector.size() || size == 0) {
		if (size == 0) size = BUFFER_RESIZE_STEP;
		while (size < vector.size()) size *= 2;
		platform.message( SuperHaxagon::Dbg::INFO, "switch", "resized " + label + " to " + std::to_string(size));
		glBufferData(type, size * sizeof(T), vector.data(), GL_DYNAMIC_DRAW);
	} else {
		glBufferSubData(type, 0, size * sizeof(T), vector.data());
	}
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
			PlatformSwitch::message(Dbg::FATAL, "switch,display", "error " + std::to_string(eglGetError()));
			return;
		}

		if (!initEGL()) {
			PlatformSwitch::message(Dbg::FATAL, "switch", "there was a fatal error creating an opengl context");
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

		const auto vs = compile(GL_VERTEX_SHADER, vertex_shader, *this);
		const auto fs = compile(GL_FRAGMENT_SHADER, fragment_shader, *this);

		_program = glCreateProgram();
		glAttachShader(_program, vs);
		glAttachShader(_program, fs);
		glLinkProgram(_program);
		glUseProgram(_program);

		glDeleteShader(vs);
		glDeleteShader(fs);

		glGenVertexArrays(1, &_vao);
		glBindVertexArray(_vao);
		glGenBuffers(1, &_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, _vbo);
		glGenBuffers(1, &_ibo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ibo);

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(0, 2, GL_DOUBLE, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, p));
		glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex), (void*)offsetof(Vertex, c));
		glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, z));

		PlatformSwitch::message(Dbg::INFO, "switch",  "opengl ok");
		_loaded = true;
	}

	PlatformSwitch::~PlatformSwitch() {
		romfsExit();
		PlatformSwitch::message(SuperHaxagon::Dbg::INFO, "switch", "shutdown ok");
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
		_vertices.clear();
		_indicesOpaque.clear();
		_indicesTransparent.clear();
		_iboLastIndex = 0;
		_z = 0.0f;
		glClearColor(0.2f, 0.3f, 0.8f, 1.0f);
		glClearDepth(0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		const auto s_width = glGetUniformLocation(_program, "s_width");
		const auto s_height = glGetUniformLocation(_program, "s_height");
		glUniform1f(s_width, static_cast<float>(_width));
		glUniform1f(s_height, static_cast<float>(_height));
	}

	void PlatformSwitch::screenSwap() {
		// Nothing to do, the switch only has one screen
	}

	void PlatformSwitch::screenFinalize() {
		resize(GL_ARRAY_BUFFER, _vboBufferSize, _vertices, *this, "vertices");

		// Need to reverse opaque objects so the gpu renders transparency correctly
		std::reverse(_indicesOpaque.begin(),_indicesOpaque.end());
		resize(GL_ELEMENT_ARRAY_BUFFER, _iboBufferSize, _indicesOpaque, *this, "opaque indices");
		glDrawElements(GL_TRIANGLES, _indicesOpaque.size(), GL_UNSIGNED_INT, nullptr);

		glEnable(GL_BLEND);
		glDepthMask(GL_FALSE);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		resize(GL_ELEMENT_ARRAY_BUFFER, _iboBufferSize, _indicesTransparent, *this, "transparent indices");
		glDrawElements(GL_TRIANGLES, _indicesTransparent.size(), GL_UNSIGNED_INT, nullptr);
		glDepthMask(GL_TRUE);
		glDisable(GL_BLEND);

		eglSwapBuffers(_display, _surface);
	}

	void PlatformSwitch::drawRect(const Color& color, const Point& point, const Point& size) {
		_vertices.push_back({point, color, _z});
		_vertices.push_back({{point.x + size.x, point.y}, color, _z});
		_vertices.push_back({{point.x, point.y + size.y}, color, _z});
		_vertices.push_back({{point.x + size.x, point.y + size.y}, color, _z});

		auto& buffer = color.a == 0xFF || color.a == 0 ? _indicesOpaque : _indicesTransparent;
		buffer.push_back(_iboLastIndex);
		buffer.push_back(_iboLastIndex + 1);
		buffer.push_back(_iboLastIndex + 2);
		buffer.push_back(_iboLastIndex + 1);
		buffer.push_back(_iboLastIndex + 2);
		buffer.push_back(_iboLastIndex + 3);

		_iboLastIndex += 4;
		_z += Z_STEP;
	}

	void PlatformSwitch::drawTriangle(const Color& color, const std::array<Point, 3>& points) {
		unsigned int last = _iboLastIndex;
		auto& buffer = color.a == 0xFF || color.a == 0 ? _indicesOpaque : _indicesTransparent;
		for (const auto& point : points) {
			_vertices.push_back({point, color, _z});
			buffer.push_back(last++);
		}

		_iboLastIndex += 3;
		_z += Z_STEP;
	}
	
	std::unique_ptr<Twist> PlatformSwitch::getTwister() {
		// ALSO a shitty way to do this but it's the best I got.
		const auto a = new std::seed_seq{ svcGetSystemTick() };
		return std::make_unique<Twist>(
			std::unique_ptr<std::seed_seq>(a)
		);
	}
	
	void PlatformSwitch::shutdown() {
		glDeleteBuffers(1, &_vbo);
		glDeleteBuffers(1, &_ibo);
		glDeleteVertexArrays(1, &_vao);
		glDeleteProgram(_program);

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
			message(Dbg::FATAL, "switch,api", "error " + std::to_string(eglGetError()));
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
			message(Dbg::FATAL, "switch,config", "error " + std::to_string(eglGetError()));
			eglTerminate(_display);
			return false;
		}

		_surface = eglCreateWindowSurface(_display, config, reinterpret_cast<EGLNativeWindowType>(_window), nullptr);
		if (!_surface) {
			message(Dbg::FATAL, "switch,surface", "error " + std::to_string(eglGetError()));
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
			message(Dbg::FATAL, "switch,context", "error " + std::to_string(eglGetError()));
			eglDestroySurface(_display, _surface);
			eglTerminate(_display);
			return false;
		}

		eglMakeCurrent(_display, _surface, _surface, _context);
		return true;
	}
}
