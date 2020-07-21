#ifndef SUPER_HAXAGON_PLATFORM_3DS_HPP
#define SUPER_HAXAGON_PLATFORM_3DS_HPP

#include <citro2d.h>
#include <deque>

#include "Driver/Audio.hpp"
#include "Driver/Platform.hpp"
#include "Driver/Player.hpp"

static const int MAX_TRACKS = 4;

namespace SuperHaxagon {
	class Platform3DS : public Platform {
	public:
		explicit Platform3DS(Dbg dbg);
		Platform3DS(Platform3DS&) = delete;
		~Platform3DS() override;

		bool loop() override;
		double getDilation() override;

		std::string getPath(const std::string& partial) override;
		std::string getPathRom(const std::string& partial) override;
		std::unique_ptr<Audio> loadAudio(const std::string& path, SuperHaxagon::Stream stream) override;
		std::unique_ptr<Font> loadFont(const std::string& path, int size) override;

		void playSFX(Audio& audio) override;
		void playBGM(Audio& audio) override;
		double getBgmVelocity() override;

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

	private:
		std::unique_ptr<Player> _sfx[MAX_TRACKS]{};

		std::deque<std::pair<Dbg, std::string>> _messages{};

		C3D_RenderTarget* _top = nullptr;
		C3D_RenderTarget* _bot = nullptr;
		C2D_TextBuf _buff;

		bool _drawingOnTop = true;
	};
}

#endif //SUPER_HAXAGON_PLATFORM_3DS_HPP
