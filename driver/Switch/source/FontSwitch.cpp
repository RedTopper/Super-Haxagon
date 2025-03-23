// SPDX-FileCopyrightText: 2020 AJ Walter
// SPDX-License-Identifier: GPL-3.0-or-later OR MIT

#include "Driver/Font.hpp"

#include "RenderTarget.hpp"
#include "Driver/Platform.hpp"

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
	static constexpr int GLYPH_START = 32;
	static constexpr int GLYPH_END = 128;

	struct CharacterInfo {
		Vec2f pxAdvance;
		Vec2f pxOffset;
		Vec2f pxDim;
		Vec2f uv;
		float x;
	};

	struct Font::FontImpl {
		FontImpl(const Platform& platform, const std::string& path, const float size, float* z) :
			platform(platform),
			z(z) {

			const auto filename = path + ".ttf";

			// Use a new FreeType2 library per font
			FT_Library ft;
			if (FT_Init_FreeType(&ft)) {
				platform.message(Dbg::FATAL, "font", "FreeType2 failed to load");
				return;
			}

			FT_Face face;
			auto error = FT_New_Face(ft, filename.c_str(), 0, &face);
			if (error) {
				platform.message(Dbg::FATAL, "font", "could not load font " + filename);
				platform.message(Dbg::FATAL, "font", "error: " + std::to_string(error));
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

			texWidth = w;
			texHeight = h;
			top = t > 0 ? t : 0;

			surface = std::make_shared<RenderTarget<VertexUV>>(platform, true, vertex_shader, fragment_shader, path);
			surface->bind();

			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, w, h, 0, GL_RED, GL_UNSIGNED_BYTE, nullptr);

			auto x = 0;
			for(auto i = GLYPH_START; i < GLYPH_END; i++) {
				if(FT_Load_Char(face, i, FT_LOAD_RENDER)) continue;

				glTexSubImage2D(GL_TEXTURE_2D, 0, x, 0, g->bitmap.width, g->bitmap.rows,  GL_RED, GL_UNSIGNED_BYTE, g->bitmap.buffer);
				chars[i].pxAdvance = {
						g->advance.x / 64.0f,
						g->advance.y / 64.0f
				};

				chars[i].pxOffset = {
						static_cast<float>(g->bitmap_left),
						static_cast<float>(g->bitmap_top)
				};

				chars[i].pxDim = {
						static_cast<float>(g->bitmap.width),
						static_cast<float>(g->bitmap.rows)
				};

				chars[i].uv = {
						static_cast<float>(x) / static_cast<float>(texWidth),
						chars[i].pxDim.y / static_cast<float>(texHeight)
				};

				x += g->bitmap.width + 1;
			}

			FT_Done_Face(face);
			FT_Done_FreeType(ft);

			loaded = true;

			platform.message(Dbg::INFO, "font", "loaded font " + filename);
		}

		float getWidth(const std::string& text) const {
			auto width = 0.0;
			for (auto c : text) {
				const auto i = static_cast<int>(c);
				width += chars[i].pxAdvance.x;
			}

			return width;
		}

		void draw(const Color& color, const Vec2f& position, Alignment alignment, const std::string& text) const {
			if (!loaded) return;

			Vec2f cursor = {
					position.x,
					position.y + static_cast<float>(top)
			};

			const auto width = getWidth(text);
			if (alignment == Alignment::LEFT) cursor.x = position.x;
			if (alignment == Alignment::CENTER) cursor.x = position.x - width / 2;
			if (alignment == Alignment::RIGHT) cursor.x = position.x - width;

			const auto tz = *z;
			*z += Z_STEP;

			for (auto c : text) {

				const auto i = static_cast<int>(c);
				const Vec2f draw = {
						std::round(cursor.x + chars[i].pxOffset.x),
						std::round(cursor.y - chars[i].pxOffset.y)
				};

				const auto dim = chars[i].pxDim;
				const auto uv = chars[i].uv;

				cursor.x += chars[i].pxAdvance.x;
				cursor.y -= chars[i].pxAdvance.y;

				// Cannot render empty characters
				if(!dim.x || !dim.y) continue;

				surface->insert({ {draw.x, draw.y + dim.y}, {uv.x, uv.y}, color, tz }); // BL
				surface->insert({{draw.x, draw.y}, {uv.x, 0}, color, tz}); // TL
				surface->insert({{draw.x + dim.x, draw.y}, {uv.x + dim.x / texWidth, 0}, color, tz}); // TR
				surface->insert({{draw.x + dim.x, draw.y + dim.y}, {uv.x + dim.x / texWidth, uv.y}, color, tz}); // BR

				// Insert clockwise
				surface->reference(0);
				surface->reference(1);
				surface->reference(2);
				surface->reference(0);
				surface->reference(2);
				surface->reference(3);

				surface->advance(4);
			}
		}

		const Platform& platform;

		bool loaded = false;
		unsigned int texWidth;
		unsigned int texHeight;
		unsigned int top; // from baseline to top

		std::shared_ptr<RenderTarget<VertexUV>> surface;

		CharacterInfo chars[GLYPH_END]{};

		float* z;
	};

	Font::Font(std::unique_ptr<Font::FontImpl> impl) : _impl(std::move(impl)) {}

	Font::~Font() = default;

	void Font::setScale(float) {}

	float Font::getHeight() const {
		return static_cast<float>(_impl->top);
	}

	float Font::getWidth(const std::string& text) const {
		return _impl->getWidth(text);
	}

	void Font::draw(const Color& color, const Vec2f& position, Alignment alignment, const std::string& text) const {
		_impl->draw(color, position, alignment, text);
	}

	std::unique_ptr<Font> createFont(const Platform& platform, const std::string& path, const int size, float* z, std::shared_ptr<RenderTarget<VertexUV>>& surface) {
		auto data = std::make_unique<Font::FontImpl>(platform, path, size, z);
		surface = data->surface; // return the surface back to the platform for rendering
		return std::make_unique<Font>(std::move(data));
	}
}
