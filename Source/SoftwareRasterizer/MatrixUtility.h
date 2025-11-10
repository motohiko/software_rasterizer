#pragma once

#include "Types.h"
#include "..\Lib\Matrix.h"
#include "..\Lib\Vector.h"

namespace SoftwareRasterizer
{
	//
	// 右手座標系
	// 
	//      人差し指
	//      +y 
	//        |
	//        |
	//        +---- +x 親指
	//       /
	//      /
	//   +z
	//   中指
	//
	// note.
	// 
	// 座標軸は xyz = rgb で描画される
	// 

	//
	// 列優先行列
	//
	// 列ベクトル
	//
	//     | x |
	// v = | y |
	//     | z |
	//     | w |
	//

	class MatrixUtility
	{

	public:

		static Matrix4x4 CreateBasis(const Vector3& xAxis, const Vector3& yAxis, const Vector3& zAxis, const Vector3& origon);

		// Transform Matrix
		static Matrix4x4 CreateRotationX(float angle);
		static Matrix4x4 CreateRotationY(float angle);
		static Matrix4x4 CreateRotationZ(float angle);
		static Matrix4x4 CreateScale(float x, float y, float z);
		static Matrix4x4 CreateShear(float xy, float xz, float yx, float yz, float zx, float zy);
		static Matrix4x4 CreateTranslate(float x, float y, float z);

		// View Matrix
		static Matrix4x4 CreateLookAt(const Vector3& eye, const Vector3& center, const Vector3& up);
		
		// Projection Matrix
		static Matrix4x4 CreateFrustum(float left, float right, float bottom, float top, float nearVal, float farVal);
		static Matrix4x4 CreatePerspective(float fovy, float aspect, float zNear, float zFar);

	};
}
