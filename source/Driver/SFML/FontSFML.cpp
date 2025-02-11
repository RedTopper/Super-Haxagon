#include "Driver/Font.hpp"

#include "Core/Structs.hpp"

#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/RenderWindow.hpp>

#include <cmath>

namespace SuperHaxagon {
	struct Font::FontData {
		explicit FontData(sf::RenderWindow& renderWindow, const std::string& path, int size) :
			window(renderWindow),
			size(static_cast<float>(size)) {
			loaded = font.loadFromFile(path + ".ttf");
		}

		sf::RenderWindow& window;
		sf::Font font;
		float scale{};
		float size;
		bool loaded;
	};

	std::unique_ptr<Font> createFont(sf::RenderWindow& renderWindow, const std::string& path, int size) {
		return std::make_unique<Font>(std::make_unique<Font::FontData>(renderWindow, path, size));
	}

	Font::Font(std::unique_ptr<Font::FontData> data) : _data(std::move(data)) {}

	Font::~Font() = default;

	void Font::setScale(const float scale) {
		// scale up at half rate so it looks nice I guess?
		_data->scale = (scale - 1) / 2 + 1;
	}

	float Font::getHeight() const {
		return _data->size * _data->scale;
	}

	float Font::getWidth(const std::string& str) const {
		if (!_data->loaded) return 0;
		sf::Text sfText;
		sfText.setFont(_data->font);
		sfText.setCharacterSize(static_cast<int>(_data->size * _data->scale));
		sfText.setString(str);
		return sfText.getLocalBounds().width;
	}

	void Font::draw(const Color& color, const Point& position, const Alignment alignment, const std::string& text) const {
		if (!_data->loaded) return;
		sf::Text sfText;
		sf::Vector2f sfPosition;
		sfPosition.y = static_cast<float>(position.y);
		sfText.setFont(_data->font);
		sfText.setCharacterSize(static_cast<int>(_data->size * _data->scale));
		sfText.setFillColor({color.r, color.g, color.b, color.a});
		sfText.setString(text);

		const auto width = sfText.getLocalBounds().width;
		if (alignment == Alignment::LEFT) sfPosition.x = position.x;
		if (alignment == Alignment::CENTER) sfPosition.x = position.x - width / 2;
		if (alignment == Alignment::RIGHT) sfPosition.x = position.x - width;
		sfPosition.x = std::round(sfPosition.x);
		sfPosition.y = std::round(sfPosition.y);
		sfText.setPosition(sfPosition);

		_data->window.draw(sfText);
	}
}
