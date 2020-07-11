#include <string>
#include <ctime>
#include <algorithm>
#include <array>
#include <iostream>

#ifdef _WIN32
#include <direct.h>
#else
#include <sys/types.h>
#include <sys/stat.h>
#endif

#include "DriverSFML/PlatformSFML.hpp"
#include "DriverSFML/PlayerSoundSFML.hpp"
#include "DriverSFML/AudioSFML.hpp"
#include "DriverSFML/FontSFML.hpp"

namespace SuperHaxagon {
	PlatformSFML::PlatformSFML() : _bgm(nullptr) {
		_clock.restart();

		sf::ContextSettings settings;
		settings.antialiasingLevel = 8;

		sf::VideoMode video(
			static_cast<int>(sf::VideoMode::getDesktopMode().width * 0.75),
			static_cast<int>(sf::VideoMode::getDesktopMode().height * 0.75)
		);

		#if defined(_WIN32)
		const auto nError = _mkdir("./sdmc");
		#else
		const auto nError = mkdir("./sdmc", 755);
		#endif

		if (nError) std::cout << "Could not create user directory" << std::endl;

		_window = std::make_unique<sf::RenderWindow>(video, "Super Haxagon", sf::Style::Default, settings);
		_window->setVerticalSyncEnabled(true);
		_loaded = true;
	}

	PlatformSFML::~PlatformSFML() = default;

	bool PlatformSFML::loop() {
		const auto throttle = sf::milliseconds(1);
		sf::sleep(throttle);
		_delta = _clock.getElapsedTime().asSeconds();
		_clock.restart();
		sf::Event event{};
		while (_window->pollEvent(event)) {
			if (event.type == sf::Event::Closed) {
				_window->close();
			}

			if (event.type == sf::Event::Resized) {
				sf::FloatRect visibleArea(
					0.0f, 
					0.0f, 
					static_cast<float>(event.size.width), 
					static_cast<float>(event.size.height)
				);

				_window->setView(sf::View(visibleArea));
			}
		}
		return _loaded && _window->isOpen();
	}

	double PlatformSFML::getDilation() {
		// The game was originally designed with 60FPS in mind
		const auto dilation = _delta / (1.0 / 60.0);
		return dilation > 4.0 ? 4.0 : (dilation < 0.05 ? 0.05 : dilation);
	}

	std::string PlatformSFML::getPath(const std::string& partial) {
		auto path = std::string("./sdmc") + partial;
		return path;
	}

	std::string PlatformSFML::getPathRom(const std::string& partial) {
		auto path = std::string("./romfs") + partial;
		return path;
	}

	std::unique_ptr<Audio> PlatformSFML::loadAudio(const std::string& path, Stream stream) {
		return std::make_unique<AudioSFML>(path, stream);
	}

	std::unique_ptr<Font> PlatformSFML::loadFont(const std::string& path, int size) {
		return std::make_unique<FontSFML>(*this, path, size);
	}

	void PlatformSFML::playSFX(Audio& audio) {
		for (auto it = _sfx.begin(); it != _sfx.end();) {
			auto& playing = *it;
			if (playing->isDone()) {
				it = _sfx.erase(it);
			} else {
				++it;
			}
		}

		auto player = audio.instantiate();
		if (!player) return;
		player->setLoop(false);
		player->play();
		_sfx.emplace_back(std::move(player));
	}

	void PlatformSFML::playBGM(Audio& audio) {
		_bgm = audio.instantiate();
		if (!_bgm) return;
		_bgm->setChannel(0);
		_bgm->setLoop(true);
		_bgm->play();
	}

	void PlatformSFML::stopBGM() {
		// Destructor will be called and audio will stop
		if (_bgm) _bgm = nullptr;
	}

	double PlatformSFML::getBgmVelocity() {
		return _bgm ? _bgm->getVelocity() : 0.0;
	}

	std::string PlatformSFML::getButtonName(const Buttons button) {
		if (button.back) return "ESC";
		if (button.select) return "ENTER";
		if (button.left) return "LEFT";
		if (button.right) return "RIGHT";
		if (button.quit) return "DELETE";
		return "?";
	}

	Buttons PlatformSFML::getPressed() {
		Buttons buttons{};
		buttons.select = sf::Keyboard::isKeyPressed(sf::Keyboard::Enter);
		buttons.back = sf::Keyboard::isKeyPressed(sf::Keyboard::Escape);
		buttons.quit = sf::Keyboard::isKeyPressed(sf::Keyboard::Delete);
		buttons.left = sf::Keyboard::isKeyPressed(sf::Keyboard::Left) | sf::Keyboard::isKeyPressed(sf::Keyboard::A);
		buttons.right = sf::Keyboard::isKeyPressed(sf::Keyboard::Right) | sf::Keyboard::isKeyPressed(sf::Keyboard::D);
		return buttons;
	}

	Point PlatformSFML::getScreenDim() const {
		Point point{};
		point.x = _window->getSize().x;
		point.y = _window->getSize().y;
		return point;
	}

	void PlatformSFML::screenBegin() {
		_window->clear(sf::Color::Black);
	}

	void PlatformSFML::screenSwap() {
		// Do nothing, since there is no second screen on a PC
	}

	void PlatformSFML::screenFinalize() {
		_window->display();
	}

	void PlatformSFML::drawRect(const Color& color, const Point& point, const Point& size) {
		sf::RectangleShape rectangle(sf::Vector2f(static_cast<float>(size.x), static_cast<float>(size.y)));
		const sf::Color sfColor{color.r, color.g, color.b, color.a};
		rectangle.setFillColor(sfColor);
		rectangle.setPosition(static_cast<float>(point.x), static_cast<float>(point.y));
		_window->draw(rectangle);
	}

	void PlatformSFML::drawTriangle(const Color& color, const std::array<Point, 3>& points) {
		const auto height = getScreenDim().y;
		const sf::Color sfColor{ color.r, color.g, color.b, color.a };
		sf::ConvexShape convex;
		convex.setPointCount(3);
		convex.setPoint(0, sf::Vector2f(static_cast<float>(points[0].x), static_cast<float>(height - points[0].y)));
		convex.setPoint(1, sf::Vector2f(static_cast<float>(points[1].x), static_cast<float>(height - points[1].y)));
		convex.setPoint(2, sf::Vector2f(static_cast<float>(points[2].x), static_cast<float>(height - points[2].y)));
		convex.setFillColor(sfColor);
		convex.setPosition(0, 0);
		_window->draw(convex);
	}

	std::unique_ptr<Twist> PlatformSFML::getTwister() {
		try {
			std::random_device source;
			if (source.entropy() == 0) throw std::runtime_error("entropy too low, seeding with time");
			std::mt19937::result_type data[std::mt19937::state_size];
			generate(std::begin(data), std::end(data), ref(source));
			return std::make_unique<Twist>(
					std::make_unique<std::seed_seq>(std::begin(data), std::end(data))
			);
		} catch (std::runtime_error& e) {
			std::cout << e.what() << std::endl;
			auto* a = new std::seed_seq{time(nullptr)};
			return std::make_unique<Twist>(
					std::unique_ptr<std::seed_seq>(a)
			);
		}
	}
}
