// SPDX-FileCopyrightText: 2025 AJ Walter, see driver/3DS/README.md for MIT usage.
// SPDX-License-Identifier: GPL-3.0-or-later OR MIT

#include "Driver/Screen.hpp"

#include "Core/Structs.hpp"

#include <3ds.h>
#include <citro2d.h>

namespace SuperHaxagon {
	struct Screen::ScreenImpl {
		explicit ScreenImpl(bool wide, bool debugConsole) {
			gfxSetWide(wide);

			C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
			C2D_Init(C2D_DEFAULT_MAX_OBJECTS);
			C2D_Prepare();

			top = C2D_CreateScreenTarget(GFX_TOP, GFX_LEFT);

			if (!debugConsole) {
				// Use the bottom screen for drawing if there's no debug console there
				bot = C2D_CreateScreenTarget(GFX_BOTTOM, GFX_LEFT);
			}

			// Clear screen
			C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
			C2D_TargetClear(top, C2D_Color32(0x00, 0x00, 0x00, 0xff));
			C2D_SceneBegin(top);

			if (!debugConsole) {
				// Clear the bottom if no debug console is present
				C2D_TargetClear(bot, C2D_Color32(0x00, 0x00, 0x00, 0xff));
				C2D_SceneBegin(bot);
			}

			C3D_FrameEnd(0);
		}

		~ScreenImpl() {
			C2D_Fini();
			C3D_Fini();
		}

		void screenSwitch() {
			// Don't switch to the bottom screen in debug console mode
			if (debugConsole)  return;

			drawingOnTop = false;
			C2D_TargetClear(bot, C2D_Color32(0x00, 0x00, 0x00, 0xff));
			C2D_SceneBegin(bot);
		}

		void drawPoly(const Color& color, const std::vector<Vec2f>& points) {
			const auto c = C2D_Color32(color.r, color.g, color.b, color.a);
			for (size_t i = 1; i < points.size() - 1; i++) {
				C2D_DrawTriangle(
					static_cast<float>(points[0].x), static_cast<float>(points[0].y), c,
					static_cast<float>(points[i].x), static_cast<float>(points[i].y), c,
					static_cast<float>(points[i + 1].x), static_cast<float>(points[i + 1].y), c,
					0
				);
			}
		}

		void clear(const Color& color) {
			C2D_TargetClear(top, C2D_Color32(color.r, color.g, color.b, 0xff));
			C2D_SceneBegin(top);
		}

		C3D_RenderTarget* top = nullptr;
		C3D_RenderTarget* bot = nullptr;

		bool drawingOnTop = true;
		bool debugConsole = false;
	};

	Screen::Screen(std::unique_ptr<ScreenImpl> impl) : _impl(std::move(impl)) {}

	Screen::~Screen() = default;

	Vec2f Screen::getScreenDim() const {
		return {static_cast<float>(_impl->drawingOnTop ? 400 : 320), 240};
	}

	void Screen::screenBegin() const {
		_impl->drawingOnTop = true;
		C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
	}

	void Screen::screenSwitch() const {
		_impl->screenSwitch();
	}

	void Screen::screenFinalize() const {
		C3D_FrameEnd(0);
	}

	void Screen::drawPoly(const Color& color, const std::vector<Vec2f>& points) const {
		_impl->drawPoly(color, points);
	}

	void Screen::clear(const Color& color) const {
		_impl->clear(color);
	}

	std::unique_ptr<Screen> createScreen(bool wide, bool debugConsole) {
		return std::make_unique<Screen>(std::make_unique<Screen::ScreenImpl>(wide, debugConsole));
	}
}
