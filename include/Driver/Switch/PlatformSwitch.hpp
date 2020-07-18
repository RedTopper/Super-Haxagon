#ifndef SUPER_HAXAGON_PLATFORM_SWITCH_HPP
#define SUPER_HAXAGON_PLATFORM_SWITCH_HPP

#include <fstream>
#include <deque>
#include <array>
#include <switch/display/native_window.h>
#include <glad/glad.h>
#include <EGL/egl.h>

#include "Driver/Platform.hpp"

namespace SuperHaxagon {
	struct Vertex {
		Point p;
		Color c;
		float z;
	};

	class PlatformSwitch : public Platform {
	public:
		explicit PlatformSwitch(Dbg dbg);
		PlatformSwitch(PlatformSwitch&) = delete;
		~PlatformSwitch() override;

		bool loop() override;
		double getDilation() override;

		std::string getPath(const std::string& partial) override;
		std::string getPathRom(const std::string& partial) override;
		std::unique_ptr<Font> loadFont(const std::string& path, int size) override;
		std::unique_ptr<Audio> loadAudio(const std::string& path, Stream stream) override;

		void playSFX(Audio& audio) override;
		void playBGM(Audio& audio) override;
		void stopBGM() override;
		double getBgmVelocity() override;

		std::string getButtonName(Buttons button) override;
		Buttons getPressed() override;
		Point getScreenDim() const override;

		void screenBegin() override;
		void screenSwap() override;
		void screenFinalize() override;
		void drawRect(const Color& color, const Point& point, const Point& size) override;
		void drawTriangle(const Color& color, const std::array<Point, 3>& points) override;

		std::unique_ptr<Twist> getTwister() override;

		void shutdown() override;
		void message(Dbg dbg, const std::string& where, const std::string& message) override;

	private:
		bool initEGL();

		bool _loaded = false;

		unsigned int _iboLastIndex = 0;
		unsigned int _vboBufferSize = 0;
		unsigned int _iboBufferSize = 0;

		unsigned int _width = 1280;
		unsigned int _height = 720;

		float _z;

		NWindow* _window;
		EGLDisplay _display;
		EGLContext _context{};
		EGLSurface _surface{};

		GLuint _program{};
		GLuint _vao{};
		GLuint _vbo{};
		GLuint _ibo{};

		std::vector<Vertex> _vertices;
		std::vector<unsigned int> _indicesTransparent;
		std::vector<unsigned int> _indicesOpaque;

		std::ofstream _console;
		std::deque<std::pair<Dbg, std::string>> _messages{};
	};
}

#endif //SUPER_HAXAGON_PLATFORM_SWITCH_HPP
