// SPDX-FileCopyrightText: 2025 AJ Walter
// SPDX-License-Identifier: GPL-3.0-or-later OR MIT

#include "Driver/Screen.hpp"

#include "Core/Structs.hpp"

#include <ngc.h>

namespace SuperHaxagon {
	typedef struct {
		int x;
		int y;
	} Point2D;

	struct Screen::ScreenImpl {
		explicit ScreenImpl(Gc& gc) : gc(gc) {
			gui_gc_begin(gc);
			gui_gc_setColorRGB(gc, 0, 0, 0);
			gui_gc_fillRect(gc, 0, 0, 320, 240);
			gui_gc_finish(gc);
			gui_gc_blit_to_screen(gc);
		}

		void drawPoly(const Color& color, const std::vector<Vec2f>& points) {
			const auto pos = std::make_unique<Point2D[]>(points.size());
			for (size_t i = 0; i < points.size(); i++) {
				pos[i] = {
						static_cast<int>(points[i].x),
						static_cast<int>(points[i].y)
				};
			}

			gui_gc_setColorRGB(gc, color.r, color.g, color.b);
			gui_gc_fillPoly(gc, reinterpret_cast<unsigned*>(pos.get()), points.size());
		}

		void clear(const Color& color) {
			gui_gc_setColorRGB(gc, color.r, color.g, color.b);
			gui_gc_fillRect(gc, 0, 0, 320, 240);
		}

		Gc& gc;
	};

	Screen::Screen(std::unique_ptr<ScreenImpl> impl) : _impl(std::move(impl)) {}

	Screen::~Screen() = default;

	Vec2f Screen::getScreenDim() const {
		return {320, 240};
	}

	void Screen::screenBegin() const {
		gui_gc_begin(_impl->gc);
	}

	// Do nothing since we don't have two screens
	void Screen::screenSwitch() const {}

	void Screen::screenFinalize() const {
		gui_gc_blit_to_screen(_impl->gc);
	}

	void Screen::drawPoly(const Color& color, const std::vector<Vec2f>& points) const {
		_impl->drawPoly(color, points);
	}

	void Screen::clear(const Color& color) const {
		_impl->clear(color);
	}

	Screen createScreen(Gc& gc) {
		return Screen(std::make_unique<Screen::ScreenImpl>(gc));
	}
}
