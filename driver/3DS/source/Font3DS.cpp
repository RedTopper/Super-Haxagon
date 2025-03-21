// SPDX-FileCopyrightText: 2020 AJ Walter, see driver/3DS/README.md for MIT usage.
// SPDX-License-Identifier: GPL-3.0-or-later OR MIT

#include "Driver/Font.hpp"

#include "Core/Structs.hpp"

#include <3ds/font.h>
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
	struct Font::FontImpl {
		FontImpl(const std::string& path, int size) : size(size) {
			buff = C2D_TextBufNew(128);
			font = C2D_FontLoad(path.c_str());

			// For some reason the latest version of citro2d messes with font scaling,
			// causing all sorts of issues. Manually set scale to be 1.0.
			font->textScale = 1.0f;
		}

		~FontImpl() {
			C2D_FontFree(font);
			C2D_TextBufDelete(buff);
		}

		float getWidth(const std::string& str) {
			float width, height;
			C2D_Text text;
			C2D_TextFontParse(&text, font, buff, str.c_str());
			C2D_TextGetDimensions(&text, 1.0f, 1.0f, &width, &height);
			C2D_TextBufClear(buff);
			return width;
		}

		void draw(const Color& color, const Vec2f& position, const Alignment alignment, const std::string& str) const {
			C2D_Text text;
			const auto c = C2D_Color32(color.r, color.g, color.b, color.a);
			C2D_TextFontParse(&text, font, buff, str.c_str());
			C2D_TextOptimize(&text);
			int flags = C2D_WithColor;
			if (alignment == Alignment::LEFT) flags |= static_cast<int>(C2D_AlignLeft);
			if (alignment == Alignment::CENTER) flags |= static_cast<int>(C2D_AlignCenter);
			if (alignment == Alignment::RIGHT) flags |= static_cast<int>(C2D_AlignRight);
			C2D_DrawText(&text, flags, std::round(position.x), std::round(position.y), 0, 1.0f, 1.0f, c);
			C2D_TextBufClear(buff);
		}

		C2D_Font font;
		C2D_TextBuf buff;
		int size;
	};

	Font::Font(std::unique_ptr<Font::FontImpl> impl) : _impl(std::move(impl)) {}

	Font::~Font() = default;

	void Font::setScale(const float) {}

	float Font::getWidth(const std::string& str) const {
		return _impl->getWidth(str);
	}

	float Font::getHeight() const {
		return _impl->size * 36.0f/32.0f;
	}

	void Font::draw(const Color& color, const Vec2f& position, const Alignment alignment, const std::string& str) const {
		_impl->draw(color, position, alignment, str);
	}

	std::unique_ptr<Font> createFont(const std::string& path, int size) {
		return std::make_unique<Font>(std::make_unique<Font::FontImpl>(path, size));
	}
}
