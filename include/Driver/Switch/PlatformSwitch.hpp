#ifndef SUPER_HAXAGON_PLATFORM_SWITCH_HPP
#define SUPER_HAXAGON_PLATFORM_SWITCH_HPP

#include "Core/Platform.hpp"

#include "RenderTarget.hpp"

#include <EGL/egl.h>
#include <switch.h>
#include <switch/display/native_window.h>

#include <deque>
#include <fstream>

namespace SuperHaxagon {
	class Font;

	class PlatformSwitch : public Platform {
	public:
		explicit PlatformSwitch(Dbg dbg);
		PlatformSwitch(PlatformSwitch&) = delete;
		~PlatformSwitch() override;

		bool loop() override;
		float getDilation() override;

		std::string getPath(const std::string& partial, Location location) override;
		std::unique_ptr<Font> loadFont(const std::string& partial, int size) override;
		std::unique_ptr<Audio> loadAudio(const std::string& partial, Stream stream, Location location) override;

		void playSFX(Audio& audio) override;
		void playBGM(Audio& audio) override;

		std::string getButtonName(const Buttons& button) override;
		Buttons getPressed() override;
		Point getScreenDim() const override;

		void screenBegin() override;
		void screenFinalize() override;
		void drawPoly(const Color& color, const std::vector<Point>& points) override;

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
		PadState _pad{};

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
