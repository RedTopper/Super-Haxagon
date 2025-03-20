// SPDX-FileCopyrightText: 2025 AJ Walter
// SPDX-License-Identifier: GPL-3.0-or-later

#include "Core/SurfaceUI.hpp"

#include "Driver/Screen.hpp"

namespace SuperHaxagon {
	SurfaceUI::SurfaceUI(Screen& screen) : _screen(screen) {}

	void SurfaceUI::drawPolyUI(const Color& color, const std::vector<Vec2f>& points) {
		// Passthrough to underlying surface
		_screen.drawPoly(color, points);
	}

	void SurfaceUI::drawPolyUITopLeft(const Color& color, const Vec2f& dim) {
		std::vector<Vec2f> poly{
				{0, 0},
				{dim.x + dim.y / 2, 0},
				{dim.x, dim.y},
				{0, dim.y}
		};

		drawPolyUI(color, poly);
	}

	void SurfaceUI::drawPolyUIBottomLeft(const SuperHaxagon::Color &color, const SuperHaxagon::Vec2f &dim) {
		const float screenHeight = getScreenDim().y;
		std::vector<Vec2f> poly{
				{0, screenHeight - dim.y},
				{dim.x,  screenHeight - dim.y},
				{dim.x + dim.y / 2, screenHeight},
				{0,  screenHeight},
		};

		drawPolyUI(color, poly);
	}

	void SurfaceUI::drawPolyUITopRight(const SuperHaxagon::Color &color, const SuperHaxagon::Vec2f &dim) {
		const float screenWidth = getScreenDim().x;
		// Clockwise, from top left
		const std::vector<Vec2f> poly{
				{screenWidth - dim.x - dim.y / 2, 0},
				{screenWidth, 0},
				{screenWidth, dim.y},
				{screenWidth - dim.x, dim.y}
		};

		drawPolyUI(color, poly);
	}

	void SurfaceUI::drawPolyUIBottomRight(const SuperHaxagon::Color &color, const SuperHaxagon::Vec2f &dim) {
		const auto screen = getScreenDim();
		std::vector<Vec2f> poly{
				{screen.x - dim.x, screen.y - dim.y},
				{screen.x, screen.y - dim.y},
				{screen.x, screen.y},
				{screen.x - dim.x - dim.y / 2, screen.y},
		};

		drawPolyUI(color, poly);
	}

	void SurfaceUI::drawRectUI(const Color color, const Vec2f position, const Vec2f size) {
		const std::vector<Vec2f> points{
				{position.x, position.y},
				{position.x + size.x, position.y},
				{position.x + size.x, position.y + size.y},
				{position.x, position.y + size.y},
		};

		this->drawPolyUI(color, points);
	}

	Vec2f SurfaceUI::getScreenDim() const {
		return _screen.getScreenDim();
	}

	float SurfaceUI::getScale() const {
		// The original game was built with a 3DS in mind, so when
		// drawing we have to scale the game to however many times larger the viewport is.
		return this->getScreenDimMin() / 240.0f;
	}

	float SurfaceUI::getScreenDimMax() const {
		const auto size = this->getScreenDim();
		return std::max(size.x, size.y);
	}

	float SurfaceUI::getScreenDimMin() const {
		const auto size = this->getScreenDim();
		return std::min(size.x, size.y);
	}
}