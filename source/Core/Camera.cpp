#include "Camera.hpp"

namespace SuperHaxagon {
	void Camera::update(float dilation) {
		if (_percentBetween < 1.0f) {
			_percentBetween += dilation / _framesToCompletion;

			if (_percentBetween > 1.0f) _percentBetween = 1.0f;

			_currentPos = _fromPos.ease(_toPos, _percentBetween);
			_currentLookAt = _fromLookAt.ease(_toLookAt, _percentBetween);
		}
	}

	bool Camera::isMoving() const {
		return _locked || _percentBetween < 0.999f;
	}

	void Camera::set(SuperHaxagon::Vec3f pos, SuperHaxagon::Vec3f at) {
		_currentLookAt = at;
		_currentPos = pos;
		_fromLookAt = at;
		_fromPos = pos;
		_toLookAt = at;
		_toPos = pos;
		_framesToCompletion = 0.0f;
		_percentBetween = 1.0f;
	}

	void Camera::setNext(SuperHaxagon::Vec3f pos, SuperHaxagon::Vec3f at, float frames) {
		_fromPos = _currentPos;
		_fromLookAt = _currentLookAt;
		_toPos = pos;
		_toLookAt = at;
		_framesToCompletion = frames;
		_percentBetween = 0.0f;
	}

	void Camera::reset(float frames) {
		_locked = false;
		_tiltDir = 0.0f;
		setNext(
			{0.0f, _currentPos.y, _currentPos.z},
			_currentLookAt,
			frames
		);
	}

	void Camera::tilt(float frames) {
		if (_tiltDir == 0) _tiltDir = _rand.rand() > 0.5f ? 1.0f : -1.0f;
		_tiltDir *= -1.0f;
		_locked = true;
		auto pos = Vec3f{0.0f, _currentPos.y, _currentPos.z} - Vec3f{_tiltDir, 0.0f, 0.0f};
		setNext(
			pos,
			_currentLookAt,
			frames
		);
	}
}