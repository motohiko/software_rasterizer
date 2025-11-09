#pragma once

#include "Types.h"
#include "..\Lib\Matrix.h"
#include "..\Lib\Vector.h"

namespace SoftwareRasterizer
{
	//
	// ベクトル・行列は列優先
	//
	//     | x |        | xx yy zx wx |
	// v = | y |    m = | xy yy zy wy |
	//     | z |        | xz yz zz wz |
	//     | w |        | xw yw zw ww |
	//

	//
	// 座標系は右手系
	//

	class MatrixUtility
	{

	public:

		static Matrix4x4 createBasis(const Vector3& xAxis, const Vector3& yAxis, const Vector3& zAxis, const Vector3& origon);
		static Matrix4x4 createRotationX(float angle);
		static Matrix4x4 createRotationY(float angle);
		static Matrix4x4 createScale(float x, float y, float z, float w);
		static Matrix4x4 createShear(float xy, float xz, float yx, float yz, float zx, float zy);

		static Matrix4x4 lookAtRH(const Vector3& eye, const Vector3& center, const Vector3& up);
		static Matrix4x4 createFrustum(float left, float right, float bottom, float top, float nearVal, float farVal);
		static Matrix4x4 createProjection(float fovy, float aspect, float zNear, float zFar);

	};
}
