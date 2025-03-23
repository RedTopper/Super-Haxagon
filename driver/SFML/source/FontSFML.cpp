// SPDX-FileCopyrightText: 2020 AJ Walter
// SPDX-License-Identifier: GPL-3.0-or-later OR MIT

#include "Driver/Font.hpp"

#include "Core/Structs.hpp"

#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/RenderWindow.hpp>

#include <cmath>

namespace SuperHaxagon {
	struct Font::FontImpl {
		explicit FontImpl(sf::RenderWindow& window, const std::string& path, int size) :
			window(window),
			size(static_cast<float>(size)) {
			loaded = font.loadFromFile(path);
		}

		float getHeight() const {
			return size * scale;
		}

		float getWidth(const std::string& str) const {
			if (!loaded) return 0;
			sf::Text sfText;
			sfText.setFont(font);
			sfText.setCharacterSize(static_cast<int>(size * scale));
			sfText.setString(str);
			return sfText.getLocalBounds().width;
		}

		void draw(const Color& color, const Vec2f& position, const Alignment alignment, const std::string& text) const {
			if (!loaded) return;
			sf::Text sfText;
			sf::Vector2f sfPosition;
			sfPosition.y = static_cast<float>(position.y);
			sfText.setFont(font);
			sfText.setCharacterSize(static_cast<int>(size * scale));
			sfText.setFillColor({color.r, color.g, color.b, color.a});
			sfText.setString(text);

			const auto width = sfText.getLocalBounds().width;
			if (alignment == Alignment::LEFT) sfPosition.x = position.x;
			if (alignment == Alignment::CENTER) sfPosition.x = position.x - width / 2;
			if (alignment == Alignment::RIGHT) sfPosition.x = position.x - width;
			sfPosition.x = std::round(sfPosition.x);
			sfPosition.y = std::round(sfPosition.y);
			sfText.setPosition(sfPosition);

			window.draw(sfText);
		}

		sf::RenderWindow& window;
		sf::Font font;
		float scale{};
		float size;
		bool loaded;
	};

	Font::Font(std::unique_ptr<Font::FontImpl> impl) : _impl(std::move(impl)) {}

	Font::~Font() = default;

	void Font::setScale(const float scale) {
		// scale up at half rate so it looks nice I guess?
		_impl->scale = (scale - 1) / 2 + 1;
	}

	float Font::getHeight() const {
		return _impl->getHeight();
	}

	float Font::getWidth(const std::string& str) const {
		return _impl->getWidth(str);
	}

	void Font::draw(const Color& color, const Vec2f& position, const Alignment alignment, const std::string& text) const {
		_impl->draw(color, position, alignment, text);
	}

	std::unique_ptr<Font> createFont(sf::RenderWindow& window, const std::string& path, int size) {
		return std::make_unique<Font>(std::make_unique<Font::FontImpl>(window, path, size));
	}
}
