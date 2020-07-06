#include "DriverWin/FontWin.hpp"
#include "DriverWin/PlatformWin.hpp"

namespace SuperHaxagon {
	FontWin::FontWin(PlatformWin& platform, const std::string& path, int size) : platform(platform), size(size) {
		loaded = font.loadFromFile(path + ".ttf");
	}

	int FontWin::getHeight() const {
		return size;
	}

	void FontWin::draw(const Color& color, const Point& position, Alignment alignment, std::string text) const {
		if (!loaded) return;
		sf::Text sfText;
		sf::Vector2f sfPosition;
		sfPosition.y = position.y;
		sfText.setFont(font);
		sfText.setCharacterSize(size);
		sfText.setFillColor({color.r, color.g, color.b, color.a});
		sfText.setString(text);

		float width = sfText.getLocalBounds().width;
		if (alignment == Alignment::LEFT) sfPosition.x = position.x;
		if (alignment == Alignment::CENTER) sfPosition.x = position.x - width / 2;
		if (alignment == Alignment::RIGHT) sfPosition.x = position.x - width;
		sfText.setPosition(sfPosition);

		platform.getWindow().draw(sfText);
	}
}
