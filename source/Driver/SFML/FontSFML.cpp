#include "Driver/SFML/FontSFML.hpp"

#include "Core/Structs.hpp"
#include "Driver/SFML/PlatformSFML.hpp"

#include <cmath>

namespace SuperHaxagon {
	FontSFML::FontSFML(PlatformSFML& platform, const std::string& path, const float size) :
		_platform(platform),
		_scale(1),
		_size(size) {
		_loaded = _font.loadFromFile(path + ".ttf");
	}

	FontSFML::~FontSFML() = default;

	void FontSFML::setScale(const float scale) {
		// scale up at half rate so it looks nice I guess?
		_scale = (scale - 1) / 2 + 1;
	}

	float FontSFML::getHeight() const {
		return _size * _scale;
	}

	float FontSFML::getWidth(const std::string& text) const {
		if (!_loaded) return 0;
		sf::Text sfText;
		sfText.setFont(_font);
		sfText.setCharacterSize(static_cast<int>(_size * _scale));
		sfText.setString(text);
		return sfText.getLocalBounds().width;
	}

	void FontSFML::draw(const Color& color, const Point& position, const Alignment alignment, const std::string& text) {
		if (!_loaded) return;
		sf::Text sfText;
		sf::Vector2f sfPosition;
		sfPosition.y = static_cast<float>(position.y);
		sfText.setFont(_font);
		sfText.setCharacterSize(static_cast<int>(_size * _scale));
		sfText.setFillColor({color.r, color.g, color.b, color.a});
		sfText.setString(text);

		const auto width = sfText.getLocalBounds().width;
		if (alignment == Alignment::LEFT) sfPosition.x = position.x;
		if (alignment == Alignment::CENTER) sfPosition.x = position.x - width / 2;
		if (alignment == Alignment::RIGHT) sfPosition.x = position.x - width;
		sfPosition.x = std::round(sfPosition.x);
		sfPosition.y = std::round(sfPosition.y);
		sfText.setPosition(sfPosition);

		_platform.getWindow().draw(sfText);
	}
}
