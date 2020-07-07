#include <sstream>

#include "Core/Structs.hpp"
#include "Driver3DS/Font3DS.hpp"

namespace SuperHaxagon {
	Font3DS::Font3DS(const std::string& path, int size, C2D_TextBuf& buff) : size(size), buff(buff) {
		std::stringstream s;
		s << path << "-" << size << ".bcfnt";
		font = C2D_FontLoad(s.str().c_str());
	}

	Font3DS::~Font3DS() {
		C2D_FontFree(font);
		C2D_TextBufDelete(buff);
	}

	double Font3DS::getWidth(const std::string& str) const {
		float width, height;
		C2D_Text text;
		C2D_TextFontParse(&text, font, buff, str.c_str());
		C2D_TextGetDimensions(&text, 1, 1, &width, &height);
		C2D_TextBufClear(buff);
		return width;
	}

	double Font3DS::getHeight() const {
		return size + size*4/32;
	}

	void Font3DS::draw(const Color& color, const Point& position, Alignment alignment, const std::string& str) const {
		C2D_Text text;
		Point temp = position;
		auto c = C2D_Color32(color.r, color.g, color.b, color.a);
		C2D_TextFontParse(&text, font, buff, str.c_str());
		C2D_TextOptimize(&text);
		float width, height;
		C2D_TextGetDimensions(&text, 1, 1, &width, &height);
		if (alignment == Alignment::LEFT) temp.x = position.x;
		if (alignment == Alignment::CENTER) temp.x = position.x - width / 2;
		if (alignment == Alignment::RIGHT) temp.x = position.x - width;
		C2D_DrawText(&text, C2D_WithColor, temp.x, temp.y, 0, 1, 1, c);
		C2D_TextBufClear(buff);
	}
}
