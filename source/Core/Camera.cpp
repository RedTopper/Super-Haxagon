// SPDX-FileCopyrightText: 2025 AJ Walter
// SPDX-License-Identifier: GPL-3.0-or-later

#include "Camera.hpp"

#include <array>

namespace SuperHaxagon {
	constexpr static std::array<CameraLayer, 5> ALL_LAYERS {
		CameraLayer::LOOK_AT,
		CameraLayer::MAIN,
		CameraLayer::SCALE,
		CameraLayer::TILTS,
		CameraLayer::ZOOMS,
	};

	constexpr static std::array<CameraLayer, 3> EFFECT_LAYERS {
		CameraLayer::SCALE,
		CameraLayer::TILTS,
		CameraLayer::ZOOMS,
	};

	constexpr static std::array<CameraLayer, 3> POSITION_LAYERS {
		CameraLayer::MAIN,
		CameraLayer::TILTS,
		CameraLayer::ZOOMS,
	};

	void Camera::update(float dilation) {
		for (const auto& pair : _movements) {
			const auto key = pair.first;

			// Deque a movement if our current movement is null,
			// starting at the last known position
			if (!pair.second && !_queue[key].empty()) {
				_movements[key] = std::move(_queue[key][0]);
				_queue[key].pop_front();
				_movements[key]->start(_position[key]);
			}

			// If we have a movement to process, check if it's done.
			if (_movements[key]) {
				_position[key] = _movements[key]->getCurrent();

				// If it's done, remove it, otherwise update.
				if (_movements[key]->isFinished()) {
					_movements[key] = nullptr;
				} else {
					_movements[key]->update(dilation);
				}
			}
		}
	}

	Camera::Camera() {
		for (const auto layer : ALL_LAYERS) {
			_movements[layer] = nullptr;
			_position[layer] = Vec3f{};
			_queue[layer] = std::deque<std::unique_ptr<Camera::Movement>>{};
		}
	}

	bool Camera::isMoving(CameraLayer layer) {
		return _movements[layer] != nullptr;
	}

	Vec3f Camera::currentPos() {
		Vec3f result{};
		for (auto layer : POSITION_LAYERS) {
			result = result + _position[layer];
		}

		return result;
	}

	Vec3f Camera::currentPos(CameraLayer layer) {
		return _position[layer];
	}

	Vec3f Camera::currentLookAt() {
		return _position[CameraLayer::LOOK_AT];
	}

	void Camera::stopAllEffects() {
		for (auto layer : EFFECT_LAYERS) {
			_queue[layer].clear();
			_movements[layer] = std::make_unique<Camera::Movement>(Vec3f{}, 15.0f);
			_movements[layer]->start(_position[layer]);
		}
	}

	void Camera::reset() {
		for (auto layer : ALL_LAYERS) {
			_queue[layer].clear();
			_movements[layer] = nullptr;
		}
	}

	void Camera::setPosition(CameraLayer layer, Vec3f pos) {
		_position[layer] = pos;
		_movements[layer] = nullptr;
		_queue[layer].clear();
	}

	void Camera::setMovement(CameraLayer layer, Vec3f to, float frames) {
		_queue[layer].clear();
		_movements[layer] = std::make_unique<Camera::Movement>(to, frames);
		_movements[layer]->start(_position[layer]);
	}

	void Camera::queueMovement(CameraLayer layer, Vec3f to, float frames) {
		_queue[layer].push_back(std::make_unique<Camera::Movement>(to, frames));
	}

	void Camera::setMovementRotation(CameraLayer layer, Vec3f anchor, float rotation, float magnitude, float frames) {
		_queue[layer].clear();
		_movements[layer] = std::make_unique<Camera::MovementCircular>(anchor, rotation, magnitude, frames);
		_movements[layer]->start(_position[layer]);
	}

	Camera::Movement::Movement(Vec3f to, float frames) :
			_percentStep(1.0f / frames),
			_to(to) {
	}

	void Camera::Movement::start(Vec3f from) {
		_from = from;
		_percentBetween = 0.0f;
	}

	Vec3f Camera::Movement::getCurrent() const {
		return _from.ease(_to, _percentBetween);;
	}

	void Camera::Movement::update(float dilation) {
		if (_percentBetween < 1.0f) {
			_percentBetween += dilation * _percentStep;
			if (_percentBetween > 1.0f) _percentBetween = 1.0f;
		}
	}

	bool Camera::Movement::isFinished() const {
		return _percentBetween >= 1.0f;
	}

	Camera::MovementCircular::MovementCircular(Vec3f anchor, float rotation, float magnitude, float frames) :
			Camera::Movement(calculateTilt(anchor, rotation, magnitude), frames),
			_anchor(anchor),
			_tiltMagnitude(magnitude),
			_tiltTo(rotation) {
	}

	void Camera::MovementCircular::start(Vec3f from) {
		const auto vec = (_anchor + from).normalize();
		_tiltFrom = std::atan2(-vec.y, vec.z);
	}

	Vec3f Camera::MovementCircular::getCurrent() const {
		auto tilt = linear(_tiltFrom, _tiltTo, ease(_percentBetween));
		return calculateTilt(_anchor, tilt, _tiltMagnitude);
	}
}
