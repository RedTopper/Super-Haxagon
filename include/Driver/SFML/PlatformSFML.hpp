#ifndef SUPER_HAXAGON_PLATFORM_SFML_HPP
#define SUPER_HAXAGON_PLATFORM_SFML_HPP

#include "Core/Platform.hpp"

#include <SFML/Graphics.hpp>

#include <deque>

namespace SuperHaxagon {
	class Audio;
	class PlatformSFML : public Platform {
	public:
		PlatformSFML(Dbg dbg, sf::VideoMode video);
		~PlatformSFML() override;

		bool loop() override;
		float getDilation() override;

		std::string getPath(const std::string& partial, Location location) override = 0;
		std::unique_ptr<Audio> loadAudio(const std::string& partial, Stream stream, Location location) override;
		std::unique_ptr<Font> loadFont(const std::string& partial, int size) override;

		void playSFX(Audio& audio) override;
		void playBGM(Audio& audio) override;

		std::string getButtonName(const Buttons& button) override;
		Buttons getPressed() override;
		Point getScreenDim() const override;

		void screenBegin() override;
		void screenFinalize() override;
		void drawPoly(const Color& color, const std::vector<Point>& points) override;

		std::unique_ptr<Twist> getTwister() override = 0;

		void shutdown() override = 0;
		void message(Dbg dbg, const std::string& where, const std::string& message) override = 0;

		sf::RenderWindow& getWindow() const {return *_window;}

	private:
		bool _loaded = false;
		bool _focus = true;
		float _delta = 0.0;
		sf::Clock _clock;
		std::unique_ptr<sf::RenderWindow> _window;
		std::deque<std::unique_ptr<Player>> _sfx;
	};
}

#endif //SUPER_HAXAGON_PLATFORM_SFML_HPP
