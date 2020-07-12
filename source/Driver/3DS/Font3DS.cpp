#include <sstream>

#include "Core/Structs.hpp"
#include "Driver/3DS/Font3DS.hpp"

namespace SuperHaxagon {
	Font3DS::Font3DS(const std::string& path, const int size, C2D_TextBuf& buff) : _size(size), _buff(buff) {
		std::stringstream s;
		s << path << "-" << size << ".bcfnt";
		_font = C2D_FontLoad(s.str().c_str());
	}

	Font3DS::~Font3DS() {
		C2D_FontFree(_font);
		C2D_TextBufDelete(_buff);
	}

	double Font3DS::getWidth(const std::string& str) const {
		float width, height;
		C2D_Text text;
		C2D_TextFontParse(&text, _font, _buff, str.c_str());
		C2D_TextGetDimensions(&text, 1, 1, &width, &height);
		C2D_TextBufClear(_buff);
		return width;
	}

	double Font3DS::getHeight() const {
		return _size + _size * 4.0/32.0;
	}

	void Font3DS::draw(const Color& color, const Point& position, const Alignment alignment, const std::string& str) const {
		C2D_Text text;
		auto temp = position;
		const auto c = C2D_Color32(color.r, color.g, color.b, color.a);
		C2D_TextFontParse(&text, _font, _buff, str.c_str());
		C2D_TextOptimize(&text);
		float width, height;
		C2D_TextGetDimensions(&text, 1, 1, &width, &height);
		if (alignment == Alignment::LEFT) temp.x = position.x;
		if (alignment == Alignment::CENTER) temp.x = position.x - width / 2;
		if (alignment == Alignment::RIGHT) temp.x = position.x - width;
		C2D_DrawText(&text, C2D_WithColor, static_cast<float>(temp.x), static_cast<float>(temp.y), 0, 1, 1, c);
		C2D_TextBufClear(_buff);
	}
}
