#include "Driver/Switch/FontSwitch.hpp"

#include "Driver/Switch/PlatformSwitch.hpp"

#include <ft2build.h>
#include FT_FREETYPE_H

#include <cmath>

static auto* vertex_shader = R"text(
#version 330 core

layout(location = 0) in vec2 v_position;
layout(location = 1) in vec2 v_uv;
layout(location = 2) in vec4 v_color;
layout(location = 3) in float v_z;

out vec2 f_uv;
out vec4 f_color;

uniform float s_width;
uniform float s_height;

void main() {
	float x_norm = (v_position.x / s_width - 0.5) * 2.0;
	float y_norm = (v_position.y / s_height - 0.5) * -2.0;

	gl_Position = vec4(x_norm, y_norm, v_z, 1.0);
	f_color = v_color;
	f_uv = v_uv;
}
)text";

static auto* fragment_shader = R"text(
#version 330 core

layout(location = 0) out vec4 color;

uniform sampler2D f_tex;
in vec4 f_color;
in vec2 f_uv;

void main() {
	color = vec4(1.0, 1.0, 1.0, texture2D(f_tex, f_uv).r) * f_color;
}
)text";

namespace SuperHaxagon {
	FontSwitch::FontSwitch(PlatformSwitch& platform, const std::string& path, const double size) : _platform(platform) {
		const auto filename = path + ".ttf";

		// Use a new FreeType2 library per font
		FT_Library ft;
		if (FT_Init_FreeType(&ft)) {
			platform.message(Dbg::FATAL, "font", "FreeType2 failed to load");
			return;
		}

		FT_Face face;
		if (FT_New_Face(ft, filename.c_str(), 0, &face)) {
			platform.message(Dbg::FATAL, "font", "could not load font " + filename);
			return;
		}

		FT_Set_Pixel_Sizes(face, 0, size * 2);

		auto t = 0;
		unsigned int w = 0;
		unsigned int h = 0;
		auto* g = face->glyph;
		for(auto i = GLYPH_START; i < GLYPH_END; i++) {
			if(FT_Load_Char(face, i, FT_LOAD_RENDER)) {
				const auto letter = std::to_string(static_cast<char>(i));
				platform.message(Dbg::WARN, "font", "failed to load " + letter);
				continue;
			}

			w += g->bitmap.width + 1;
			h = std::max(h, g->bitmap.rows);
			t = std::max(t, g->bitmap_top);
		}

		_texWidth = w;
		_texHeight = h;
		_top = t > 0 ? t : 0;

		_surface = std::make_shared<RenderTarget<VertexUV>>(platform, true, vertex_shader, fragment_shader, path);
		_surface->bind();

		platform.addRenderTarget(_surface);

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, w, h, 0, GL_RED, GL_UNSIGNED_BYTE, nullptr);

		auto x = 0;
		for(auto i = GLYPH_START; i < GLYPH_END; i++) {
			if(FT_Load_Char(face, i, FT_LOAD_RENDER)) continue;

			glTexSubImage2D(GL_TEXTURE_2D, 0, x, 0, g->bitmap.width, g->bitmap.rows,  GL_RED, GL_UNSIGNED_BYTE, g->bitmap.buffer);
			_chars[i].pxAdvance = {
				g->advance.x / 64.0,
				g->advance.y / 64.0
			};

			_chars[i].pxOffset = {
				static_cast<double>(g->bitmap_left),
				static_cast<double>(g->bitmap_top)
			};

			_chars[i].pxDim = {
				g->bitmap.width + 0.0,
				g->bitmap.rows + 0.0
			};

			_chars[i].uv = {
				(x + 0.0) / _texWidth,
				_chars[i].pxDim.y / _texHeight
			};

			x += g->bitmap.width + 1;
		}

		FT_Done_Face(face);
		FT_Done_FreeType(ft);

		_loaded = true;
	}

	FontSwitch::~FontSwitch() = default;

	double FontSwitch::getHeight() const {
		return _top;
	}

	double FontSwitch::getWidth(const std::string& text) const {
		auto width = 0.0;
		for (auto c : text) {
			const auto i = static_cast<int>(c);
			width += _chars[i].pxAdvance.x;
		}

		return width;
	}

	void FontSwitch::draw(const Color& color, const Point& position, Alignment alignment, const std::string& text) {
		if (!_loaded) return;

		Point cursor = {
			position.x,
			position.y + _top
		};

		const auto width = getWidth(text);
		if (alignment == Alignment::LEFT) cursor.x = position.x;
		if (alignment == Alignment::CENTER) cursor.x = position.x - width / 2;
		if (alignment == Alignment::RIGHT) cursor.x = position.x - width;

		const auto z = _platform.getAndIncrementZ();
		for (auto c : text) {

			const auto i = static_cast<int>(c);
			const Point draw = {
				std::round(cursor.x + _chars[i].pxOffset.x),
				std::round(cursor.y - _chars[i].pxOffset.y)
			};

			const auto dim = _chars[i].pxDim;
			const auto uv = _chars[i].uv;

			cursor.x += _chars[i].pxAdvance.x;
			cursor.y -= _chars[i].pxAdvance.y;

			// Cannot render empty characters
			if(!dim.x || !dim.y) continue;

			_surface->insert({{draw.x, draw.y}, {uv.x, 0}, color, z}); // TL
			_surface->insert({{draw.x + dim.x, draw.y}, {uv.x + dim.x / _texWidth, 0}, color, z}); // TR
			_surface->insert({{draw.x, draw.y + dim.y}, {uv.x, uv.y}, color, z}); // BL
			_surface->insert({{draw.x + dim.x, draw.y + dim.y}, {uv.x + dim.x / _texWidth, uv.y}, color, z}); // BR

			_surface->reference(0);
			_surface->reference(1);
			_surface->reference(2);
			_surface->reference(1);
			_surface->reference(2);
			_surface->reference(3);

			_surface->advance(4);
		}
	}
}
