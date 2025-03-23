#ifndef DRIVER_VECTOR_HPP
#define DRIVER_VECTOR_HPP

// SPDX-FileCopyrightText: 2021 AJ Walter
// SPDX-License-Identifier: GPL-3.0-or-later OR MIT

#include <algorithm>
#include <cstdint>
#include <cmath>

namespace SuperHaxagon {

	static constexpr float PI = 3.14159265358979f;
	static constexpr float TAU = PI * 2.0f;

	// Converts a linear input to an eased input
	template<typename T>
	inline T ease(T t) {
		t = std::clamp(t, static_cast<T>(0), static_cast<T>(1));
		T f = 0;
		if (t < static_cast<T>(0.5)) {
			f = static_cast<T>(2) * t * t;
		} else {
			t -= static_cast<T>(0.5);
			f = static_cast<T>(2) * t * (static_cast<T>(1) - t) + static_cast<T>(0.5);
		}

		return f;
	}

	template<typename T>
	inline T interpolate(T start, T end, T t) {
		return (end - start) * t + start;
	}

	template<typename T>
	struct Vec2 {
		T x;
		T y;

		Vec2() : x(0), y(0) {}
		explicit Vec2(const T xx) : x(xx), y(xx) {}
		Vec2(const T xx, const T yy) : x(xx), y(yy) {}
		
		Vec2 operator+(const Vec2& v) const {
			return Vec2(x + v.x, y + v.y);
		}
		
		Vec2 operator/(const T& r) const {
			return Vec2(x / r, y / r);
		}
		
		Vec2 operator*(const T& r) const {
			return Vec2(x * r, y * r);
		}
		
		Vec2& operator/=(const T& r) {
			x /= r, y /= r; return *this;
		}
		
		Vec2& operator*=(const T& r) {
			x *= r, y *= r; return *this;
		}
	};

	template<typename T>
	struct Vec3 {
		T x;
		T y;
		T z;

		Vec3() : x(0), y(0), z(0) {}
		explicit Vec3(Vec2<T> v) : x(v.x), y(v.y), z(0) {}
		explicit Vec3(T xx) : x(xx), y(xx), z(xx) {}
		Vec3(T xx, T yy, T zz) : x(xx), y(yy), z(zz) {}
		
		Vec3 operator+(const Vec3& v) const {
			return Vec3(x + v.x, y + v.y, z + v.z);
		}
		
		Vec3 operator-(const Vec3& v) const {
			return Vec3(x - v.x, y - v.y, z - v.z);
		}
		
		Vec3 operator-() const {
			return Vec3(-x, -y, -z);
		}
		
		Vec3 operator*(const T& r) const {
			return Vec3(x * r, y * r, z * r);
		}
		
		Vec3 operator*(const Vec3& v) const {
			return Vec3(x * v.x, y * v.y, z * v.z);
		}

		Vec3& operator*=(const T& r) {
			x *= r, y *= r, z *= r; return *this;
		}

		Vec3& operator/=(const T& r) {
			x /= r, y /= r, z /= r; return *this;
		}

		Vec3 cross(const Vec3& r) const {
			return Vec3 {
				y*r.z - z*r.y,
				z*r.x - x*r.z,
				x*r.y - y*r.x
			};
		}

		Vec3& normalize() {
			if (T n = norm(); n > 0) {
				T factor = 1 / sqrt(n);
				x *= factor, y *= factor, z *= factor;
			}

			return *this;
		}

		T dot(const Vec3& r) const {
			auto mult = *this * r;
			return mult.x + mult.y + mult.z;
		}

		T norm() const {
			return x * x + y * y + z * z;
		}

		T length() const {
			return sqrt(norm());
		}

		Vec3 interpolate(Vec3 r, T t) const {
			return {
				SuperHaxagon::interpolate<T>(x, r.x, t),
				SuperHaxagon::interpolate<T>(y, r.y, t),
				SuperHaxagon::interpolate<T>(z, r.z, t),
			};
		}

		Vec3 ease(Vec3 r, T t) const {
			return interpolate(r, SuperHaxagon::ease<T>(t));
		}
	};

	typedef Vec2<float> Vec2f;
	typedef Vec3<float> Vec3f;
}

#endif //DRIVER_VECTOR_HPP
