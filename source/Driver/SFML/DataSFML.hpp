#ifndef DATA_SFML_HPP
#define DATA_SFML_HPP

#include "Driver/Platform.hpp"

#include <SFML/Graphics.hpp>

#include <memory>
#include <deque>

namespace sf {
	class VideoMode;
	class SoundBuffer;
	class Music;
}

namespace SuperHaxagon {
	class Sound;

	std::unique_ptr<Platform::PlatformData> createPlatform(sf::VideoMode video, const std::string& sdmc, const std::string& romfs, bool backslash);

	struct Platform::PlatformData {
		bool loaded = false;
		bool focus = true;
		bool backslash = false;
		sf::Clock clock{};
		std::unique_ptr<sf::RenderWindow> window{};
		std::deque<std::unique_ptr<Sound>> sfx{};
		std::vector<std::pair<SoundEffect, sf::SoundBuffer>> sfxBuffers{};
		std::unique_ptr<sf::Music> bgmAudio = std::make_unique<sf::Music>();
		std::string romfs;
		std::string sdmc;
	};
}

#endif //DATA_SFML_HPP
