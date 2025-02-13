#ifndef SUPER_HAXAGON_MATRIX_HPP
#define SUPER_HAXAGON_MATRIX_HPP

#include <cstdint>
#include <cmath>

namespace SuperHaxagon {

	static constexpr float PI = 3.14159265358979f;
	static constexpr float TAU = PI * 2.0f;

	template<typename T>
	struct Vec2 {
		T x;
		T y;

		Vec2() : x(0), y(0) {}
		Vec2(const T xx) : x(xx), y(xx) {}
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
		Vec3(Vec2<T> v) : x(v.x), y(v.y), z(0) {}
		Vec3(T xx) : x(xx), y(xx), z(xx) {}
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

		Vec3& normalize() {
			if (T n = norm(); n > 0) {
				T factor = 1 / sqrt(n);
				x *= factor, y *= factor, z *= factor;
			}

			return *this;
		}

		T norm() const {
			return x * x + y * y + z * z;
		}

		T length() const {
			return sqrt(norm());
		}
	};

	template<typename T>
	class Matrix4x4 {
	public:
		T x[4][4] = { {1,0,0,0},{0,1,0,0},{0,0,1,0},{0,0,0,1} };

		Matrix4x4() = default;
		Matrix4x4(T a, T b, T c, T d, T e, T f, T g, T h, T i, T j, T k, T l, T m, T n, T o, T p);

		const T* operator[](uint8_t i) const { return x[i]; }
		T* operator[](uint8_t i) { return x[i]; }

		Matrix4x4 operator*(const Matrix4x4& v) const;
		Vec3<T> operator*(const Vec3<T>& v) const;
	
		Matrix4x4 transposed() const;
		Matrix4x4& transpose();

		static void multiply(const Matrix4x4<T>& a, const Matrix4x4& b, Matrix4x4& c);
		static Matrix4x4 generateProjection(float angleOfView, float near, float far);
	};

	typedef Vec2<float> Vec2f;
	typedef Vec3<float> Vec3f;
	typedef Matrix4x4<float> Matrix4x4f;
}

#endif //SUPER_HAXAGON_MATRIX_HPP