// SPDX-FileCopyrightText: 2025 AJ Walter
// SPDX-License-Identifier: GPL-3.0-or-later OR MIT

#include "Driver/Screen.hpp"

#include <SDL2/SDL.h>

namespace SuperHaxagon {
	struct Screen::ScreenImpl {
		explicit ScreenImpl(SDL_Window* platWindow, SDL_Renderer* platRenderer) {
			window = platWindow;
			renderer = platRenderer;
			loaded = window && renderer;
		}

		~ScreenImpl() = default;

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
			vertices.reserve(points.size());
			references.reserve((points.size()-2)*3);

			// Convert to SDL2 format
			for (const auto& point : points) {
				vertices.emplace_back(SDL_Vertex{
					SDL_FPoint{point.x, point.y}, SDL_Color{color.r, color.g, color.b, color.a}, SDL_FPoint{0, 0}
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

	std::unique_ptr<Screen> createScreen(SDL_Window* window, SDL_Renderer* renderer) {
		return std::make_unique<Screen>(std::make_unique<Screen::ScreenImpl>(window, renderer));
	}
}
