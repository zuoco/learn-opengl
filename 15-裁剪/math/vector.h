#pragma once
#include <iostream>
#include <assert.h>

namespace math {
	template<typename T>
	class Vector3;

	template<typename T>
	class Vector4;

	template<typename T>
	class Vector2 {
	public:
		Vector2() { x = y = 0; }
		Vector2(T x, T y) :x(x), y(y) {}
		Vector2(const Vector2<T>& v) :x(v.x), y(v.y) {}
		Vector2(const Vector3<T>& v) :x(v.x), y(v.y) {}
		Vector2(const Vector4<T>& v) :x(v.x), y(v.y) {}

		/*
		* ȡ������ĳ��Ԫ��
		* int a = v[1]
		*/
		T operator[](int i) const {
			assert(i >= 0 && i < 2);

			if (i == 0) return x;

			return y;
		}

		/*
		* ������ĳ��Ԫ�ظ�ֵ
		* v[1]=6
		*/
		T& operator[](int i) {
			assert(i >= 0 && i < 2);

			if (i == 0) return x;

			return y;
		}

		//�Ⱥ����������
		Vector2<T> operator=(const Vector3<T>& v) {
			x = v.x; y = v.y;
			return *this;
		}

		Vector2<T> operator=(const Vector4<T>& v) {
			x = v.x; y = v.y;
			return *this;
		}

		/*
		* �ӷ�
		* v = v1+v2
		*/
		Vector2<T> operator+(const Vector2<T>& v) const {
			return Vector2(x + v.x, y + v.y);
		}

		/*
		* �ӷ�����ֵ
		* v += v2
		*/
		Vector2<T> operator+=(const Vector2<T>& v) {
			x += v.x; y += v.y;
			return *this;
		}

		/*
		* �˷�
		* v = v1 * s
		*/
		Vector2<T> operator*(T s) const {
			return Vector2(x * s, y * s);
		}

		/*
		* �˷�����ֵ
		* v *= s
		*/
		Vector2<T> operator*=(T s) {
			x *= s; y *= s;
			return *this;
		}

		/*
		* ����
		* v = v1 / f
		*/
		Vector2<T> operator/(T f) const {
			assert(f != 0);
			float inv = static_cast<T>(1) / f;
			return Vector2(x * inv, y * inv);
		}

		/*
		* ��������ֵ
		* v /= f
		*/
		Vector2<T> operator/=(T f) {
			assert(f != 0);
			float inv = static_cast<T>(1) / f;
			x *= inv; y *= inv;
			return *this;
		}

		/*
		* ���Ų���
		* v = -v1
		*/
		Vector2<T> operator-() {
			return Vector2(-x, -y);
		}

		void print() {
			std::cout << "Vector2 is:" << std::endl;
			std::cout << "x = " << x << ", y = " << y << std::endl;
			std::cout << std::endl;
		}

	public:
		T x, y;
	};


	template<typename T>
	class Vector3 {
	public:
		Vector3() { x = y = z = 0; }
		Vector3(T x, T y, T z) :x(x), y(y), z(z) {}
		Vector3(const Vector3<T>&v):x(v.x), y(v.y), z(v.z) {}
		Vector3(const Vector4<T>&v):x(v.x), y(v.y), z(v.z) {}

		T operator[](int i) const {
			assert(i >= 0 && i <= 2);

			if (i == 0) return x;
			if (i == 1) return y;

			return z;
		}

		T& operator[](int i) {
			assert(i >= 0 && i <= 2);

			if (i == 0) return x;
			if (i == 1) return y;

			return z;
		}

		Vector3<T> operator=(const Vector2<T>& v) {
			x = v.x; y = v.y;
			return *this;
		}

		Vector3<T> operator=(const Vector4<T>& v) {
			x = v.x; y = v.y; z = v.z;
			return *this;
		}

		Vector3<T> operator+(const Vector3<T>& v) const {
			return Vector3<T>(x + v.x, y + v.y, z + v.z);
		}

		Vector3<T> operator+=(const Vector3<T>& v) {
			x += v.x; y += v.y; z += v.z;
			return *this;
		}

		Vector3<T> operator-(const Vector3<T>& v) const {
			return Vector3(x - v.x, y - v.y, z - v.z);
		}

		Vector3<T> operator-=(const Vector3<T>& v) {
			x -= v.x; y -= v.y; z -= v.z;
			return *this;
		}

		Vector3<T> operator*(T s) const {
			return Vector3(x * s, y * s, z * s);
		}

		Vector3<T> operator*=(T s) {
			x *= s; y *= s; z *= s;
			return *this;
		}

		Vector3<T> operator/(T f) const {
			assert(f != 0);
			float inv = 1.0 / f;
			return Vector3(x * inv, y * inv, z * inv);
		}

		Vector3<T> operator/=(T f) {
			assert(f != 0);
			float inv = 1.0 / f;
			x *= inv; y *= inv; z *= inv;
			return *this;
		}

		Vector3<T>  operator-() const {
			return Vector3<T>(-x, -y, -z);
		}

		void print() {
			std::cout << "Vector3 is:" << std::endl;
			std::cout << "x = " << x << ", y = " << y << ", z = " << z << std::endl;
			std::cout << std::endl;
		}

	public:
		T x, y, z;
	};

	template<typename T>
	class Vector4 {
	public:
		Vector4() { x = y = z = w = 0; }
		Vector4(T x, T y, T z, T w) :x(x), y(y), z(z), w(w) {}
		Vector4(const Vector4<T>&v):x(v.x), y(v.y), z(v.z), w(v.w) {}

		T operator[](int i) const {
			assert(i >= 0 && i <= 3);

			if (i == 0) return x;
			if (i == 1) return y;
			if (i == 2) return z;

			return w;
		}

		T& operator[](int i) {
			assert(i >= 0 && i <= 3);

			if (i == 0) return x;
			if (i == 1) return y;
			if (i == 2) return z;

			return w;
		}

		Vector4<T> operator=(const Vector2<T>& v) {
			x = v.x; y = v.y;
			return *this;
		}

		Vector4<T> operator=(const Vector3<T>& v) {
			x = v.x; y = v.y; z = v.z;
			return *this;
		}

		Vector4<T> operator+(const Vector4<T>& v) const {
			return Vector4(x + v.x, y + v.y, z + v.z, w + v.w);
		}

		Vector4<T> operator+=(const Vector4<T>& v) {
			x += v.x; y += v.y; z += v.z; w += v.w;
			return *this;
		}

		Vector4<T> operator-(const Vector4<T>& v) const {
			return Vector4<T>(x - v.x, y - v.y, z - v.z, w - v.w);
		}

		Vector4<T> operator-=(const Vector4<T>& v) {
			x -= v.x; y -= v.y; z -= v.z; w -= v.w;
			return *this;
		}

		Vector4<T> operator*(T s) const {
			return Vector4(x * s, y * s, z * s, w * s);
		}

		Vector4<T> operator*=(T s) {
			x *= s; y *= s; z *= s; w *= s;
			return *this;
		}

		Vector4<T> operator*=(const Vector3<T>& v) {
			x *= v.x; y *= v.y; z *= v.z;
			return *this;
		}

		Vector4<T> operator/(T f) const {
			assert(f != 0);
			float inv = 1.0 / f;
			return Vector4(x * inv, y * inv, z * inv, w * inv);
		}

		Vector4<T> operator/=(T f) {
			assert(f != 0);
			float inv = 1.0 / f;
			x *= inv; y *= inv; z *= inv; w *= inv;
			return *this;
		}

		Vector4<T> operator-() const {
			return Vector4<T>(-x, -y, -z, -w);
		}

		void print() {
			std::cout << "Vector4 is:" << std::endl;
			std::cout << "x = " << x << ", y = " << y << ", z = " << z << ", w = " << w << std::endl;
			std::cout << std::endl;
		}

	public:
		T x, y, z, w;
	};

	using vec2f = Vector2<float>;
	using vec2i = Vector2<int>;
	using vec3f = Vector3<float>;
	using vec3i = Vector3<int>;
	using vec4f = Vector4<float>;
	using vec4i = Vector4<int>;
}
