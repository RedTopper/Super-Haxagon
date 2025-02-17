#include "Driver/Platform.hpp"

#include "CreateSFML.hpp"
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
	Screen createScreen(sf::RenderWindow& window);
	std::unique_ptr<Sound> createSound(const std::string& path);
	std::unique_ptr<Font> createFont(sf::RenderWindow& renderWindow, const std::string& path, int size);

	sf::VideoMode getVideoMode() {
		// Give the user the default intended aspect ratio
		unsigned int width = (sf::VideoMode::getDesktopMode().width - 1) / 400;
		unsigned int height = (sf::VideoMode::getDesktopMode().height - 1) / 240;
		unsigned int min = std::max<unsigned int>(std::min(width, height), 1);
		return {min * 400, min * 240};
	}

	sf::ContextSettings getSettings() {
		sf::ContextSettings settings;
		settings.antialiasingLevel = 8;
		return settings;
	}

	struct Platform::PlatformImpl {
		PlatformImpl(const std::string& sdmcPath, const std::string& romfsPath, const bool platformBackslash) :
			window(getVideoMode(), "Super Haxagon", sf::Style::Default, getSettings()),
			screen(createScreen(window))
		{
			sdmc = sdmcPath;
			romfs = romfsPath;
			backslash = platformBackslash;

			clock.restart();

			window.setVerticalSyncEnabled(true);
			window.display();

			// Set the window name to "Super Haxagon" first (above), to match the .desktop file,
			// then add the version after the window has loaded.
			window.setTitle(std::string("Super Haxagon (") + VERSION + ")");

			loaded = true;
		}

		bool loop() {
			const auto throttle = sf::milliseconds(1);
			sf::sleep(throttle);
			delta = clock.getElapsedTime().asSeconds();
			clock.restart();
			sf::Event event{};
			while (window.pollEvent(event)) {
				if (event.type == sf::Event::Closed) window.close();
				if (event.type == sf::Event::GainedFocus) focus = true;
				if (event.type == sf::Event::LostFocus) focus = false;
				if (event.type == sf::Event::Resized) {
					const auto width = event.size.width > 400 ? event.size.width : 400;
					const auto height = event.size.height > 240 ? event.size.height : 240;

					if (width != event.size.width || height != event.size.height) {
						window.setSize({width, height });
					}

					sf::FloatRect visibleArea(
							0.0f,
							0.0f,
							static_cast<float>(width),
							static_cast<float>(height)
					);

					window.setView(sf::View(visibleArea));
				}
			}
			return loaded && window.isOpen();
		}

		std::string getPath(const std::string& partial, const Location location) const {
			auto path = partial;

			if (backslash) std::replace(path.begin(), path.end(), '/', '\\');

			switch (location) {
				case Location::ROM:
					return romfs + path;
				case Location::USER:
					return sdmc + path;
			}

			return "";
		}

		bool loaded = false;
		bool focus = true;
		bool backslash = false;
		sf::Clock clock{};
		sf::RenderWindow window;
		Screen screen;
		std::string romfs;
		std::string sdmc;
		float delta = 0.0f;
	};

	Platform::Platform() {
		bool backslash;
		std::string sdmc, romfs;

		// Get information about our platform and setup anything
		// platform specific (like creating folders)
		initializePlatform(sdmc, romfs, backslash);

		_impl = std::make_unique<PlatformImpl>(sdmc, romfs, backslash);

		// Let windows do whatever it needs with the Window
		osSpecificWindowSetup(_impl->window);
	}

	Platform::~Platform() = default;

	bool Platform::loop() {
		return _impl->loop();
	}

	float Platform::getDilation() const {
		// The game was originally designed with 60FPS in mind
		return _impl->delta / (1.0f / 60.0f);
	}

	std::string Platform::getPath(const std::string& partial, const Location location) const {
		return _impl->getPath(partial, location);
	}

	std::unique_ptr<Font> Platform::loadFont(const int size) const {
		return createFont(_impl->window, getPath("/bump-it-up.ttf", Location::ROM), size);
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

	Screen& Platform::getScreen() {
		return _impl->screen;
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
		if (!_impl->focus) return buttons;
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
