#include "DriverWin/FontWin.hpp"
#include "DriverWin/PlatformWin.hpp"

namespace SuperHaxagon {
	FontWin::FontWin(PlatformWin& platform, const std::string& path, double size) : platform(platform), scale(1), size(size) {
		loaded = font.loadFromFile(path + ".ttf");
	}

	FontWin::~FontWin() = default;

	void FontWin::setScale(double _scale) {
		// scale up at half rate so it looks nice I guess?
		scale = (_scale - 1) / 2 + 1;
	}

	double FontWin::getHeight() const {
		return size * scale;
	}

	double FontWin::getWidth(const std::string& text) const {
		if (!loaded) return 0;
		sf::Text sfText;
		sfText.setFont(font);
		sfText.setCharacterSize((int)(size * scale));
		sfText.setString(text);
		return sfText.getLocalBounds().width;
	}

	void FontWin::draw(const Color& color, const Point& position, Alignment alignment, const std::string& str) const {
		if (!loaded) return;
		sf::Text sfText;
		sf::Vector2f sfPosition;
		sfPosition.y = position.y;
		sfText.setFont(font);
		sfText.setCharacterSize((int)(size * scale));
		sfText.setFillColor({color.r, color.g, color.b, color.a});
		sfText.setString(str);

		float width = sfText.getLocalBounds().width;
		if (alignment == Alignment::LEFT) sfPosition.x = position.x;
		if (alignment == Alignment::CENTER) sfPosition.x = position.x - width / 2;
		if (alignment == Alignment::RIGHT) sfPosition.x = position.x - width;
		sfText.setPosition(sfPosition);

		platform.getWindow().draw(sfText);
	}
}
