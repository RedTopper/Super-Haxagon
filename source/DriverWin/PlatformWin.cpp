#include <string>
#include <algorithm>

#include "DriverWin/PlatformWin.hpp"
#include "DriverWin/AudioWin.hpp"
#include "DriverWin/FontWin.hpp"

namespace SuperHaxagon {
	PlatformWin::PlatformWin() : clock() {
		clock.restart();

		sf::ContextSettings settings;
		settings.antialiasingLevel = 8;

		sf::VideoMode video(
			400,//(int)(sf::VideoMode::getDesktopMode().width * 0.75),
			240//(int)(sf::VideoMode::getDesktopMode().height * 0.75)
		);

		window = std::make_unique<sf::RenderWindow>(video, "Super Haxagon", sf::Style::Default, settings);
		window->setVerticalSyncEnabled(true);
		loaded = true;
	}

	PlatformWin::~PlatformWin() = default;

	bool PlatformWin::loop() {
		delta = clock.getElapsedTime().asSeconds();
		clock.restart();
		sf::Event event{};
		while (window->pollEvent(event)) {
			if (event.type == sf::Event::Closed) {
				window->close();
			}

			if (event.type == sf::Event::Resized) {
				sf::FloatRect visibleArea(0, 0, event.size.width, event.size.height);
				window->setView(sf::View(visibleArea));
			}
		}
		return loaded && window->isOpen();
	}

	double PlatformWin::getDilation() {
		// The game was originally designed with 60FPS in mind
		double dilation = delta / (1.0 / 60.0);
		return dilation > 4.0 ? 4.0 : (dilation < 0.001 ? 0.05 : dilation);
	}

	std::string PlatformWin::getPath(const std::string& partial) {
		auto path = std::string("./data/custom") + partial;
		std::replace(path.begin(), path.end(), '/', '\\');
		return path;
	}

	std::string PlatformWin::getPathRom(const std::string& partial) {
		auto path = std::string("./data/rom") + partial;
		std::replace(path.begin(), path.end(), '/', '\\');
		return path;
	}

	std::unique_ptr<Audio> PlatformWin::loadAudio(const std::string& path) {
		return std::make_unique<AudioWin>(path);
	}

	std::unique_ptr<Font> PlatformWin::loadFont(const std::string& path, int size) {
		return std::make_unique<FontWin>(*this, path, size);
	}

	void PlatformWin::playSFX(Audio& audio) {

	}

	void PlatformWin::playBGM(Audio& audio) {

	}

	void PlatformWin::stopBGM() {

	}

	Buttons PlatformWin::getPressed() {
		Buttons buttons{};
		buttons.select = sf::Keyboard::isKeyPressed(sf::Keyboard::Enter);
		buttons.back = sf::Keyboard::isKeyPressed(sf::Keyboard::Escape);
		buttons.quit = sf::Keyboard::isKeyPressed(sf::Keyboard::Delete);
		buttons.left = sf::Keyboard::isKeyPressed(sf::Keyboard::Left) | sf::Keyboard::isKeyPressed(sf::Keyboard::A);
		buttons.right = sf::Keyboard::isKeyPressed(sf::Keyboard::Right) | sf::Keyboard::isKeyPressed(sf::Keyboard::D);
		return buttons;
	}

	Point PlatformWin::getScreenDim() const {
		Point point{};
		point.x = window->getSize().x;
		point.y = window->getSize().y;
		return point;
	}

	void PlatformWin::screenBegin() {
		window->clear(sf::Color::White);
	}

	void PlatformWin::screenSwap() {
		// Do nothing, since there is no second screen on a PC
	}

	void PlatformWin::screenFinalize() {
		window->display();
	}

	void PlatformWin::drawRect(const Color& color, const Point& point, const Point& size) {
		sf::RectangleShape rectangle(sf::Vector2f(size.x, size.y));
		sf::Color sfColor{color.r, color.g, color.b, color.a};
		rectangle.setFillColor(sfColor);
		rectangle.setPosition((float)point.x, (float)point.y);
		window->draw(rectangle);
	}

	void PlatformWin::drawTriangle(const Color& color, const std::array<Point, 3>& points) {
		double height = getScreenDim().y;
		sf::ConvexShape convex;
		convex.setPointCount(3);
		convex.setPoint(0, sf::Vector2f(points[0].x, height - points[0].y));
		convex.setPoint(1, sf::Vector2f(points[1].x, height - points[1].y));
		convex.setPoint(2, sf::Vector2f(points[2].x, height - points[2].y));
		sf::Color sfColor{color.r, color.g, color.b, color.a};
		convex.setFillColor(sfColor);
		convex.setPosition(0, 0);
		window->draw(convex);
	}

	std::unique_ptr<Twist> PlatformWin::getTwister() {
		std::random_device source;
		std::mt19937::result_type data[std::mt19937::state_size];
		generate(std::begin(data), std::end(data), ref(source));
		return std::make_unique<Twist>(
				std::make_unique<std::seed_seq>(std::begin(data), std::end(data))
		);
	}
}
