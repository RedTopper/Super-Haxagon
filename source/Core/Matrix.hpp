#ifndef SUPER_HAXAGON_MATRIX_HPP
#define SUPER_HAXAGON_MATRIX_HPP

#include "Core/Vector.hpp"

#include <cstdint>

namespace SuperHaxagon {
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
		static Matrix4x4 generateProjection(T fov, T aspect, T near, T far);
		static Matrix4x4 lookAt(const Vec3<T>& pos, const Vec3<T>& at, const Vec3<T>& up);
	};

	typedef Matrix4x4<float> Matrix4x4f;
}

#endif //SUPER_HAXAGON_MATRIX_HPP