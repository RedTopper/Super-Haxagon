#ifndef SUPER_HAXAGON_PLATFORM_SWITCH_HPP
#define SUPER_HAXAGON_PLATFORM_SWITCH_HPP

#include <fstream>
#include <deque>
#include <array>
#include <EGL/egl.h>
#include <switch/display/native_window.h>

#include "Driver/Platform.hpp"
#include "Driver/Player.hpp"
#include "RenderTarget.hpp"

namespace SuperHaxagon {
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
		double getBGMTime() override;

		std::string getButtonName(const Buttons& button) override;
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

		float getAndIncrementZ();
		void addRenderTarget(std::shared_ptr<RenderTarget<Vertex>>& target) {_targetVertex.emplace_back(target);}
		void addRenderTarget(std::shared_ptr<RenderTarget<VertexUV>>& target) {_targetVertexUV.emplace_back(target);}

	private:
		bool initEGL();

		template<class T>
		void render(std::deque<std::shared_ptr<RenderTarget<T>>> targets, bool transparent);

		bool _loaded = false;

		unsigned int _width = 1280;
		unsigned int _height = 720;

		float _z = 0.0f;

		std::shared_ptr<RenderTarget<Vertex>> _opaque;
		std::shared_ptr<RenderTarget<Vertex>> _transparent;

		NWindow* _window;
		EGLDisplay _display;
		EGLContext _context{};
		EGLSurface _surface{};

		std::ofstream _console;
		std::deque<std::pair<Dbg, std::string>> _messages{};
		std::deque<std::shared_ptr<RenderTarget<Vertex>>> _targetVertex{};
		std::deque<std::shared_ptr<RenderTarget<VertexUV>>> _targetVertexUV{};
	};
}

#endif //SUPER_HAXAGON_PLATFORM_SWITCH_HPP
