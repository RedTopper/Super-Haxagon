#include "Driver/Font.hpp"

#include "Core/Structs.hpp"

#include <libndls.h>

namespace SuperHaxagon {
	struct Font::FontData {
		FontData(Gc& gc, const int size) : gc(gc) {
			if (size == 32) {
				font = SerifBold24;
			} else {
				font = SerifRegular10;
			}
		};

		gui_gc_Font font{};
		Gc& gc;
	};

	std::unique_ptr<Font> createFont(Gc& gc, const int size) {
		return std::make_unique<Font>(std::make_unique<Font::FontData>(gc, size));
	}

	std::unique_ptr<char[]> utf16(const std::string& text) {
		auto chars = std::make_unique<char[]>(text.length() * 2 + 2);
		for (size_t i = 0; i < text.length(); i++) {
			chars[i * 2] = text[i];
			chars[i * 2 + 1] = '\0';
		}

		return chars;
	}

	Font::Font(std::unique_ptr<Font::FontData> data) : _data(std::move(data)) {}

	Font::~Font() = default;

	float Font::getWidth(const std::string& text) const {
		const auto chars = utf16(text);
		return gui_gc_getStringWidth(_data->gc, _data->font, chars.get(), 0, text.length());
	}

	void Font::draw(const Color& color, const Point& position, const Alignment alignment, const std::string& text) const {
		gui_gc_setColorRGB(_data->gc, color.r, color.g, color.b);
		gui_gc_setFont(_data->gc, _data->font);

		Point translate{};
		const auto chars = utf16(text);
		const auto width = getWidth(text);
		if (alignment == Alignment::LEFT) translate.x = position.x;
		if (alignment == Alignment::CENTER) translate.x = position.x - width / 2;
		if (alignment == Alignment::RIGHT) translate.x = position.x - width;
		translate.y = position.y + getHeight()/2.0f;
		
		gui_gc_drawString(_data->gc, chars.get(), translate.x, translate.y, static_cast<gui_gc_StringMode>(GC_SM_NORMAL | GC_SM_MIDDLE));
	}

	void Font::setScale(float) {}

	float Font::getHeight() const {
		const auto size = static_cast<float>(gui_gc_getCharHeight(_data->gc, _data->font, 0x4100));
		return size - size / 6.0f;
	}
}
