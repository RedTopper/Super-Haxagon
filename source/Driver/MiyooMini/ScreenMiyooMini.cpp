#include "Driver/Screen.hpp"

#include "Core/Configuration.hpp"
#include "Core/Structs.hpp"

#include <SDL2/SDL.h>

const int width = 640;
const int height = 480;

namespace SuperHaxagon {
	typedef Vec2<int> Vec2i;

	// Thanks to https://fgiesen.wordpress.com/2013/02/10/optimizing-the-basic-rasterizer/

	inline int orient2d(const Vec2i& a, const Vec2i& b, const Vec2i& c) {
		return (b.x-a.x)*(c.y-a.y) - (b.y-a.y)*(c.x-a.x);
	}

	// Assumes that the pixel is within the bounds of the surface.
	inline void putPixel2x2(SDL_Surface *surface, int x, int y, Uint64 color64) {
		int baseIndex64 = (y * width + x);
		uint64_t* pixels = (uint64_t *)surface->pixels;
		pixels[baseIndex64] = color64;
		pixels[baseIndex64 + width/2] = color64;
	}

	inline void putPixel(SDL_Surface *surface, int x, int y, Uint32 color) {
		Uint32 *pixels = (Uint32 *)surface->pixels;
		pixels[y * width + x] = color;
	}

	void drawTriangle(SDL_Surface *surface, Vec2i size, Vec2i v0, Vec2i v1, Vec2i v2, Uint32 color) {
		// I hate #ifdefs, but we're working with speed and I still wanted an
		// option at compile time to disable the 2x2 pixel filter.
		#ifdef RENDER_QUARTER_RES
		uint64_t color64 = static_cast<uint64_t>(color) | static_cast<uint64_t>(color) << 32;
		#endif

		// Compute triangle bounding box
		int minX = SDL_min(SDL_min(v0.x, v1.x), v2.x);
		int minY = SDL_min(SDL_min(v0.y, v1.y), v2.y);
		int maxX = SDL_max(SDL_max(v0.x, v1.x), v2.x);
		int maxY = SDL_max(SDL_max(v0.y, v1.y), v2.y);

		// Early rejection: If the triangle is completely off-screen, skip rendering
		if (maxX < 0 || minX >= size.x || maxY < 0 || minY >= size.y) {
			return;
		}

		// Clamp bounding box to screen dimensions
		minX = SDL_max(minX, 0);
		minY = SDL_max(minY, 0);
		maxX = SDL_min(maxX, size.x - 1);
		maxY = SDL_min(maxY, size.y - 1);

		// Compute edge function deltas
		int A01 = v0.y - v1.y, B01 = v1.x - v0.x;
		int A12 = v1.y - v2.y, B12 = v2.x - v1.x;
		int A20 = v2.y - v0.y, B20 = v0.x - v2.x;

		// Barycentric coordinates at minX/minY corner
		Vec2i p = { minX, minY };
		int w0_row = orient2d(v1, v2, p);
		int w1_row = orient2d(v2, v0, p);
		int w2_row = orient2d(v0, v1, p);

		// Rasterize
		for (p.y = minY; p.y <= maxY; p.y++) {

			// Barycentric coordinates at start of row
			int w0 = w0_row;
			int w1 = w1_row;
			int w2 = w2_row;

			for (p.x = minX; p.x <= maxX; p.x++) {

				// If p is on or inside all edges, render pixel.
				if ((w0 | w1 | w2) >= 0) {
					#ifdef RENDER_QUARTER_RES
					putPixel2x2(surface, p.x, p.y, color64);
					#else
					putPixel(surface, p.x, p.y, color);
					#endif
				}

				// One step to the right
				w0 += A12;
				w1 += A20;
				w2 += A01;
			}

			// One row step
			w0_row += B12;
			w1_row += B20;
			w2_row += B01;
		}
	}

	struct Screen::ScreenImpl {
		explicit ScreenImpl(SDL_Window* platWindow, SDL_Renderer* platRenderer, SDL_Surface* platSurface) {
			if (!platWindow || !platRenderer || !platSurface) return;
			window = platWindow;
			renderer = platRenderer;
			surface = platSurface;
			context = SDL_GL_CreateContext(window);
			texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB565, SDL_TEXTUREACCESS_STREAMING, width, height);
			loaded = true;
		}

		~ScreenImpl() {
			if (!loaded) return;

			SDL_DestroyTexture(texture);
			SDL_DestroyRenderer(renderer);
		}

		void screenFinalize() const {
			if (!loaded) return;

			SDL_RenderClear(renderer);
			SDL_UpdateTexture(texture, NULL, surface->pixels, surface->pitch);
			SDL_RenderCopy(renderer, texture, NULL, NULL);
			SDL_RenderPresent(renderer);
		}

		void drawPoly(const Color& color, const std::vector<Vec2f>& points) const {
			if (!loaded) return;

			std::vector<Vec2i> vertices;

			#ifdef RENDER_QUARTER_RES
			const Vec2i screenSize = {width/2, height/2};
			for (const auto& point : points) {
				vertices.emplace_back(Vec2i{static_cast<int>(point.x / 2.0f), static_cast<int>(point.y / 2.0f)});
			}
			#else
			const Vec2i screenSize = {width, height};
			for (const auto& point : points) {
				vertices.emplace_back(Vec2i{static_cast<int>(point.x), static_cast<int>(point.y)});
			}
			#endif

			// The Miyoo Mini doesn't have a GPU, so I figured I'd try to target the steward-fu SDL2 build here:
			//
			// https://github.com/steward-fu/sdl2
			//
			// Unfortunately, that SDL2 implementation only allows SDL_TEXTUREACCESS_STREAMING, and SDL_RenderGeometry,
			// which the desktop and PortMaster platforms use, cannot render to streaming textures. SDL_RenderGeometry
			// ALSO does not render to the default render surface, but I already got so far into the port I didn't want
			// to give up.
			//
			// I present, software triangle rasterization! It works, but it's slow. This means the platform doesn't
			// support shadows, and by default I render at half the screen resolution.
			for (size_t i = 1; i < points.size() - 1; i++) {
				drawTriangle(surface, screenSize, vertices[0], vertices[i], vertices[i+1], SDL_MapRGB(surface->format, color.r, color.g, color.b));
			}
		}

		void clear(const Color& color) {
			if (!loaded) return;

			SDL_FillRect(surface, &surface->clip_rect, SDL_MapRGB(surface->format, color.r, color.g, color.b));
			SDL_UpdateTexture(texture, NULL, surface->pixels, surface->pitch);
		}

		bool loaded = false;
		SDL_Window* window = nullptr;
		SDL_Renderer* renderer = nullptr;
		SDL_Texture* texture = nullptr;
		SDL_Surface* surface = nullptr;
		SDL_GLContext context = {0};
	};

	Screen::Screen(std::unique_ptr<ScreenImpl> data) : _impl(std::move(data)) {}

	Screen::~Screen() = default;

	Vec2f Screen::getScreenDim() const {
		return {width, height};
	}

	void Screen::screenBegin() const {}

	void Screen::screenSwitch() const {}

	void Screen::screenFinalize() const {
		_impl->screenFinalize();
	}

	void Screen::drawPoly(const Color& color, const std::vector<Vec2f>& points) const {
		_impl->drawPoly(color, points);
	}

	void Screen::clear(const Color& color) const {
		_impl->clear(color);
	}

	std::unique_ptr<Screen> createScreen(SDL_Window* window, SDL_Renderer* renderer, SDL_Surface* surface) {
		return std::make_unique<Screen>(std::make_unique<Screen::ScreenImpl>(window, renderer, surface));
	}
}
