#pragma once

#include <iostream>
#include "vector.h"
#include "matrix.h"

namespace math {

	/*
	* 标量与向量
	*-s* v 
	*/
	template<typename T, typename S>
	inline Vector2<T> operator * (S s, const Vector2<T>& v) {
		return v * s;
	}

	template<typename T, typename S>
	inline Vector3<T> operator * (S s, const Vector3<T>& v) {
		return v * s;
	}

	template<typename T, typename S>
	inline Vector4<T> operator * (S s, const Vector4<T>& v) {
		return v * s;
	}

	/*
	* 向量与向量
	*-v = v0 * v1
	*/
	template<typename T>
	inline Vector2<T> operator * (const Vector2<T>& v0, const Vector2<T>& v1) {
		return Vector2<T>(v0.x * v1.x , v0.y * v1.y);
	}

	template<typename T>
	inline Vector3<T> operator * (const Vector3<T>& v0, const Vector3<T>& v1) {
		return Vector3<T>(v0.x * v1.x, v0.y * v1.y, v0.z * v1.z);
	}

	template<typename T>
	inline Vector4<T> operator * (const Vector4<T>& v0, const Vector4<T>& v1) {
		return Vector4<T>(v0.x * v1.x, v0.y * v1.y, v0.z * v1.z, v0.w * v1.w);
	}
	
	/*
	* 绝对值
	*/
	template<typename T>
	inline Vector2<T> abs(const Vector2<T>& v) {
		return Vector2<T>(std::abs(v.x), std::abs(v.y));
	}

	template<typename T>
	inline Vector3<T> abs(const Vector3<T>& v) {
		return Vector3<T>(std::abs(v.x), std::abs(v.y), std::abs(v.z));
	}

	template<typename T>
	inline Vector4<T> abs(const Vector4<T>& v) {
		return Vector4<T>(std::abs(v.x), std::abs(v.y), std::abs(v.z), std::abs(v.w));
	}

	/*
	* 点乘函数
	*/
	template<typename T>
	inline T dot(const Vector2<T>& v1, const Vector2<T>& v2) {
		return v1.x * v2.x + v1.y * v2.y;
	}

	template<typename T>
	inline T dot(const Vector3<T>& v1, const Vector3<T>& v2) {
		return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
	}

	template<typename T>
	inline T dot(const Vector4<T>& v1, const Vector4<T>& v2) {
		return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z + v1.w * v2.w;
	}

	/*
	* 叉乘函数
	*/
	template<typename T>
	inline T cross(const Vector2<T>& v1, const Vector2<T>& v2) {
		return v1.x * v2.y - v1.y * v2.x;
	}

	template<typename T>
	inline Vector3<T> cross(const Vector3<T>& v1, const Vector3<T>& v2) {
		double v1x = v1.x, v1y = v1.y, v1z = v1.z;
		double v2x = v2.x, v2y = v2.y, v2z = v2.z;

		return Vector3<T>(
			v1y * v2z - v1z * v2y,
			v1z * v2x - v1x * v2z,
			v1x * v2y - v1y * v2x
		);
	}

	/*
	* 求长度平方
	*/
	template<typename T>
	inline float lengthSquared(const Vector2<T>& v) {
		return v.x * v.x + v.y * v.y;
	}

	template<typename T>
	inline float lengthSquared(const Vector3<T>& v) {
		return v.x * v.x + v.y * v.y + v.z * v.z;
	}

	template<typename T>
	inline float lengthSquared(const Vector4<T>& v) {
		return v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w;
	}

	/*
	* 求长度
	*/
	template<typename T>
	inline T length(const Vector2<T>& v) {
		return std::sqrt(lengthSquared(v));
	}

	template<typename T>
	inline T length(const Vector3<T>& v) {
		return std::sqrt(lengthSquared(v));
	}

	template<typename T>
	inline T length(const Vector4<T>& v) {
		return std::sqrt(lengthSquared(v));
	}

	/*
	* 归一化
	*/
	template<typename T>
	inline Vector2<T> normalize(const Vector2<T>& v) {
		return v / length(v);
	}

	template<typename T>
	inline Vector3<T> normalize(const Vector3<T>& v) {
		return v / length(v);
	}

	template<typename T>
	inline Vector4<T> normalize(const Vector4<T>& v) {
		return v / length(v);
	}



	//matrix
	/*
	* m0 m3 m6
	* m1 m4 m7
	* m2 m5 m8
	*/
	template<typename T>
	Matrix33<T> transpose(const Matrix33<T>& m) {
		Matrix33<T> result;
		auto dst = result.m;
		auto src = m.m;
		dst[0] = src[0]; dst[3] = src[1]; dst[6] = src[2];
		dst[1] = src[3]; dst[4] = src[4]; dst[7] = src[5];
		dst[2] = src[6]; dst[5] = src[7]; dst[8] = src[8];

		return result;
	}

	/*
	* m0 m4 m8	m12
	* m1 m5 m9	m13
	* m2 m6 m10 m14
	* m3 m7 m11	m15
	*/
	template<typename T>
	Matrix44<T> transpose(const Matrix44<T>& m) {
		Matrix44<T> result;
		auto dst = result.m;
		auto src = m.m;

		dst[0] = src[0]; dst[4] = src[1]; dst[8] = src[2]; dst[12] = src[3];
		dst[1] = src[4]; dst[5] = src[5]; dst[9] = src[6]; dst[13] = src[7];
		dst[2] = src[8]; dst[6] = src[9]; dst[10] = src[10]; dst[14] = src[11];
		dst[3] = src[12]; dst[7] = src[13]; dst[11] = src[14]; dst[15] = src[15];

		return result;
	}

	template<typename T>
	Matrix33<T> operator * (const Matrix33<T>& m1, const Matrix33<T>& m2) {
		auto m1Col0 = m1.getColum(0);
		auto m1Col1 = m1.getColum(1);
		auto m1Col2 = m1.getColum(2);

		auto m2Col0 = m2.getColum(0);
		auto m2Col1 = m2.getColum(1);
		auto m2Col2 = m2.getColum(2);

		//使用列视图进行计算
		Vector3<T> rCol0, rCol1, rCol2;
		rCol0 = m1Col0 * m2Col0[0] + m1Col1 * m2Col0[1] + m1Col2 * m2Col0[2];
		rCol1 = m1Col0 * m2Col1[0] + m1Col1 * m2Col1[1] + m1Col2 * m2Col1[2];
		rCol2 = m1Col0 * m2Col2[0] + m1Col1 * m2Col2[1] + m1Col2 * m2Col2[2];

		Matrix33<T> result;
		result.setColum(rCol0, 0);
		result.setColum(rCol1, 1);
		result.setColum(rCol2, 2);

		return result;
	}

	template<typename T>
	Matrix44<T> operator * (const Matrix44<T>& m1, const Matrix44<T>& m2) {
		auto m1Col0 = m1.getColum(0);
		auto m1Col1 = m1.getColum(1);
		auto m1Col2 = m1.getColum(2);
		auto m1Col3 = m1.getColum(3);

		auto m2Col0 = m2.getColum(0);
		auto m2Col1 = m2.getColum(1);
		auto m2Col2 = m2.getColum(2);
		auto m2Col3 = m2.getColum(3);

		//使用列视图进行计算
		Vector4<T> rCol0, rCol1, rCol2, rCol3;
		rCol0 = m1Col0 * m2Col0[0] + m1Col1 * m2Col0[1] + m1Col2 * m2Col0[2] + m1Col3 * m2Col0[3];
		rCol1 = m1Col0 * m2Col1[0] + m1Col1 * m2Col1[1] + m1Col2 * m2Col1[2] + m1Col3 * m2Col1[3];
		rCol2 = m1Col0 * m2Col2[0] + m1Col1 * m2Col2[1] + m1Col2 * m2Col2[2] + m1Col3 * m2Col2[3];
		rCol3 = m1Col0 * m2Col3[0] + m1Col1 * m2Col3[1] + m1Col2 * m2Col3[2] + m1Col3 * m2Col3[3];

		Matrix44<T> result;
		result.setColum(rCol0, 0);
		result.setColum(rCol1, 1);
		result.setColum(rCol2, 2);
		result.setColum(rCol3, 3);

		return result;
	}

	/*
	* m0 m4 m8	m12
	* m1 m5 m9	m13
	* m2 m6 m10 m14
	* m3 m7 m11	m15
	*/
	template<typename T>
	Matrix44<T> inverse(const Matrix44<T>& src) {
		Matrix44<T> result(static_cast<T>(1));

		//计算每个必须的2*2矩阵行列式,下标是左上角到右下角
		T D_22_33 = src.get(2, 2) * src.get(3, 3) - src.get(2, 3) * src.get(3, 2);

		T D_12_23 = src.get(1, 2) * src.get(2, 3) - src.get(1, 3) * src.get(2, 2);
		T D_12_33 = src.get(1, 2) * src.get(3, 3) - src.get(1, 3) * src.get(3, 2);

		T D_21_32 = src.get(2, 1) * src.get(3, 2) - src.get(2, 2) * src.get(3, 1);
		T D_21_33 = src.get(2, 1) * src.get(3, 3) - src.get(2, 3) * src.get(3, 1);

		T D_11_22 = src.get(1, 1) * src.get(2, 2) - src.get(1, 2) * src.get(2, 1);
		T D_11_23 = src.get(1, 1) * src.get(2, 3) - src.get(1, 3) * src.get(2, 1);
		T D_11_32 = src.get(1, 1) * src.get(3, 2) - src.get(1, 2) * src.get(3, 1);
		T D_11_33 = src.get(1, 1) * src.get(3, 3) - src.get(1, 3) * src.get(3, 1);

		T D_02_13 = src.get(0, 2) * src.get(1, 3) - src.get(0, 3) * src.get(1, 2);
		T D_02_23 = src.get(0, 2) * src.get(2, 3) - src.get(0, 3) * src.get(2, 2);
		T D_02_33 = src.get(0, 2) * src.get(3, 3) - src.get(0, 3) * src.get(3, 2);

		T D_01_12 = src.get(0, 1) * src.get(1, 2) - src.get(0, 2) * src.get(1, 1);
		T D_01_13 = src.get(0, 1) * src.get(1, 3) - src.get(0, 3) * src.get(1, 1);
		T D_01_22 = src.get(0, 1) * src.get(2, 2) - src.get(0, 2) * src.get(2, 1);
		T D_01_23 = src.get(0, 1) * src.get(2, 3) - src.get(0, 3) * src.get(2, 1);
		T D_01_32 = src.get(0, 1) * src.get(3, 2) - src.get(0, 2) * src.get(3, 1);
		T D_01_33 = src.get(0, 1) * src.get(3, 3) - src.get(0, 3) * src.get(3, 1);

		//计算伴随阵的每列数据
		Vector4<T> col0, col1, col2, col3;

		/*
		* 
		* m5 m9	 m13
		* m6 m10 m14
		* m7 m11 m15
		*/
		col0.x = src.get(1, 1) * D_22_33 - src.get(2, 1) * D_12_33 + src.get(3, 1) * D_12_23;
		col0.y = -(src.get(1, 0) * D_22_33 - src.get(2, 0) * D_12_33 + src.get(3, 0) * D_12_23);
		col0.z = src.get(1, 0) * D_21_33 - src.get(2, 0) * D_11_33 + src.get(3, 0) * D_11_23;
		col0.w = -(src.get(1, 0) * D_21_32 - src.get(2, 0) * D_11_32 + src.get(3, 0) * D_11_22);

		col1.x = -(src.get(0, 1) * D_22_33 - src.get(2, 1) * D_02_33 + src.get(3, 1) * D_02_23);
		col1.y = src.get(0, 0) * D_22_33 - src.get(2, 0) * D_02_33 + src.get(3, 0) * D_02_23;
		col1.z = -(src.get(0, 0) * D_21_33 - src.get(2, 0) * D_01_33 + src.get(3, 0) * D_01_23);
		col1.w = src.get(0, 0) * D_21_32 - src.get(2, 0) * D_01_32 + src.get(3, 0) * D_01_22;

		col2.x = src.get(0, 1) * D_12_33 - src.get(1, 1) * D_02_33 + src.get(3, 1) * D_02_13;
		col2.y = -(src.get(0, 0) * D_12_33 - src.get(1, 0) * D_02_33 + src.get(3, 0) * D_02_13);
		col2.z = src.get(0, 0) * D_11_33 - src.get(1, 0) * D_01_33 + src.get(3, 0) * D_01_13;
		col2.w = -(src.get(0, 0) * D_11_32 - src.get(1, 0) * D_01_32 + src.get(3, 0) * D_01_12);

		col3.x = -(src.get(0, 1) * D_12_23 - src.get(1, 1) * D_02_23 + src.get(2, 1) * D_02_13);
		col3.y = src.get(0, 0) * D_12_23 - src.get(1, 0) * D_02_23 + src.get(2, 0) * D_02_13;
		col3.z = -(src.get(0, 0) * D_11_23 - src.get(1, 0) * D_01_23 + src.get(2, 0) * D_01_13);
		col3.w = src.get(0, 0) * D_11_22 - src.get(1, 0) * D_01_22 + src.get(2, 0) * D_01_12;

		result.setColum(col0, 0);
		result.setColum(col1, 1);
		result.setColum(col2, 2);
		result.setColum(col3, 3);

		//计算行列式
		Vector4<T> row0(result.get(0, 0), result.get(0, 1), result.get(0, 2), result.get(0, 3));
		Vector4<T> colum0 = src.getColum(0);
		T determinant = dot(row0, colum0);

		assert(determinant != 0);

		T oneOverDeterminant = static_cast<T>(1) / determinant;

		return result * oneOverDeterminant;
	}

}