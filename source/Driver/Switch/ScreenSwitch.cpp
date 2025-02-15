#include "Driver/Screen.hpp"

#include "RenderTarget.hpp"
#include "Driver/Platform.hpp"

#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <glad/glad.h>
#include <switch.h>

#include <deque>
#include <sstream>

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
	const auto* platform = static_cast<const SuperHaxagon::Platform*>(userParam);
	if (!platform) return;
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
	template<class T>
	void render(const Platform& platform, int width, int height, const std::deque<std::shared_ptr<RenderTarget<T>>>& targets, bool transparent) {
		for (const auto& target : targets) {
			if (target->isTransparent() != transparent) continue;
			target->draw(platform, width, height);
		}
	}

	struct Screen::ScreenImpl {
		bool initEGL() {
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

		ScreenImpl(const Platform& platform, VertexStorage& targetVertex, VertexUVStorage& targetVertexUV, float& z) :
				platform(platform),
				targetVertex(targetVertex),
				targetVertexUV(targetVertexUV),
				z(z) {

			platform.message(Dbg::INFO, "screen", "booting");

			window = nwindowGetDefault();
			display = eglGetDisplay(EGL_DEFAULT_DISPLAY);

			// Create window as 1080p, but width and height start at 720p.
			// Window is then cropped.
			nwindowSetDimensions(window, 1920, 1080);
			nwindowSetCrop(window, 0, 0, width, height);

			if (!display) {
				platform.message(Dbg::FATAL, "display", "error " + std::to_string(eglGetError()));
				return;
			}

			if (!initEGL()) {
				platform.message(Dbg::FATAL, "egl", "there was a fatal error creating an opengl context");
				return;
			}

			gladLoadGL();

			glEnable(GL_DEBUG_OUTPUT);
			glEnable(GL_DEPTH_TEST);
			glEnable(GL_FRAMEBUFFER_SRGB);
			glDepthMask(GL_TRUE);
			glDepthFunc(GL_GREATER);
			glDepthRange(0.0f, 1.0f);
			glDebugMessageCallback(callback, &platform);
			glViewport(0, 1080 - height, width, height);

			opaque = std::make_shared<RenderTarget<Vertex>>(platform, false, vertex_shader, fragment_shader, "platform opaque");
			transparent = std::make_shared<RenderTarget<Vertex>>(platform, true, vertex_shader, fragment_shader, "platform transparent");
			targetVertex.emplace_back(opaque);
			targetVertex.emplace_back(transparent);

			loaded = true;

			platform.message(Dbg::INFO, "screen",  "opengl ok");
		}

		~ScreenImpl() {
			if (display) {
				eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);

				if (context) {
					eglDestroyContext(display, context);
				}

				if (surface) {
					eglDestroySurface(display, surface);
				}

				eglTerminate(display);
			}
		}

		void screenFinalize() {
			if (!loaded) return;

			// Want to render opaque first, then transparent
			render(platform, width, height, targetVertex, false);
			render(platform, width, height, targetVertexUV, false);
			render(platform, width, height, targetVertex, true);
			render(platform, width, height, targetVertexUV, true);

			// Present
			eglSwapBuffers(display, surface);
		}

		void drawPoly(const Color& color, const std::vector<Vec2f>& points) {
			if (!loaded) return;

			const auto tz = z;
			z += Z_STEP;

			auto& buffer = color.a == 0xFF || color.a == 0 ? opaque : transparent;
			for (const auto& point : points) {
				buffer->insert({point, color, tz});
			}

			for (size_t i = 1; i < points.size() - 1; i++) {
				buffer->reference(0);
				buffer->reference(i);
				buffer->reference(i + 1);
			}

			buffer->advance(points.size());
		}

		void clear(const Color& color) {
			if (!loaded) return;

			float r = static_cast<float>(color.r) / 255.0f;
			float g = static_cast<float>(color.g) / 255.0f;
			float b = static_cast<float>(color.b) / 255.0f;
			glClearColor(r, g, b, 1.0f);
			glClearDepth(0.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		}

		Vec2f getScreenDim() {
			if (!loaded) return {1280.0f, 720.0f};

			const auto curWidth = width;
			const auto curHeight = height;
			switch (appletGetOperationMode()) {
				default:
				case AppletOperationMode_Handheld:
					width = 1280;
					height = 720;
					break;
				case AppletOperationMode_Console:
					width = 1920;
					height = 1080;
					break;
			}

			if (curWidth != width || curHeight != height) {
				nwindowSetCrop(window, 0, 0, width, height);
				glViewport(0, 1080 - height, width, height);
			}

			return Vec2f{static_cast<float>(width), static_cast<float>(height)};
		}

		std::shared_ptr<RenderTarget<Vertex>> opaque;
		std::shared_ptr<RenderTarget<Vertex>> transparent;

		NWindow* window = nullptr;
		EGLDisplay display = nullptr;
		EGLContext context = nullptr;
		EGLSurface surface = nullptr;
		unsigned int width = 1280;
		unsigned int height = 720;
		bool loaded = false;

		// Shared with platform
		const Platform& platform;
		VertexStorage& targetVertex;
		VertexUVStorage& targetVertexUV;
		float& z;
	};

	Screen::Screen(std::unique_ptr<ScreenImpl> impl) : _impl(std::move(impl)) {}

	Screen::~Screen() = default;

	Vec2f Screen::getScreenDim() const {
		return _impl->getScreenDim();
	}

	void Screen::screenBegin() const {
		_impl->z = 0.0f;
	}

	// Nothing to do, only one screen.
	void Screen::screenSwitch() const {}

	void Screen::screenFinalize() const {
		_impl->screenFinalize();
	}

	void Screen::drawPoly(const Color& color, const std::vector<Vec2f>& points) const {
		_impl->drawPoly(color, points);
	}

	void Screen::clear(const Color& color) const {
		_impl->clear(color);
	}

	std::unique_ptr<Screen> createScreen(const Platform& platform, VertexStorage& targetVertex, VertexUVStorage& targetVertexUV, float& z) {
		return std::make_unique<Screen>(std::make_unique<Screen::ScreenImpl>(platform, targetVertex, targetVertexUV, z));
	}
}
