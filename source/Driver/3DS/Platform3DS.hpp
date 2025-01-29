#ifndef SUPER_HAXAGON_PLATFORM_3DS_HPP
#define SUPER_HAXAGON_PLATFORM_3DS_HPP

#include "Core/Platform.hpp"

#include <citro2d.h>

#include <deque>

static const int MAX_TRACKS = 4;

namespace SuperHaxagon {
	class Platform3DS : public Platform {
	public:
		explicit Platform3DS(Dbg dbg);
		Platform3DS(Platform3DS&) = delete;
		~Platform3DS() override;

		bool loop() override;
		float getDilation() override;

		std::string getPath(const std::string& partial, Location location) override;
		std::unique_ptr<AudioLoader> loadAudio(const std::string& partial, Stream stream, Location location) override;
		std::unique_ptr<Font> loadFont(const std::string& partial, int size, Location location) override;

		void playSFX(AudioLoader& audio) override;
		void playBGM(AudioLoader& audio) override;

		std::string getButtonName(const Buttons& button) override;
		Buttons getPressed() override;
		Point getScreenDim() const override;

		void screenBegin() override;
		void screenSwap() override;
		void screenFinalize() override;
		void drawPoly(const Color& color, const std::vector<Point>& points) override;

		std::unique_ptr<Twist> getTwister() override;

		void shutdown() override;
		void message(Dbg dbg, const std::string& where, const std::string& message) override;

	private:
		std::unique_ptr<AudioPlayer> _sfx[MAX_TRACKS]{};

		std::deque<std::pair<Dbg, std::string>> _messages{};

		C3D_RenderTarget* _top = nullptr;
		C3D_RenderTarget* _bot = nullptr;
		C2D_TextBuf _buff;

		bool _drawingOnTop = true;
		float _delta = 0.0f;
		uint64_t _last = 0;
	};
}

#endif //SUPER_HAXAGON_PLATFORM_3DS_HPP
