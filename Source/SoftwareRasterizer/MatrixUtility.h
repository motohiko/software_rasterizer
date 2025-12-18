#pragma once

// ベクトル表現の規約（変換行列の記法）
// 
//  列ベクトル形式
// 
//  ベクトルの形：列（nx1行列）
//  行列の掛け方：左から掛ける（M・v）
//  平行移動ベクトルの位置：最後の列
// 

// 座標系
// 
//  ワールド空間　：右手
//  ビュー空間　　：右手（+y:up, +x:right）
//  クリップ空間　：左手（+y:up, +x:right）
//  ウィンドウ空間：左手（+y:up, +x:right）※ビューポート
//

// note.
// 
// 右手系
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

#include "Core\Types.h"

namespace SoftwareRasterizer
{
	class TransformMatrix
	{

	public:

		static Matrix4x4 CreateBasis(const Vector3& xAxis, const Vector3& yAxis, const Vector3& zAxis);
		static Matrix4x4 CreateBasisAndOrigin(const Vector3& xAxis, const Vector3& yAxis, const Vector3& zAxis, const Vector3& origin);

		static Matrix4x4 CreateTranslate(float x, float y, float z);//glTranslate

		static Matrix4x4 CreateRotationX(float rad);
		static Matrix4x4 CreateRotationY(float rad);
		static Matrix4x4 CreateRotationZ(float rad);

		static Matrix4x4 CreateScale(float x, float y, float z);//glScale

		static Matrix4x4 CreateShear(float xy, float xz, float yx, float yz, float zx, float zy);

	};

	class ViewMatrix
	{

	public:

		static Matrix4x4 CreateLookAt(const Vector3& eye, const Vector3& center, const Vector3& up);//gluLookAt

	};

	class ProjectionMatrix
	{

	public:

		static Matrix4x4 CreateFrustum(float left, float right, float bottom, float top, float nearVal, float farVal);//glFrustum

		static Matrix4x4 CreatePerspective(float fovy, float aspect, float zNear, float zFar);//gluPerspective

	};

}
