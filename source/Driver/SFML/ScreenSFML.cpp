#include "Driver/Screen.hpp"

#include "Core/Structs.hpp"

#include <SFML/Graphics.hpp>

namespace SuperHaxagon {
	struct Screen::ScreenImpl {
		explicit ScreenImpl(sf::RenderWindow& window) : window(window) {}

		sf::RenderWindow& window;
	};

	Screen::Screen(std::unique_ptr<ScreenImpl> data) : _impl(std::move(data)) {}

	Screen::~Screen() = default;

	Vec2f Screen::getScreenDim() const {
		Vec2f point{};
		auto size = _impl->window.getSize();
		point.x = static_cast<float>(size.x);
		point.y = static_cast<float>(size.y);
		return point;
	}

	void Screen::screenBegin() const {}

	// Do nothing since we don't have two screens
	void Screen::screenSwitch() const {}

	void Screen::screenFinalize() const {
		_impl->window.display();
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

		_impl->window.draw(convex);
	}

	void Screen::clear(const Color& color) const {
		_impl->window.clear(sf::Color(color.r, color.g, color.b));
	}

	std::unique_ptr<Screen> createScreen(sf::RenderWindow& window) {
		return std::make_unique<Screen>(std::make_unique<Screen::ScreenImpl>(window));
	}
}
