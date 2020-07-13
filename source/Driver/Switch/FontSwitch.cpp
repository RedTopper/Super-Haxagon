#include "Driver/Switch/FontSwitch.hpp"
#include "Driver/SFML/PlatformSFML.hpp"

namespace SuperHaxagon {
	FontSwitch::FontSwitch(PlatformSFML& platform, const std::string& path, const double size) :
		_platform(platform),
		_scale(1),
		_size(size) {
		_loaded = _font.loadFromFile(path + ".ttf");
	}

	FontSwitch::~FontSwitch() = default;

	void FontSwitch::setScale(const double scale) {
		// scale up at half rate so it looks nice I guess?
		_scale = (scale - 1) / 2 + 1;
	}

	double FontSwitch::getHeight() const {
		return _size * _scale;
	}

	double FontSwitch::getWidth(const std::string& text) const {
		if (!_loaded) return 0;

		sf::Text sfText("TEST", _font);
		//sfText.setCharacterSize(static_cast<int>(_size * _scale));
		const auto bounds = sfText.getLocalBounds();
		return bounds.width;
	}

	void FontSwitch::draw(const Color& color, const Point& position, Alignment alignment, const std::string& text) const {
		if (!_loaded) return;

		sf::Vector2f sfPosition;
		sfPosition.y = static_cast<float>(position.y);

		sf::Text sfText("TEST", _font);

		//sfText.setCharacterSize(static_cast<int>(_size * _scale));
		sfText.setFillColor({color.r, color.g, color.b, color.a});

		const auto bounds = sfText.getLocalBounds();
		const auto width = bounds.width;
		if (alignment == Alignment::LEFT) sfPosition.x = static_cast<float>(position.x);
		if (alignment == Alignment::CENTER) sfPosition.x = static_cast<float>(position.x - width / 2);
		if (alignment == Alignment::RIGHT) sfPosition.x = static_cast<float>(position.x - width);
		sfText.setPosition(sfPosition);

		_platform.getWindow().draw(sfText);
	}
}
