// SPDX-FileCopyrightText: 2025 AJ Walter
// SPDX-License-Identifier: GPL-3.0-or-later

#include "Core/SurfaceGame.hpp"

#include "Driver/Screen.hpp"
#include "Factories/PatternFactory.hpp"

#include <cmath>
#include <algorithm>

namespace SuperHaxagon {
	SurfaceGame::SurfaceGame(Screen& screen, Camera& camera) : _screen(screen), _camera(camera) {}

	void SurfaceGame::project(const Color& color, std::vector<Vec2f>& points) {
		for (auto& p : points) {
			Vec3f point(p);
			point.z = _depth;
			auto res = _matrix * point;
			if (res.z > 1.0f || res.z < -1.0f) return;
			p.x = res.x;
			p.y = res.y;
		}

		drawPolyGame(color, points);
	}

	void SurfaceGame::drawPolyGame(const Color& color, std::vector<Vec2f>& points) {
		toScreenSpace(points);
		_screen.drawPoly(color, points);
	}

	void SurfaceGame::drawBackground(const Color& color1, const Color& color2, const float sides) {
		const auto exactSides = static_cast<size_t>(std::ceil(sides));

		//solid background.
		_screen.clear(color1);

		//This draws the main background.
		std::vector<Vec2f> edges;
		edges.resize(exactSides);

		for(size_t i = 0; i < exactSides; i++) {
			// Screen space is from [-1.0, 1.0], but zoom transforms may be applied,
			// so draw it out to 2.0 to be safe
			edges[i].x = 2.0f * std::cos(static_cast<float>(i) * TAU / sides);
			edges[i].y = 2.0f * std::sin(static_cast<float>(i) * TAU / sides + PI);
		}

		std::vector<Vec2f> triangle;
		triangle.resize(3);

		//if the sides is odd we need to "make up a color" to put in the gap between the last and first color
		if(exactSides % 2) {
			triangle[0] = {0, 0};
			triangle[1] = edges[exactSides - 1];
			triangle[2] = edges[0];
			this->project(interpolateColor(color1, color2, 0.5f), triangle);
		}

		//Draw the rest of the triangles
		for(size_t i = 0; i < exactSides - 1; i = i + 2) {
			triangle[0] = {0, 0};
			triangle[1] = edges[i];
			triangle[2] = edges[i + 1];
			this->project(color2, triangle);
		}
	}

	void SurfaceGame::drawRegular(const Color& color, const float radius, const float sides) {
		const auto exactSides = static_cast<size_t>(std::ceil(sides));

		std::vector<Vec2f> edges;
		edges.resize(exactSides);

		// Calculate the triangle backwards so it overlaps correctly.
		for(size_t i = 0; i < exactSides; i++) {
			edges[i].x = radius * std::cos(static_cast<float>(i) * TAU/sides);
			edges[i].y = radius * std::sin(static_cast<float>(i) * TAU/sides + PI);
		}

		this->project(color, edges);
	}

	void SurfaceGame::drawCursor(const Color& color, const float radius, const float cursor) {
		// Note: A cursor and rotation of zero points to the left
		std::vector<Vec2f> triangle;
		triangle.resize(3);
		triangle[0] = {radius, -PLAYER_TRI_WIDTH / 2};
		triangle[1] = {radius, PLAYER_TRI_WIDTH / 2};
		triangle[2] = {radius + PLAYER_TRI_HEIGHT, 0};
		for (auto& p : triangle) {
			const auto orig = rotateAroundOrigin(p, cursor);
			p.x = orig.x;
			p.y = orig.y;
		}

		this->project(color, triangle);
	}

	void SurfaceGame::drawPatterns(const Color& color, const std::deque<Pattern>& patterns, const float sides) {
		for(const auto& pattern : patterns) {
			for(const auto& wall : pattern.getWalls()) {
				this->drawWalls(color, wall, sides);
			}
		}
	}

	void SurfaceGame::drawWalls(const Color& color, const Wall& wall, const float sides) {
		if(wall.getDistance() + wall.getHeight() < HEX_LENGTH) return; //TOO_CLOSE;
		if(static_cast<float>(wall.getSide()) >= sides) return; //NOT_IN_RANGE
		auto trap = wall.calcPoints(sides, _offset);
		this->project(color, trap);
	}

	void SurfaceGame::drawDebugTriangles() {
		constexpr float sideLength = 0.05f;
		std::vector<Color> colors {
			Color{255, 0, 0, 255},
			Color{0, 255, 0, 255},
			Color{0, 0, 255, 255},
			Color{255, 255, 255, 255},
		};

		std::vector<Vec2f> rightAngles = {
			{-1.0f, -0.6f}, // bottom left (Red)
			{-1.0f, 0.6f}, // top left (Green)
			{1.0f, -0.6f}, // bottom right (Blue)
			{1.0f, 0.6f} // top right (White)
		};

		for (size_t i = 0; i < rightAngles.size(); i++) {
			std::vector<Vec2f> triangle;
			const auto& pos = rightAngles[i];
			const auto& color = colors[i];
			triangle.resize(3);
			triangle[0] = pos;
			triangle[1] = {pos.x - sideLength * (pos.x > 0 ? 1.0f : -1.0f), pos.y};
			triangle[2] = {pos.x, pos.y - sideLength * (pos.y > 0 ? 1.0f : -1.0f)};
			this->project(color, triangle);
		}
	}

	void SurfaceGame::toScreenSpace(std::vector<Vec2f>& points) {
		const auto screen = _screen.getScreenDim();
		for(auto& p : points) {
			p.x = std::floor(((p.x + 1.0f) / 2.0f) * screen.x);
			p.y = std::floor(((-p.y + 1.0f) / 2.0f) * screen.y);
		}
	}

	void SurfaceGame::calculateMatrix(float rotation) {
		constexpr auto fov = 15.5f * PI / 180.0f;
		const auto project = Matrix4x4f::generateProjection(fov, ASPECT_DEFAULT, 0.01f, 10.0f);
		const auto camera = Matrix4x4f::lookAt(
				_camera.currentPos(),
				_camera.currentLookAt(),
				{0, 1.0f, 0}
		);

		// We only need to rotate around Z and scale along X and Y
		// Initialized as identity matrix
		Matrix4x4f model{};
		model[0][0] = std::cos(rotation);
		model[0][1] = -std::sin(rotation);
		model[1][0] = std::sin(rotation);
		model[1][1] = std::cos(rotation);

		const auto scale = _camera.currentPos(CameraLayer::SCALE);
		Matrix4x4f scaleMat{};
		scaleMat[0][0] = scale.x + 1.0f;
		scaleMat[1][1] = scale.y + 1.0f;

		// Create a scaling matrix that clamps the aspect ratio
		const auto screen = _screen.getScreenDim();
		const float aspect = screen.x / screen.y;
		const float scaleX = (aspect > ASPECT_DEFAULT)
		                     ? (ASPECT_DEFAULT / std::min(aspect, ASPECT_MAX))
		                     : (aspect < ASPECT_MIN ? (ASPECT_MIN / aspect) : 1.0f);
		const float scaleY = (aspect < ASPECT_DEFAULT)
		                     ? ((std::max(aspect, ASPECT_MIN)) / ASPECT_DEFAULT)
		                     : (aspect >= ASPECT_MAX ? (aspect / ASPECT_MAX) : 1.0f);

		Matrix4x4f aspectMatrix{};
		aspectMatrix[0][0] = scaleX;
		aspectMatrix[1][1] = scaleY;

		_matrix = (model * (scaleMat * (camera * (project * aspectMatrix))));
	}

	void SurfaceGame::copyMatrix(const SurfaceGame& other) {
		_matrix = other._matrix;
	}

	void SurfaceGame::setWallOffset(float offset) {
		_offset = offset;
	}

	void SurfaceGame::setDepth(float depth) {
		_depth = depth;
	}
}
