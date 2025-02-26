#include "Driver/Platform.hpp"

#include "Core/Structs.hpp"
#include "Core/Twist.hpp"
#include "Driver/Font.hpp"
#include "Driver/Music.hpp"
#include "Driver/Screen.hpp"
#include "Driver/Sound.hpp"

#include <ctime>
#include <memory>
#include <iostream>

namespace SuperHaxagon {

	// Forward declare all of your factories here
	// so the platform can instantiate the objects.
	std::unique_ptr<Music> createMusic();
	Screen createScreen();
	std::unique_ptr<Sound> createSound();
	std::unique_ptr<Font> createFont();

	struct Platform::PlatformImpl {
		PlatformImpl() : screen(createScreen()) {
			// Do any platform specific initialization here.
		}

		// And any platform specific member variables go here.
		Screen screen;
	};

	Platform::Platform() : _impl(std::make_unique<PlatformImpl>()) {
		// You can either have the initialization list instantiate
		// your platform's implementation (above), or do it here in
		// the constructor body.
		message(Dbg::INFO, "platform", "Congrats, you compiled the null driver!");
	}

	Platform::~Platform() = default;

	bool Platform::loop() {
		// This method is called every frame. It returns true
		// if the game should keep running or not.

		message(Dbg::WARN, "platform", "This driver will do nothing without any implementation.");
		message(Dbg::INFO, "platform", "And can be used to test if it would be viable to port this game.");

		return false;
	}

	float Platform::getDilation() const {
		// This controls the speed of the game logic.
		// A dilation of 1.0 means the game is running at exactly 60.0fps.
		// If the game can run higher or lower than 60.0fps on your platform,
		// use a system timer to adjust the game logic to run at an appropriate speed.
		float frameTime = 0.01666f;
		return frameTime / (1.0f/60.0f);
	}

	std::string Platform::getPath(const std::string& partial, const Location) const {
		return "./" + partial;
	}

	std::unique_ptr<std::istream> Platform::openFile(const std::string&, const Location) const {
		return nullptr;
	}

	std::unique_ptr<Font> Platform::loadFont(const int) const {
		// Call your platform's implementations here. For fonts, you can
		// return a valid object, or nullptr if your system does not support fonts.
		// The linker will still require that you provide a "valid" implementation,
		// but it will not call into it if you return nullptr here.
		return createFont();
		// return nullptr;
	}

	std::unique_ptr<Sound> Platform::loadSound(const std::string&) const {
		return createSound();
	}

	std::unique_ptr<Music> Platform::loadMusic(const std::string& base, const Location location) const {
		return createMusic();
	}

	Screen& Platform::getScreen() {
		return _impl->screen;
	}

	std::vector<std::pair<Location, std::string>> Platform::loadUserLevels() {
		// You can use the implementation PlatformSupportsFilesystem.cpp to have your platform
		// load all levels from a specific folder. If you can't load files _at all_, you
		// can return no additional levels loaded like below.
		return {};
	}

	std::string Platform::getButtonName(ButtonName buttonName) {
		switch (buttonName) {
			case ButtonName::BACK: return "ESC";
			case ButtonName::SELECT: return "ENTER";
			case ButtonName::LEFT: return "LEFT | A";
			case ButtonName::RIGHT: return "RIGHT | D";
			case ButtonName::QUIT: return "DEL";
			default: return "?";
		}
	}

	Buttons Platform::getPressed() const {
		// Set the button object to what keys are currently pressed here.
		// An example for SFML is written below.
		Buttons buttons{};
		//buttons.select = sf::Keyboard::isKeyPressed(sf::Keyboard::Enter);
		//buttons.back = sf::Keyboard::isKeyPressed(sf::Keyboard::Escape);
		return buttons;
	}

	void Platform::shutdown() {
		// If you need to do anything before the application quits, such as
		// showing error messages to the user, do that here.
		// Note: The game will no longer exist by this point!
		message(Dbg::FATAL, "platform", "As such, the game will now quit.");
	}

	Supports Platform::supports() {
		return Supports::SHADOWS;
	}

	void Platform::message(const Dbg dbg, const std::string& where, const std::string& message) const {
		if (dbg == Dbg::INFO) {
			std::cout << "[null:info] " + where + ": " + message << std::endl;
		} else if (dbg == Dbg::WARN) {
			std::cout << "[null:warn] " + where + ": " + message << std::endl;
		} else if (dbg == Dbg::FATAL) {
			std::cerr << "[null:fatal] " + where + ": " + message << std::endl;
		}
	}

	Twist Platform::getTwister() {
		std::unique_ptr<std::seed_seq> seq;
		seq.reset(new std::seed_seq{time(nullptr)});
		return Twist(std::move(seq));
	}
}
