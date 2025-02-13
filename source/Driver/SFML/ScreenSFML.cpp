#include "Driver/Screen.hpp"

#include <SFML/Graphics.hpp>

namespace SuperHaxagon {
	struct Screen::ScreenData {
		ScreenData(sf::RenderWindow& window) : window(window) {}

		sf::RenderWindow& window;
	};

	std::unique_ptr<Screen> createScreen(sf::RenderWindow& window) {
		return std::make_unique<Screen>(std::make_unique<Screen::ScreenData>(window));
	}

	Screen::Screen(std::unique_ptr<ScreenData> data) : _data(std::move(data)) {}

	Screen::~Screen() = default;

	Vec2f Screen::getScreenDim() const {
		Vec2f point{};
		point.x = static_cast<float>(_data->window.getSize().x);
		point.y = static_cast<float>(_data->window.getSize().y);
		return point;
	}

	void Screen::screenBegin() const {
		_data->window.clear(sf::Color(_color.r, _color.g, _color.b));
	}

	// Do nothing since we don't have two screens
	void Screen::screenSwitch() const {}

	void Screen::screenFinalize() const {
		_data->window.display();
	}

	void Screen::drawPoly(const Color& color, const std::vector<Vec2f>& points) const {
		const sf::Color sfColor{ color.r, color.g, color.b, color.a };
		sf::ConvexShape convex(points.size());
		convex.setPosition(0, 0);
		convex.setFillColor(sfColor);
		auto index = 0;
		for (const auto& point : points) {
			convex.setPoint(index++, sf::Vector2f(point.x, point.y));
		}

		_data->window.draw(convex);
	}
}
