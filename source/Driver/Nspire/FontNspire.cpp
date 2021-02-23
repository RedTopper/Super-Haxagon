#include "Driver/Nspire/FontNspire.hpp"

#include "Core/Structs.hpp"

#include <libndls.h>

namespace SuperHaxagon {
	FontNspire::FontNspire(const int size) {
		if (size == 32) {
			_font = SerifBold24;
		} else {
			_font = SerifRegular10;
		}

		_size = FontNspire::getHeight();
	}

	FontNspire::~FontNspire() = default;

	float FontNspire::getWidth(const std::string& text) const {
		auto* gc = gui_gc_global_GC();
		const auto chars = utf16(text);
		return gui_gc_getStringWidth(gc, _font, chars.get(), 0, text.length());
	}

	void FontNspire::draw(const Color& color, const Point& position, const Alignment alignment, const std::string& text) {
		auto* gc = gui_gc_global_GC();
		gui_gc_setColorRGB(gc, color.r, color.b, color.g);
		gui_gc_setFont(gc, _font);

		Point translate{};
		const auto chars = utf16(text);
		const auto width = getWidth(text);
		if (alignment == Alignment::LEFT) translate.x = position.x;
		if (alignment == Alignment::CENTER) translate.x = position.x - width / 2;
		if (alignment == Alignment::RIGHT) translate.x = position.x - width;
		// Fonts draw slightly too low, bump it up a bit
		translate.y = position.y - _size / 4.5f;
		
		gui_gc_drawString(gc, chars.get(), translate.x, translate.y, static_cast<gui_gc_StringMode>(GC_SM_NORMAL | GC_SM_TOP));
	}

	void FontNspire::setScale(float) {}

	float FontNspire::getHeight() const {
		auto* gc = gui_gc_global_GC();
		return static_cast<float>(gui_gc_getCharHeight(gc, _font, 0x4100));
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
