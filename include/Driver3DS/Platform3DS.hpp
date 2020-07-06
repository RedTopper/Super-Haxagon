#ifndef SUPER_HAXAGON_PLATFORM_3DS_HPP
#define SUPER_HAXAGON_PLATFORM_3DS_HPP

#include <3ds.h>

#include "Driver/Platform.hpp"

static const int MAX_TRACKS = 4;

namespace SuperHaxagon {
	class Player;

	class Platform3DS : public Platform {
	public:
		Platform3DS();
		~Platform3DS();

		bool loop() override;

		std::string getPath(const std::string& partial) override;
		std::string getPathRom(const std::string& partial) override;
		std::unique_ptr<Audio> loadAudio(const std::string& path) override;
		std::unique_ptr<Font> loadFont(const std::string& path, int size) override;

		void playSFX(Audio& audio) override;
		void playBGM(Audio& audio) override;
		void stopBGM() override;

		Buttons getPressed() override;
		Point getScreenDim() const override;

		void drawRect(const Color& color, const Point& point, const Point& size) const override;
		void drawTriangle(const Color& color, const std::array<Point, 3>& points) const override;

		std::unique_ptr<Twist> getTwister() override;

	private:
		std::unique_ptr<Player> sfx[MAX_TRACKS];
		std::unique_ptr<Player> bgm;
	};
}

#endif //SUPER_HAXAGON_PLATFORM_3DS_HPP
