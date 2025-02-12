#include "Driver/Font.hpp"

#include "Core/Structs.hpp"

#include <citro2d.h>

#include <sstream>

// Naughty naughty re-defining internal structures!
struct C2D_Font_s
{
	CFNT_s* cfnt;
	C3D_Tex* glyphSheets;
	float textScale;
};

namespace SuperHaxagon {
	struct Font::FontData {
		FontData(const std::string& path, int size) : size(size) {
			buff = C2D_TextBufNew(4096);
			font = C2D_FontLoad(path.c_str());

			// For some reason the latest version of citro2d messes with font scaling,
			// causing all sorts of issues. Manually set scale to be 1.0.
			font->textScale = 1.0f;
		}

		~FontData() {
			C2D_FontFree(font);
			C2D_TextBufDelete(buff);
		}

		C2D_Font font;
		C2D_TextBuf buff;
		int size;
	};


	std::unique_ptr<Font> createFont(const std::string& path, int size) {
		return std::make_unique<Font>(std::make_unique<Font::FontData>(path, size));
	}

	Font::Font(std::unique_ptr<Font::FontData> data) : _data(std::move(data)) {}

	Font::~Font() = default;

	void Font::setScale(const float) {}

	float Font::getWidth(const std::string& str) const {
		float width, height;
		C2D_Text text;
		C2D_TextFontParse(&text, _data->font, _data->buff, str.c_str());
		C2D_TextGetDimensions(&text, 1.0f, 1.0f, &width, &height);
		C2D_TextBufClear(_data->buff);
		return width;
	}

	float Font::getHeight() const {
		return _data->size + _data->size * 4.0f/32.0f;
	}

	void Font::draw(const Color& color, const Point& position, const Alignment alignment, const std::string& str) const {
		C2D_Text text;
		const auto c = C2D_Color32(color.r, color.g, color.b, color.a);
		C2D_TextFontParse(&text, _data->font, _data->buff, str.c_str());
		C2D_TextOptimize(&text);
		int flags = C2D_WithColor;
		if (alignment == Alignment::LEFT) flags |= static_cast<int>(C2D_AlignLeft);
		if (alignment == Alignment::CENTER) flags |= static_cast<int>(C2D_AlignCenter);
		if (alignment == Alignment::RIGHT) flags |= static_cast<int>(C2D_AlignRight);
		C2D_DrawText(&text, flags, position.x, position.y, 0, 1.0f, 1.0f, c);
		C2D_TextBufClear(_data->buff);
	}
}
