#include "Driver/Nspire/FontNspire.hpp"

#include "Core/Structs.hpp"

#include <libndls.h>

namespace SuperHaxagon {
	FontNspire::FontNspire(Gc& gc, const int size) : _gc(gc) {
		if (size == 32) {
			_font = SerifBold24;
		} else {
			_font = SerifRegular10;
		}
	}

	FontNspire::~FontNspire() = default;

	float FontNspire::getWidth(const std::string& text) const {
		const auto chars = utf16(text);
		return gui_gc_getStringWidth(_gc, _font, chars.get(), 0, text.length());
	}

	void FontNspire::draw(const Color& color, const Point& position, const Alignment alignment, const std::string& text) {
		gui_gc_setColorRGB(_gc, color.r, color.b, color.g);
		gui_gc_setFont(_gc, _font);

		Point translate{};
		const auto chars = utf16(text);
		const auto width = getWidth(text);
		if (alignment == Alignment::LEFT) translate.x = position.x;
		if (alignment == Alignment::CENTER) translate.x = position.x - width / 2;
		if (alignment == Alignment::RIGHT) translate.x = position.x - width;
		translate.y = position.y + getHeight()/2.0f;
		
		gui_gc_drawString(_gc, chars.get(), translate.x, translate.y, static_cast<gui_gc_StringMode>(GC_SM_NORMAL | GC_SM_MIDDLE));
	}

	void FontNspire::setScale(float) {}

	float FontNspire::getHeight() const {
		const auto size = static_cast<float>(gui_gc_getCharHeight(_gc, _font, 0x4100));
		return size - size / 6.0f;
	}

	std::unique_ptr<char[]> FontNspire::utf16(const std::string& text) const {
		auto chars = std::make_unique<char[]>(text.length() * 2 + 2);
		for (size_t i = 0; i < text.length(); i++) {
			chars[i * 2] = text[i];
			chars[i * 2 + 1] = '\0';
		}

		return chars;
	}
}
