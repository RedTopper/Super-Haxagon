#include "Driver/Platform.hpp"

#include "DataSFML.hpp"
#include "Core/Configuration.hpp"
#include "Core/Structs.hpp"
#include "Driver/Font.hpp"
#include "Driver/Music.hpp"
#include "Driver/Screen.hpp"
#include "Driver/Sound.hpp"

#include <SFML/Window.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>

#include <string>
#include <deque>

namespace SuperHaxagon {
	std::unique_ptr<Music> createMusic(std::unique_ptr<sf::Music> music);
	std::unique_ptr<Sound> createSound(const std::string& path);
	std::unique_ptr<Font> createFont(sf::RenderWindow& renderWindow, const std::string& path, int size);

	std::unique_ptr<Platform::PlatformData> createPlatform(sf::VideoMode video, const std::string& sdmc, const std::string& romfs, const bool backslash) {
		auto plat = std::make_unique<Platform::PlatformData>();

		plat->sdmc = sdmc;
		plat->romfs = romfs;
		plat->backslash = backslash;

		plat->clock.restart();

		sf::ContextSettings settings;
		settings.antialiasingLevel = 8;

		// Set the window name to "Super Haxagon" first, to match the .desktop file,
		// then add the version after the window has loaded.
		plat->window = std::make_unique<sf::RenderWindow>(video, "Super Haxagon", sf::Style::Default, settings);

		plat->window->setVerticalSyncEnabled(true);
		plat->window->display();
		plat->window->setTitle(std::string("Super Haxagon (") + VERSION + ")");
		plat->loaded = true;

		return plat;
	}

	bool Platform::loop() {
		const auto throttle = sf::milliseconds(1);
		sf::sleep(throttle);
		_delta = _plat->clock.getElapsedTime().asSeconds();
		_plat->clock.restart();
		sf::Event event{};
		while (_plat->window->pollEvent(event)) {
			if (event.type == sf::Event::Closed) _plat->window->close();
			if (event.type == sf::Event::GainedFocus) _plat->focus = true;
			if (event.type == sf::Event::LostFocus) _plat->focus = false;
			if (event.type == sf::Event::Resized) {
				const auto width = event.size.width > 400 ? event.size.width : 400;
				const auto height = event.size.height > 240 ? event.size.height : 240;

				if (width != event.size.width || height != event.size.height) {
					_plat->window->setSize({ width, height });
				}
				
				sf::FloatRect visibleArea(
					0.0f, 
					0.0f, 
					static_cast<float>(width), 
					static_cast<float>(height)
				);

				_plat->window->setView(sf::View(visibleArea));
			}
		}
		return _plat->loaded && _plat->window->isOpen();
	}

	float Platform::getDilation() const {
		// The game was originally designed with 60FPS in mind
		return _delta / (1.0f / 60.0f);
	}

	std::string Platform::getPath(const std::string& partial, const Location location) const {
		auto path = partial;

		if (_plat->backslash) std::replace(path.begin(), path.end(), '/', '\\');

		switch (location) {
			case Location::ROM:
				return _plat->romfs + path;
			case Location::USER:
				return _plat->sdmc + path;
		}

		return "";
	}

	std::unique_ptr<std::istream> Platform::openFile(const std::string& partial, const Location location) const {
		return std::make_unique<std::ifstream>(getPath(partial, location), std::ios::in | std::ios::binary);
	}

	std::unique_ptr<Font> Platform::loadFont(const int size) const {
		return createFont(*_plat->window, getPath("/bump-it-up.ttf", Location::ROM), size);
	}

	std::unique_ptr<Sound> Platform::loadSound(const std::string& base) const {
		return createSound(getPath(base, Location::ROM) + ".wav");
	}

	std::unique_ptr<Music> Platform::loadMusic(const std::string& base, const Location location) const {
		auto music = std::make_unique<sf::Music>();
		auto loaded = music->openFromFile(getPath(base, location) + ".ogg");
		if (!loaded) return nullptr;

		return createMusic(std::move(music));
	}

	std::string Platform::getButtonName(const Buttons& button) {
		if (button.back) return "ESC";
		if (button.select) return "ENTER";
		if (button.left) return "LEFT";
		if (button.right) return "RIGHT";
		if (button.quit) return "DELETE";
		return "?";
	}

	Buttons Platform::getPressed() const {
		Buttons buttons{};
		if (!_plat->focus) return buttons;
		buttons.select = sf::Keyboard::isKeyPressed(sf::Keyboard::Enter);
		buttons.back = sf::Keyboard::isKeyPressed(sf::Keyboard::Escape);
		buttons.quit = sf::Keyboard::isKeyPressed(sf::Keyboard::Delete);
		buttons.left = sf::Keyboard::isKeyPressed(sf::Keyboard::Left) || sf::Keyboard::isKeyPressed(sf::Keyboard::A);
		buttons.right = sf::Keyboard::isKeyPressed(sf::Keyboard::Right) || sf::Keyboard::isKeyPressed(sf::Keyboard::D);
		return buttons;
	}

	// Do nothing for SFML
	void Platform::shutdown() {}

	Supports Platform::supports() {
		return Supports::SHADOWS;
	}
}
