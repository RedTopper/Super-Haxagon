#include "Driver/Font.hpp"

#include "Core/Structs.hpp"

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include <cmath>

namespace SuperHaxagon {
	struct Font::FontImpl {
		explicit FontImpl(SDL_Renderer& renderer, const std::string& path, int size) :
			renderer(renderer),
			size(size) {
			font = TTF_OpenFont(path.c_str(), size);
		}

		~FontImpl() {
			TTF_CloseFont(font);
		}

		void setScale(float scale) {
			if (!font) return;
			// Scale slower than the true 3DS scale, to look nicer.
			float realScale = (scale - 1.0f) / 1.5f + 1.0f;
			float realSize = static_cast<float>(size);
			TTF_SetFontSize(font, static_cast<int>(realScale * realSize));
		}

		float getHeight() const {
			if (!font) return 0;
			int w, h;
			TTF_SizeUTF8(font, "W", &w, &h);
			return h;
		}

		float getWidth(const std::string& str) const {
			if (!font) return 0;
			int w, h;
			TTF_SizeUTF8(font, str.c_str(), &w, &h);
			return w;
		}

		void draw(const Color& color, const Vec2f& position, const Alignment alignment, const std::string& text) const {
			if (!font) return;

			int text_width;
			int text_height;
			SDL_Surface *surface;
			SDL_Texture *texture;
			SDL_Rect rect;
			SDL_Color textColor = {color.r, color.g, color.b, 255};
			surface = TTF_RenderText_Solid(font, text.c_str(), textColor);
			texture = SDL_CreateTextureFromSurface(&renderer, surface);

			text_width = surface->w;
			text_height = surface->h;

			SDL_FreeSurface(surface);

			rect.w = text_width;
			rect.h = text_height;
			rect.y = position.y;

			const auto width = text_width;
			if (alignment == Alignment::LEFT) rect.x = position.x;
			if (alignment == Alignment::CENTER) rect.x = position.x - width / 2;
			if (alignment == Alignment::RIGHT) rect.x = position.x - width;

			SDL_RenderCopy(&renderer, texture, NULL, &rect);

			SDL_DestroyTexture(texture);
		}

		SDL_Renderer& renderer;
		TTF_Font* font;
		int size;
	};

	Font::Font(std::unique_ptr<Font::FontImpl> impl) : _impl(std::move(impl)) {}

	Font::~Font() = default;

	void Font::setScale(const float scale) {
		_impl->setScale(scale);
	}

	float Font::getHeight() const {
		return _impl->getHeight();
	}

	float Font::getWidth(const std::string& str) const {
		return _impl->getWidth(str);
	}

	void Font::draw(const Color& color, const Vec2f& position, const Alignment alignment, const std::string& text) const {
		_impl->draw(color, position, alignment, text);
	}

	std::unique_ptr<Font> createFont(SDL_Renderer& renderer, const std::string& path, int size) {
		return std::make_unique<Font>(std::make_unique<Font::FontImpl>(renderer, path, size));
	}
}
