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
        // xz : x が z に依存してずれる量
        // yx : y が x に依存してずれる量
        // yz : y が z に依存してずれる量
        // zx : z が x に依存してずれる量
        // zy : z が y に依存してずれる量

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
        // note.
        // 
        // 次を満たす変換行列 m を返す
        //
        // mv = v'
        //
        // v.x = [ left,     right ]
        // v.y = [ bottom,   top   ]
        // v.z = [-nearVal, -farVal]  ※カメラ空間は右手系（奥が -z） 
        // v.w = 1
        //
        // のとき
        //
        // v'.x = [-nearVal, nearVal]
        // v'.y = [-nearVal, nearVal]
        // v'.z = [-nearVal, farVal ]  ※クリップ空間は左手手系（奥が +z） 
        // v'.w = -v.z
        // 
        // また
        // 
        // v.xyz = (0, 0, 0)
        //
        // のとき
        //
        // v'.xyz = (0, 0, 0)　※視点(原点)は固定
        //

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
            // ニアクリップ面が左右非対称なら、視点（原点）は固定してニアクリップ面を中央に移動
            // x' = 1 * x + 0 * y + shxz * z + 0 * w
            // y' = 1 * x + 0 * y + shyz * z + 0 * w
            float shxz = (right + left) / (right - left);// Shear XZ
            float shyz = (top + bottom) / (top - bottom);

            // ニアクリップ面の上下左右 を [-near, near] にマップ
            // x' = sx * x + 0  * y + shxz * z + 0 * w
            // y' = 0  * x + sy * y + shyz * z + 0 * w
            float sx = (2.0f / (right - left)) * nearVal;// Scale X
            float sy = (2.0f / (top - bottom)) * nearVal;

            // 視体積の奥行範囲 [-nearVal, -farVal] を [-nearVal, farVal] にマップ
            // z' = 0 * x + 0 * y + sz * z + tz * w
            float sz = -(farVal + nearVal) / (farVal - nearVal);
            float tz = -(2.0f * farVal * nearVal) / (farVal - nearVal);// Translate Z

            // 変換後のベクトルの w' には変換前のベクトルの -z を入れる 
            // w' = 0 * x + 0 * y + pdt * z + 0 * w
            float pdt = -1.0f;// perspective divide term.

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
