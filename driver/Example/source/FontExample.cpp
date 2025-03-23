// SPDX-FileCopyrightText: 2025 AJ Walter
// SPDX-License-Identifier: GPL-3.0-or-later OR MIT

#include "Driver/Font.hpp"

#include "Core/Structs.hpp"

#include <cmath>

namespace SuperHaxagon {
	struct Font::FontImpl {
		explicit FontImpl() {
			// Do font initialization here
		}

		void draw(const Color& color, const Vec2f& position, const Alignment alignment, const std::string& text) const {
			// Do something when drawing.
		}

		// Member variables specific to your platform go here.
		float scale;
	};

	Font::Font(std::unique_ptr<Font::FontImpl> impl) : _impl(std::move(impl)) {}

	Font::~Font() = default;

	void Font::setScale(const float scale) {
		// Or modify variables directly, if the function is simple
		_impl->scale = (scale - 1) / 2 + 1;
	}

	float Font::getHeight() const {
		return 32.0f;
	}

	float Font::getWidth(const std::string&) const {
		// Return fixed information for your platform specifically
		return 1.0f;
	}

	void Font::draw(const Color& color, const Vec2f& position, const Alignment alignment, const std::string& text) const {
		// You can pass through the function to edit member variables directly
		_impl->draw(color, position, alignment, text);
	}

	std::unique_ptr<Font> createFont() {
		// Pass any initialization parameters here into your implementation
		return std::make_unique<Font>(std::make_unique<Font::FontImpl>());
	}
}
