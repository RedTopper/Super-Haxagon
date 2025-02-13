#include "Core/SurfaceGame.hpp"

#include "Driver/Screen.hpp"
#include "Factories/PatternFactory.hpp"

#include <cmath>

namespace SuperHaxagon {
	SurfaceGame::SurfaceGame(Screen& screen) : _screen(screen) {
		_project = Matrix4x4f::generateProjection(22.5, 0.01f, 3.0f);
		_camera[3][1] = 0;
		_camera[3][2] = -4;
	}

	void SurfaceGame::vertexShader(const Color& color, std::vector<Vec2f>& points) {
		for (auto& p : points) {
			Vec3 point = p;
			point.z = _depth;
			auto transform = getMatrix() * point;
			transform = _camera * transform;
			transform = _project * transform;
			p.x = transform.x + _translate.x;
			p.y = transform.y + _translate.y;
		}

		this->drawPolyGame(color, points);
	}

	void SurfaceGame::drawPolyGame(const Color& color, std::vector<Vec2f>& points) {
		toScreenSpace(points);
		_screen.drawPoly(color, points);
	}

	void SurfaceGame::drawRect(const Color color, const Vec2f position, const Vec2f size) {
		std::vector<Vec2f> points{
				{position.x, position.y},
				{position.x, position.y + size.y},
				{position.x + size.x, position.y + size.y},
				{position.x + size.x, position.y},
		};

		this->vertexShader(color, points);
	}

	void SurfaceGame::drawBackground(const Color& color1, const Color& color2, const float sides) {
		const auto exactSides = static_cast<size_t>(std::ceil(sides));

		//solid background.
		_screen.setClearColor(color1);

		//This draws the main background.
		std::vector<Vec2f> edges;
		edges.resize(exactSides);

		for(size_t i = 0; i < exactSides; i++) {
			// Screen space is from [-1.0, 1.0], but zoom transforms may be applied,
			// so draw it out to 2.0 to be safe
			edges[i].x = 2.0f * cos(static_cast<float>(i) * TAU / sides);
			edges[i].y = 2.0f * sin(static_cast<float>(i) * TAU / sides + PI);
		}

		std::vector<Vec2f> triangle;
		triangle.resize(3);

		//if the sides is odd we need to "make up a color" to put in the gap between the last and first color
		if(exactSides % 2) {
			triangle[0] = {0, 0};
			triangle[1] = edges[exactSides - 1];
			triangle[2] = edges[0];
			this->vertexShader(interpolateColor(color1, color2, 0.5f), triangle);
		}

		//Draw the rest of the triangles
		for(size_t i = 0; i < exactSides - 1; i = i + 2) {
			triangle[0] = {0, 0};
			triangle[1] = edges[i];
			triangle[2] = edges[i + 1];
			this->vertexShader(color2, triangle);
		}
	}

	void SurfaceGame::drawRegular(const Color& color, const float radius, const float sides) {
		const auto exactSides = static_cast<size_t>(std::ceil(sides));

		std::vector<Vec2f> edges;
		edges.resize(exactSides);

		// Calculate the triangle backwards so it overlaps correctly.
		for(size_t i = 0; i < exactSides; i++) {
			edges[i].x = radius * cos(static_cast<float>(i) * TAU/sides);
			edges[i].y = radius * sin(static_cast<float>(i) * TAU/sides + PI);
		}

		this->vertexShader(color, edges);
	}

	void SurfaceGame::drawCursor(const Color& color, const float radius, const float cursor) {
		// Note: A cursor and rotation of zero points to the left
		std::vector<Vec2f> triangle;
		triangle.resize(3);
		triangle[0] = {radius, -SCALE_HUMAN_WIDTH/2};
		triangle[1] = {radius, SCALE_HUMAN_WIDTH/2};
		triangle[2] = {radius + SCALE_HUMAN_HEIGHT, 0};
		for (auto& p : triangle) {
			const auto orig = rotateAroundOrigin(p, cursor);
			p.x = orig.x;
			p.y = orig.y;
		}

		this->vertexShader(color, triangle);
	}

	void SurfaceGame::drawPatterns(const Color& color, const std::deque<Pattern>& patterns, const float sides) {
		for(const auto& pattern : patterns) {
			for(const auto& wall : pattern.getWalls()) {
				this->drawWalls(color, wall, sides);
			}
		}
	}

	void SurfaceGame::drawWalls(const Color& color, const Wall& wall, const float sides) {
		if(wall.getDistance() + wall.getHeight() < SCALE_HEX_LENGTH) return; //TOO_CLOSE;
		if(static_cast<float>(wall.getSide()) >= sides) return; //NOT_IN_RANGE
		auto trap = wall.calcPoints(sides, _offset);
		this->vertexShader(color, trap);
	}

	void SurfaceGame::toScreenSpace(std::vector<Vec2f>& points) {
		const auto screen = _screen.getScreenDim();
		for(auto& p : points) {
			// Fix aspect ratio
			if (screen.x > screen.y) {
				p.y *= screen.x / screen.y;
			} else {
				p.x *= screen.y / screen.x;
			}

			p.x = floor(((p.x + 1.0f) / 2.0f) * screen.x);
			p.y = floor(((-p.y + 1.0f) / 2.0f) * screen.y);
		}
	}

	Matrix4x4f SurfaceGame::getMatrix() {
		if (!_recalculate) {
			return _matrix;
		}

		Matrix4x4f pitch = {};
		pitch[1][1] = cos(_pitch);
		pitch[1][2] = sin(_pitch);
		pitch[0][0] = 1.0f;
		pitch[2][1] = -sin(_pitch);
		pitch[2][2] = cos(_pitch);
		pitch[3][3] = 1.0f;
		
		Matrix4x4f rotate = {};
		rotate[0][0] = cos(_rotation);
		rotate[0][1] = sin(_rotation);
		rotate[1][0] = -sin(_rotation);
		rotate[1][1] = cos(_rotation);
		rotate[2][2] = 1.0f;
		rotate[3][3] = 1.0f;
		
		Matrix4x4f zoom = {};
		zoom[0][0] = _zoom;
		zoom[1][1] = _zoom;
		zoom[2][2] = 1.0f;
		zoom[3][3] = 1.0f;

		_matrix = zoom * rotate * pitch;
		_recalculate = false;
		return _matrix;
	}

	void SurfaceGame::copySettings(const SurfaceGame& surface) {
		_rotation = surface._rotation;
		_zoom = surface._zoom;
		_roll = surface._roll;
		_pitch = surface._pitch;
		_offset = surface._offset;
		_depth = surface._depth;
		_translate = surface._translate;
		_recalculate = true;
	}

	void SurfaceGame::setRotation(const float rotation) {
		_rotation = rotation;
		_recalculate = true;
	}

	void SurfaceGame::setZoom(const float zoom) {
		_zoom = zoom;
		_recalculate = true;
	}

	void SurfaceGame::setRoll(const float roll) {
		_roll = roll;
	}

	void SurfaceGame::setPitch(const float pitch) {
		_pitch = pitch;
	}

	void SurfaceGame::setWallOffset(const float offset) {
		_offset = offset;
	}

	void SurfaceGame::setDepth(const float depth) {
		_depth = depth;
	}

	void SurfaceGame::setTranslate(const Vec2f& translate) {
		_translate = translate;
	}

	void SurfaceGame::reset() {
		_rotation = 0.0f;
		_zoom = 1.0f;
		_roll = 0.0f;
		_pitch = 0.0f;
		_offset = 0.0f;
		_depth = 0.0f;
		_translate = {0.0f, 0.0f};
	}
}
