#pragma once

#include "Matrix.h"
#include "Vector.h"

namespace SoftwareRasterizer
{
	class Math3d
	{

	public:

		static Matrix4x4 lockAt(const Vector3& eye, const Vector3& center, const Vector3& up);
		static Matrix4x4 createFrustum(float left, float right, float bottom, float top, float nearVal, float farVal);
		static Matrix4x4 createProjection(float fovy, float aspect, float zNear, float zFar);

	};
}
