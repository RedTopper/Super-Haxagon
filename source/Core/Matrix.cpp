#include "Core/Matrix.hpp"

#include <cmath>

namespace SuperHaxagon {
	template <typename T>
	Matrix4x4<T>::Matrix4x4(T a, T b, T c, T d, T e, T f, T g, T h, T i, T j, T k, T l, T m, T n, T o, T p) {
		x[0][0] = a;
		x[0][1] = b;
		x[0][2] = c;
		x[0][3] = d;
		x[1][0] = e;
		x[1][1] = f;
		x[1][2] = g;
		x[1][3] = h;
		x[2][0] = i;
		x[2][1] = j;
		x[2][2] = k;
		x[2][3] = l;
		x[3][0] = m;
		x[3][1] = n;
		x[3][2] = o;
		x[3][3] = p;
	}

	template <typename T>
	Matrix4x4<T> Matrix4x4<T>::operator*(const Matrix4x4& v) const {
		Matrix4x4 tmp;
		multiply(*this, v, tmp);
		return tmp;
	}

	template <typename T>
	Vec3<T> Matrix4x4<T>::operator*(const Vec3<T>& v) const {
		Vec3<T> out{};
		out.x = v.x * x[0][0] + v.y * x[1][0] + v.z * x[2][0] + /* v.w = 1 */ x[3][0];
		out.y = v.x * x[0][1] + v.y * x[1][1] + v.z * x[2][1] + /* v.w = 1 */ x[3][1];
		out.z = v.x * x[0][2] + v.y * x[1][2] + v.z * x[2][2] + /* v.w = 1 */ x[3][2];
		T w   = v.x * x[0][3] + v.y * x[1][3] + v.z * x[2][3] + /* v.w = 1 */ x[3][3];
		if (w != 1) {
			out.x /= w;
			out.y /= w;
			out.z /= w;
		}

		return out;
	}

	template <typename T>
	void Matrix4x4<T>::multiply(const Matrix4x4<T>& a, const Matrix4x4& b, Matrix4x4& c) {
		const T* __restrict ap = &a.x[0][0];
		const T* __restrict bp = &b.x[0][0];
		T* __restrict cp = &c.x[0][0];

		T a0, a1, a2, a3;

		a0 = ap[0];
		a1 = ap[1];
		a2 = ap[2];
		a3 = ap[3];

		cp[0] = a0 * bp[0] + a1 * bp[4] + a2 * bp[8]  + a3 * bp[12];
		cp[1] = a0 * bp[1] + a1 * bp[5] + a2 * bp[9]  + a3 * bp[13];
		cp[2] = a0 * bp[2] + a1 * bp[6] + a2 * bp[10] + a3 * bp[14];
		cp[3] = a0 * bp[3] + a1 * bp[7] + a2 * bp[11] + a3 * bp[15];

		a0 = ap[4];
		a1 = ap[5];
		a2 = ap[6];
		a3 = ap[7];

		cp[4] = a0 * bp[0] + a1 * bp[4] + a2 * bp[8]  + a3 * bp[12];
		cp[5] = a0 * bp[1] + a1 * bp[5] + a2 * bp[9]  + a3 * bp[13];
		cp[6] = a0 * bp[2] + a1 * bp[6] + a2 * bp[10] + a3 * bp[14];
		cp[7] = a0 * bp[3] + a1 * bp[7] + a2 * bp[11] + a3 * bp[15];

		a0 = ap[8];
		a1 = ap[9];
		a2 = ap[10];
		a3 = ap[11];

		cp[8]  = a0 * bp[0] + a1 * bp[4] + a2 * bp[8]  + a3 * bp[12];
		cp[9]  = a0 * bp[1] + a1 * bp[5] + a2 * bp[9]  + a3 * bp[13];
		cp[10] = a0 * bp[2] + a1 * bp[6] + a2 * bp[10] + a3 * bp[14];
		cp[11] = a0 * bp[3] + a1 * bp[7] + a2 * bp[11] + a3 * bp[15];

		a0 = ap[12];
		a1 = ap[13];
		a2 = ap[14];
		a3 = ap[15];

		cp[12] = a0 * bp[0] + a1 * bp[4] + a2 * bp[8]  + a3 * bp[12];
		cp[13] = a0 * bp[1] + a1 * bp[5] + a2 * bp[9]  + a3 * bp[13];
		cp[14] = a0 * bp[2] + a1 * bp[6] + a2 * bp[10] + a3 * bp[14];
		cp[15] = a0 * bp[3] + a1 * bp[7] + a2 * bp[11] + a3 * bp[15];
	}

	template <typename T>
	Matrix4x4<T> Matrix4x4<T>::transposed() const {
		return Matrix4x4(
			x[0][0],
			x[1][0],
			x[2][0],
			x[3][0],
			x[0][1],
			x[1][1],
			x[2][1],
			x[3][1],
			x[0][2],
			x[1][2],
			x[2][2],
			x[3][2],
			x[0][3],
			x[1][3],
			x[2][3],
			x[3][3]);
	}

	template <typename T>
	Matrix4x4<T>& Matrix4x4<T>::transpose() {
		Matrix4x4 tmp(
			x[0][0],
			x[1][0],
			x[2][0],
			x[3][0],
			x[0][1],
			x[1][1],
			x[2][1],
			x[3][1],
			x[0][2],
			x[1][2],
			x[2][2],
			x[3][2],
			x[0][3],
			x[1][3],
			x[2][3],
			x[3][3]);
		*this = tmp;
		return *this;
	}

	template <typename T>
	Matrix4x4<T> Matrix4x4<T>::generateProjection(const T fov, const T aspect, const T near, const T far) {
		T const tanHalfFovY = std::tan(fov / static_cast<T>(2));
		Matrix4x4<T> matrix;
		matrix[0][0] = static_cast<T>(1) / (aspect * tanHalfFovY);
		matrix[1][1] = static_cast<T>(1) / (tanHalfFovY);
		matrix[2][2] = - (far + near) / (far - near);
		matrix[2][3] = - static_cast<T>(1);
		matrix[3][2] = - (static_cast<T>(2) * far * near) / (far - near);
		matrix[3][3] = 0;
		return matrix;
	}

	template <typename T>
	Matrix4x4<T> Matrix4x4<T>::lookAt(const Vec3<T>& pos, const Vec3<T>& at, const Vec3<T>& up) {
		auto f = (at - pos).normalize();
		auto s = (f.cross(up)).normalize();
		auto u = s.cross(f);

		Matrix4x4<T> matrix;
		matrix[0][0] = s.x; matrix[0][1] = u.x; matrix[0][2] = -f.x;
		matrix[1][0] = s.y; matrix[1][1] = u.y; matrix[1][2] = -f.y;
		matrix[2][0] = s.z; matrix[2][1] = u.z; matrix[2][2] = -f.z;
		matrix[3][0] = -s.dot(pos);
		matrix[3][1] = -u.dot(pos);
		matrix[3][2] = f.dot(pos);
		matrix[3][3] = static_cast<T>(1);
		return matrix;
	}

	template class Matrix4x4<float>;
}
