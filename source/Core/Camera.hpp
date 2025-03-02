#ifndef SUPER_HAXAGON_SURFACE_CAMERA_HPP
#define SUPER_HAXAGON_SURFACE_CAMERA_HPP

#include "Core/Structs.hpp"
#include "Core/Matrix.hpp"
#include "Core/Twist.hpp"

#include <deque>
#include <vector>
#include <map>

namespace SuperHaxagon {
	inline Vec3f calculateTilt(Vec3f anchor, float rotation, float magnitude) {
		return {
				anchor.x,
				anchor.y - std::sin(rotation) * magnitude,
				anchor.z + std::cos(rotation) * magnitude
		};
	}

	enum class CameraLayer {
		LOOK_AT,
		MAIN,
		SCALE,
		TILTS,
		ZOOMS
	};

	class Camera {
	public:
		Camera();
		~Camera() = default;

		void update(float dilation);

		bool isMoving(CameraLayer layer);
		Vec3f currentPos();
		Vec3f currentPos(CameraLayer layer);
		Vec3f currentLookAt();

		// Clears all effect queues and movements, and returns them to 0.
		void stopAllEffects();

		// Immediately resets all movements.
		void reset();

		// Immediately moves the camera to a position and clears any movements for that layer.
		void setPosition(CameraLayer layer, Vec3f pos);

		// Interrupts any movements and replaces it with the new one. Clears queue.
		void setMovement(CameraLayer layer, Vec3f to, float frames);

		// Queues up a movement to occur immediately after the currently playing movement is done.
		void queueMovement(CameraLayer layer, Vec3f to, float frames);

		// Like setMovement, but the motion occurs on an arc around the X axis (tilt)
		void setMovementRotation(CameraLayer layer, Vec3f anchor, float rotation, float magnitude, float frames);

	private:
		class Movement {
		public:
			Movement(Vec3f to, float frames);

			virtual ~Movement() = default;

			virtual void start(Vec3f from);
			virtual Vec3f getCurrent() const;

			void update(float dilation);
			bool isFinished() const;

		protected:
			float _percentBetween = 0.0f;
			float _percentStep;
			Vec3f _from{};
			Vec3f _to;
		};

		class MovementCircular : public Movement {
		public:
			MovementCircular(Vec3f anchor, float rotation, float magnitude, float frames);

			void start(Vec3f from) override;
			Vec3f getCurrent() const override;

		private:
			Vec3f _anchor;
			float _tiltMagnitude;
			float _tiltFrom = 0.0f;
			float _tiltTo = 0.0f;
		};

		std::map<CameraLayer, std::deque<std::unique_ptr<Camera::Movement>>> _queue{};
		std::map<CameraLayer, std::unique_ptr<Camera::Movement>> _movements{};
		std::map<CameraLayer, Vec3f> _position{};

		Vec3f _currentPos;
		Vec3f _currentLookAt;
	};
}

#endif //SUPER_HAXAGON_SURFACE_CAMERA_HPP
