#include "MatrixUtility.h"
#include <cmath>

namespace SoftwareRasterizer
{
    Matrix4x4 TransformMatrix::CreateBasis(const Vector3& xAxis, const Vector3& yAxis, const Vector3& zAxis)
    {
        return Matrix4x4(
            xAxis.x, yAxis.x, zAxis.x, 0.0f,
            xAxis.y, yAxis.y, zAxis.y, 0.0f,
            xAxis.z, yAxis.z, zAxis.z, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
        );
    }

    Matrix4x4 TransformMatrix::CreateBasisAndOrigin(const Vector3& xAxis, const Vector3& yAxis, const Vector3& zAxis, const Vector3& origin)
    {
        return Matrix4x4(
            xAxis.x, yAxis.x, zAxis.x, origin.x,
            xAxis.y, yAxis.y, zAxis.y, origin.y,
            xAxis.z, yAxis.z, zAxis.z, origin.z,
            0.0f, 0.0f, 0.0f, 1.0f
        );
    }

    Matrix4x4 TransformMatrix::CreateTranslate(float x, float y, float z)
    {
        return Matrix4x4(
            1.0f, 0.0f, 0.0f, x,
            0.0f, 1.0f, 0.0f, y,
            0.0f, 0.0f, 1.0f, z,
            0.0f, 0.0f, 0.0f, 1.0f
        );
    }

    Matrix4x4 TransformMatrix::CreateRotationX(float rad)
    {
        float c = std::cos(rad);
        float s = std::sin(rad);

        return Matrix4x4(
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, c,    s,    0.0f,
            0.0f, -s,   c,    0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
        );
    }

    Matrix4x4 TransformMatrix::CreateRotationY(float rad)
    {
        float c = std::cos(rad);
        float s = std::sin(rad);

        return Matrix4x4(
            c,    0.0f, -s,   0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            s,    0.0f, c,    0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
        );
    }

    Matrix4x4 TransformMatrix::CreateRotationZ(float rad)
    {
        float c = std::cos(rad);
        float s = std::sin(rad);

        return Matrix4x4(
            c,    s,    0.0f, 0.0f,
            -s,   c,    0.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
        );
    }

    Matrix4x4 TransformMatrix::CreateScale(float x, float y, float z)
    {
        return Matrix4x4(
            x,    0.0f, 0.0f, 0.0f,
            0.0f, y,    0.0f, 0.0f,
            0.0f, 0.0f, z,    0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
        );
    }

    Matrix4x4 TransformMatrix::CreateShear(float xy, float xz, float yx, float yz, float zx, float zy)
    {
        // xy : x が y に依存してずれる量

        return Matrix4x4(
            1.0f, xy,   xz,   0.0f,
            yx,   1.0f, yz,   0.0f,
            zx,   zy,   1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
        );
    }

    Matrix4x4 ViewMatrix::CreateLookAt(const Vector3& eye, const Vector3& center, const Vector3& up)
    {
        constexpr bool referenceImplementation = false;
        if constexpr (referenceImplementation)
        {
            // 参考 https://registry.khronos.org/OpenGL-Refpages/gl2.1/xhtml/gluLookAt.xml

            Vector3 F = center - eye;

            Vector3 f = Vector3::Normalize(F);
            Vector3 UP = Vector3::Normalize(up);

            Vector3 s = Vector3::Normalize(f.cross(UP));// fix
            Vector3 u = s.cross(f);

            Matrix4x4 r(
                s.x, s.y, s.z, 0.0f,
                u.x, u.y, u.z, 0.0f,
                -f.x, -f.y, -f.z, 0.0f,
                0.0f, 0.0f, 0.0f, 1.0f
            );

            Matrix4x4 t(
                1.0f, 0.0f, 0.0f, -eye.x,
                0.0f, 1.0f, 0.0f, -eye.y,
                0.0f, 0.0f, 1.0f, -eye.z,
                0.0f, 0.0f, 0.0f, 1.0f
            );

            return r * t;
        }
        else
        {
            // カメラ行列
            // 
            //           + center
            //     +y   / 
            //       | /
            //       |/
            //   eye +---- +x 
            //      /
            //     /
            //  +z
            // 

            // note.
            // 
            // 外積と直交基底の関係
            // 
            //   y × z = x   
            //   z × x = y   
            //   x × y = z   
            // 
            //   (0, 1, 0) x (0, 0, 1) = (1, 0, 0)
            //   (0, 0, 1) x (1, 0, 0) = (0, 1, 0)
            //   (1, 0, 0) x (0, 1, 0) = (0, 0, 1)
            // 

            Vector3 zAxis = Vector3::Normalize(eye - center);
            Vector3 xAxis = Vector3::Normalize(up.cross(zAxis));
            Vector3 yAxis = zAxis.cross(xAxis);
            Matrix4x4 cameraMatrix = TransformMatrix::CreateBasisAndOrigin(xAxis, yAxis, zAxis, eye);

            return cameraMatrix.getInverseMatrix();
        }
    }

    Matrix4x4 ProjectionMatrix::CreateFrustum(float left, float right, float bottom, float top, float nearVal, float farVal)
    {
        //static bool referenceImplementation = false;
        //referenceImplementation = !referenceImplementation;
        //if (referenceImplementation)
        constexpr bool referenceImplementation = false;
        if constexpr (referenceImplementation)
        {
            // 参考 https://registry.khronos.org/OpenGL-Refpages/gl2.1/xhtml/glFrustum.xml

            float A = (right + left) / (right - left);
            float B = (top + bottom) / (top - bottom);
            float C = -(farVal + nearVal) / (farVal - nearVal);
            float D = -(2.0f * farVal * nearVal) / (farVal - nearVal);

            return Matrix4x4(
                (2.0f * nearVal) / (right - left), 0.0f, A, 0.0f,
                0.0f, (2.0f * nearVal) / (top - bottom), B, 0.0f,
                0.0f, 0.0f, C, D,
                0.0f, 0.0f, -1.0f, 0.0f
            );
        }
        else
        {
            // 視錐台の幅・高さ・奥行（符号無し）を求めておく
            float nearWidth = right - left;
            float nearHeight = top - bottom;

            // 左右対称になるようにニアクリップ面をずらす（Shear）
            float shxz = (right + left) / nearWidth;
            float shyz = (top + bottom) / nearHeight;


            // ニアクリップ面の上下左右 を [-near, near] に写像
            float sx = (nearVal + nearVal) / nearWidth;
            float sy = (nearVal + nearVal) / nearHeight;


            // 視体積の奥行範囲 を [-nearVal, farVal] に写像

            // zの写像式
            //  Zndc = (C * z + D) / -z    (1)

            // 条件
            //  z = -n のとき Zndc = -1    (2)
            //  z = -f のとき Zndc =  1    (3)   

            // まず C を求める

            // (1)、(2)より
            //  -1 = (C * -n + D) / n
            // 両辺に n を掛ける
            //  -n = C * -n + D    (4)

            // (1)、(3)より
            //  1 = (C * -f + D) / f
            // 両辺に f を掛ける
            //  f = C * -f + D    (5)

            // (4)から(5)を引いて D を消去
            //  -n - f = C * -n - C * -f
            //  -n - f = C * (-n + f)
            // 両辺を(-n + f)で割る
            //  (-n - f) / (-n + f) = C

            // よって C は
            //  C = -(f + n) / (f - n)    (6)

            // 次に D を求める

            // (4)を D= の式にする
            //  D = -n - C * -n
            //  D = (1 - C) * -n    (7)

            // (7)に(6)を代入
            //  D = (1 - (-(f + n) / (f - n))) * -n
            //  D = (1 + ((f + n) / (f - n))) * -n

            // 1 を (f - n) / (f - n) に置き換える
            //  D = (((f - n) / (f - n)) + ((f + n) / (f - n))) * -n
            //  D = ((f - n + f + n) / (f - n)) * -n
            //  D = ((f + f) / (f - n)) * -n
            //  D = ((f + f) * -n) / (f - n)

            // よって D は
            //  D = -(2 * f * n) / (f - n) 

            float sz = -(farVal + nearVal) / (farVal - nearVal);
            float tz = -(2.0f * farVal * nearVal) / (farVal - nearVal);

            // 変換後のベクトルの w' には変換前のベクトルの -z を入れる 
            float pdt = -1.0f;// perspective divide term.

            // 次の変換行列を返す
            // x' = sx * x + 0  * y + shxz * z + 0  * w
            // y' = 0  * x + sy * y + shyz * z + 0  * w
            // z' = 0  * x + 0  * y + sz   * z + tz * w
            // w' = 0  * x + 0  * y + pdt  * z + 0  * w
            return Matrix4x4(
                sx,   0.0f, shxz, 0.0f,
                0.0f, sy,   shyz, 0.0f,
                0.0f, 0.0f, sz,   tz,
                0.0f, 0.0f, pdt,  0.0f
            );
        }
    }

    Matrix4x4 ProjectionMatrix::CreatePerspective(float fovy, float aspect, float zNear, float zFar)
    {
        constexpr bool referenceImplementation = false;
        if constexpr (referenceImplementation)
        {
            // 参考 https://registry.khronos.org/OpenGL-Refpages/gl2.1/xhtml/gluPerspective.xml

            float f = 1.0f / std::tan(fovy / 2.0f);

            return Matrix4x4(
                f / aspect, 0.0f, 0.0f, 0.0f,
                0.0f, f, 0.0f, 0.0f,
                0.0f, 0.0f, (zFar + zNear) / (zNear - zFar), (2.0f * zFar * zNear) / (zNear - zFar),
                0.0f, 0.0f, -1.0f, 0.0f
            );
        }
        else
        {
            float halfHeight = zNear * std:: tan(fovy / 2.0f);
            float halfWidth = halfHeight * aspect;
            return CreateFrustum(-halfWidth, halfWidth, -halfHeight, halfHeight, zNear, zFar);
        }
    }
}
