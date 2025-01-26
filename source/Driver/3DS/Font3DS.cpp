#include "Driver/3DS/Font3DS.hpp"

#include "Core/Structs.hpp"

#include <sstream>

namespace SuperHaxagon {
	Font3DS::Font3DS(const std::string& path, const int size) : _size(static_cast<float>(size)) {
		std::stringstream s;
		s << path << "-" << size << ".bcfnt";
		_buff = C2D_TextBufNew(4096);
		_font = C2D_FontLoad(s.str().c_str());

        // For some reason the latest version of citro2d messes with font scaling,
        // causing all sorts of issues. Manually set scale to be 1.0.
    	_font->textScale = 1.0f;
	}

	Font3DS::~Font3DS() {
		C2D_FontFree(_font);
		C2D_TextBufDelete(_buff);
	}

	float Font3DS::getWidth(const std::string& str) const {
		float width, height;
		C2D_Text text;
		C2D_TextFontParse(&text, _font, _buff, str.c_str());
		C2D_TextGetDimensions(&text, 1.0f, 1.0f, &width, &height);
		C2D_TextBufClear(_buff);
		return width;
	}

	float Font3DS::getHeight() const {
		return _size + _size * 4.0f/32.0f;
	}

	void Font3DS::draw(const Color& color, const Point& position, const Alignment alignment, const std::string& str) {
		C2D_Text text;
		const auto c = C2D_Color32(color.r, color.g, color.b, color.a);
		C2D_TextFontParse(&text, _font, _buff, str.c_str());
		C2D_TextOptimize(&text);
        int flags = C2D_WithColor;
    	if (alignment == Alignment::LEFT) flags |= static_cast<int>(C2D_AlignLeft);
        if (alignment == Alignment::CENTER) flags |= static_cast<int>(C2D_AlignCenter);
        if (alignment == Alignment::RIGHT) flags |= static_cast<int>(C2D_AlignRight);
		C2D_DrawText(&text, flags, position.x, position.y, 0, 1.0f, 1.0f, c);
		C2D_TextBufClear(_buff);
	}
}
