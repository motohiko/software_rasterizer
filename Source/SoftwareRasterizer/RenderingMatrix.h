#pragma once

#include "..\Lib\Matrix.h"
#include "..\Lib\Vector.h"

namespace SoftwareRasterizer
{
	class RenderingMatrix
	{

	public:

		static Matrix4x4 lookAtRH(const Vector3& eye, const Vector3& center, const Vector3& up);
		static Matrix4x4 createFrustum(float left, float right, float bottom, float top, float nearVal, float farVal);
		static Matrix4x4 createProjection(float fovy, float aspect, float zNear, float zFar);

	};
}
