#ifndef SUPER_HAXAGON_SURFACE_CAMERA_HPP
#define SUPER_HAXAGON_SURFACE_CAMERA_HPP

#include "Core/Structs.hpp"
#include "Core/Matrix.hpp"
#include "Core/Twist.hpp"

#include <deque>
#include <vector>

namespace SuperHaxagon {
	class Camera {
	public:
		explicit Camera(Twist& twist) : _rand(twist) {};
		~Camera() = default;

		void update(float dilation);
		bool isMoving() const;
		void set(Vec3f pos, Vec3f at);
		void setNext(Vec3f pos, Vec3f at, float frames);
		void reset(float frames);
		void tilt(float frames);
		Vec3f currentPos() {return _currentPos;}
		Vec3f currentLookAt() {return _currentLookAt;}

	private:
		Twist& _rand;

		float _percentBetween = 0.0f;
		float _framesToCompletion = 0.0f;
		float _tiltDir = 0.0f;
		bool _locked = false;

		Vec3f _currentPos;
		Vec3f _currentLookAt;

		Vec3f _fromPos;
		Vec3f _fromLookAt;

		Vec3f _toPos;
		Vec3f _toLookAt;
	};
}

#endif //SUPER_HAXAGON_SURFACE_CAMERA_HPP
