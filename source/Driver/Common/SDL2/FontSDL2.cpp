#include "Driver/Font.hpp"

#include "Core/Structs.hpp"
#include "Driver/Platform.hpp"

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include <utility>

namespace SuperHaxagon {
	struct Font::FontImpl {
		FontImpl(const Platform& platform, SDL_Renderer* renderer, SDL_Surface* surface, std::string path, int size) :
			platform(platform),
			path(std::move(path)),
			renderer(renderer),
			surface(surface),
			font(nullptr),
			currentScale(1.0f),
			size(size) {
			// font will be created on first call to setScale(..);
		}

		~FontImpl() {
			TTF_CloseFont(font);
		}

		void setScale(float scale) {
			if (currentScale == scale) return;
			currentScale = scale;

			// Scale slower than the true 3DS scale, to look nicer.
			float realScale = (scale - 1.0f) / 1.5f + 1.0f;
			auto realSize = static_cast<int>(realScale * static_cast<float>(size));

			if (font) TTF_CloseFont(font);
			font = TTF_OpenFont(path.c_str(), realSize);

			// Note: Newer versions of SDL2_ttf include TTF_SetFontSize(TTF_Font *font, int ptsize)
			// But to target older devices and SDL2 versions, fall back to closing and re-opening the font.
			platform.message(Dbg::INFO, "font", "font " + std::to_string(size) + "px auto resized to " + std::to_string(realSize));
		}

		float getHeight() const {
			if (!font) return 0;
			int w, h;
			TTF_SizeUTF8(font, "W", &w, &h);
			return static_cast<float>(h);
		}

		float getWidth(const std::string& str) const {
			if (!font) return 0;
			int w, h;
			TTF_SizeUTF8(font, str.c_str(), &w, &h);
			return static_cast<float>(w);
		}

		void draw(const Color& color, const Vec2f& position, const Alignment alignment, const std::string& text) const {
			if (!font) return;
			if (text.empty()) return;

			int text_width;
			int text_height;

			SDL_Surface* fontSurface;
			SDL_Texture* texture = nullptr;
			SDL_Rect rect;
			SDL_Color textColor = {color.r, color.g, color.b, 255};

			fontSurface = TTF_RenderText_Solid(font, text.c_str(), textColor);
			if (!fontSurface) return;

			text_width = fontSurface->w;
			text_height = fontSurface->h;

			rect.w = text_width;
			rect.h = text_height;
			rect.y = static_cast<int>(position.y);

			const auto width = text_width;
			if (alignment == Alignment::LEFT) rect.x = static_cast<int>(position.x);
			if (alignment == Alignment::CENTER) rect.x = static_cast<int>(position.x) - (width / 2);
			if (alignment == Alignment::RIGHT) rect.x = static_cast<int>(position.x) - width;

			if (renderer) {
				texture = SDL_CreateTextureFromSurface(renderer, fontSurface);
				SDL_RenderCopy(renderer, texture, nullptr, &rect);
				SDL_DestroyTexture(texture);
			} else {
				SDL_BlitSurface(fontSurface, nullptr, surface, &rect);
			}

			SDL_FreeSurface(fontSurface);
		}

		const Platform& platform;
		std::string path;
		SDL_Renderer* renderer;
		SDL_Surface* surface;
		TTF_Font* font;
		float currentScale;
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

	std::unique_ptr<Font> createFont(const Platform& platform, SDL_Renderer* renderer, SDL_Surface* surface, const std::string& path, int size) {
		return std::make_unique<Font>(std::make_unique<Font::FontImpl>(platform, renderer, surface, path, size));
	}
}
