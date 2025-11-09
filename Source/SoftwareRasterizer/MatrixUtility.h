#pragma once

#include "Types.h"
#include "..\Lib\Matrix.h"
#include "..\Lib\Vector.h"

namespace SoftwareRasterizer
{
	//
	// ベクトル（行列）は列優先
	//
	//     | x |        | xAxis.x yAxis.x zAxis.x origon.x |
	// v = | y |    m = | xAxis.y yAxis.y zAxis.y origon.y |
	//     | z |        | xAxis.z yAxis.z zAxis.z origon.z |
	//     | w |        | xAxis.w yAxis.w zAxis.w origon.w |
	//
	//
	// 座標系は右手系
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
	//
	// note.
	// 
	// 座標軸は xyz = rgb で描画される
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
