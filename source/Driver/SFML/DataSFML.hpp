#ifndef DATA_SFML_HPP
#define DATA_SFML_HPP

#include "Driver/Platform.hpp"

#include <SFML/Graphics.hpp>

#include <memory>
#include <deque>

namespace sf {
	class VideoMode;
}

namespace SuperHaxagon {
	std::unique_ptr<Screen> createScreen(sf::RenderWindow& window);
	std::unique_ptr<Platform::PlatformData> createPlatform(sf::VideoMode video, const std::string& sdmc, const std::string& romfs, bool backslash);

	struct Platform::PlatformData {
		bool loaded = false;
		bool focus = true;
		bool backslash = false;
		sf::Clock clock{};
		std::unique_ptr<sf::RenderWindow> window{};
		std::string romfs;
		std::string sdmc;
	};
}

#endif //DATA_SFML_HPP
