// SPDX-FileCopyrightText: 2021 AJ Walter
// SPDX-License-Identifier: GPL-3.0-or-later OR MIT

#include "Driver/Font.hpp"

#include "Core/Structs.hpp"

#include <libndls.h>

namespace SuperHaxagon {
	std::unique_ptr<char[]> utf16(const std::string& text) {
		auto chars = std::make_unique<char[]>(text.length() * 2 + 2);
		for (size_t i = 0; i < text.length(); i++) {
			chars[i * 2] = text[i];
			chars[i * 2 + 1] = '\0';
		}

		return chars;
	}

	struct Font::FontImpl {
		FontImpl(Gc& gc, const int size) : gc(gc) {
			if (size == 32) {
				font = SerifBold24;
			} else {
				font = SerifRegular10;
			}
		};

		float getWidth(const std::string& text) {
			const auto chars = utf16(text);
			return gui_gc_getStringWidth(gc, font, chars.get(), 0, text.length());
		}

		float getHeight() {
			const auto size = static_cast<float>(gui_gc_getCharHeight(gc, font, 0x4100));
			return size - size / 6.0f;
		}

		void draw(const Color& color, const Vec2f& position, const Alignment alignment, const std::string& text) {
			gui_gc_setColorRGB(gc, color.r, color.g, color.b);
			gui_gc_setFont(gc, font);

			Vec2f translate{};
			const auto chars = utf16(text);
			const auto width = getWidth(text);
			if (alignment == Alignment::LEFT) translate.x = position.x;
			if (alignment == Alignment::CENTER) translate.x = position.x - width / 2;
			if (alignment == Alignment::RIGHT) translate.x = position.x - width;
			translate.y = position.y + getHeight()/2.0f;

			gui_gc_drawString(gc, chars.get(), std::round(translate.x), std::round(translate.y), static_cast<gui_gc_StringMode>(GC_SM_NORMAL | GC_SM_MIDDLE));
		}

		gui_gc_Font font{};
		Gc& gc;
	};

	Font::Font(std::unique_ptr<Font::FontImpl> impl) : _impl(std::move(impl)) {}

	Font::~Font() = default;

	float Font::getWidth(const std::string& text) const {
		return _impl->getWidth(text);
	}

	void Font::draw(const Color& color, const Vec2f& position, const Alignment alignment, const std::string& text) const {
		_impl->draw(color, position, alignment, text);
	}

	// Cannot resize NSpire window, so no scaling
	void Font::setScale(float) {}

	float Font::getHeight() const {
		return _impl->getHeight();
	}

	std::unique_ptr<Font> createFont(Gc& gc, const int size) {
		return std::make_unique<Font>(std::make_unique<Font::FontImpl>(gc, size));
	}
}
