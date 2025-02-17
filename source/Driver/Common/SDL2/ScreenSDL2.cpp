#include "Driver/Screen.hpp"

#include "Driver/Platform.hpp"

#include "Core/Structs.hpp"

#include <SDL2/SDL.h>

namespace SuperHaxagon {
	struct Screen::ScreenImpl {
		explicit ScreenImpl(const Platform& platform) {
			SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
			SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 2);

			window = SDL_CreateWindow("Super Haxagon", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 0, 0, SDL_WINDOW_FULLSCREEN_DESKTOP);
			if (!window) {
				platform.message(Dbg::FATAL, "screen", "could not init sfml window!");
				return;
			}

			renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
			if (!renderer) {
				// fallback to software
				platform.message(Dbg::WARN, "screen", "falling back to software rendering");
				renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
				software = true;
				if (!renderer) {
					platform.message(Dbg::FATAL, "screen", "... and it failed!");
					return;
				}
			}

			SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
			SDL_RenderClear(renderer);
			SDL_ShowCursor(SDL_DISABLE);
			SDL_RenderPresent(renderer);

			loaded = true;
		}

		~ScreenImpl() {
			SDL_DestroyRenderer(renderer);
			SDL_DestroyWindow(window);
		}

		Vec2f getScreenDim() const {
			if (!loaded) return {640, 480};

			int w, h;
			SDL_GetRendererOutputSize(renderer, &w, &h);
			return {static_cast<float>(w), static_cast<float>(h)};
		}

		void screenFinalize() const {
			if (!loaded) return;
			SDL_RenderPresent(renderer);
		}

		void drawPoly(const Color& color, const std::vector<Vec2f>& points) const {
			if (!loaded) return;

			std::vector<SDL_Vertex> vertices;
			std::vector<int> references;

			// Convert to SDL2 format
			for (const auto& point : points) {
				vertices.emplace_back(SDL_Vertex{
						SDL_FPoint{point.x, point.y}, SDL_Color{color.r, color.g, color.b, 255}, SDL_FPoint{0, 0}
				});
			}

			// Fan triangles
			for (size_t i = 1; i < points.size() - 1; i++) {
				references.emplace_back(0);
				references.emplace_back(i);
				references.emplace_back(i + 1);
			}

			SDL_RenderGeometry(
				renderer,
				nullptr,
				vertices.data(),
				static_cast<int>(vertices.size()),
				references.data(),
				static_cast<int>(references.size())
			);
		}

		void clear(const Color& color) const {
			if (!loaded) return;

			SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, 255);
			SDL_RenderClear(renderer);
		}

		bool loaded = false;
		bool software = false;
		SDL_Window* window;
		SDL_Renderer* renderer;
	};

	Screen::Screen(std::unique_ptr<ScreenImpl> data) : _impl(std::move(data)) {}

	Screen::~Screen() = default;

	Vec2f Screen::getScreenDim() const {
		return _impl->getScreenDim();
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

	std::unique_ptr<Screen> createScreen(const Platform& platform) {
		return std::make_unique<Screen>(std::make_unique<Screen::ScreenImpl>(platform));
	}
}
