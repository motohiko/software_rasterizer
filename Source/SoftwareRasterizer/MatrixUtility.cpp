#include "MatrixUtility.h"
#include <cmath>

namespace SoftwareRasterizer
{
    Matrix4x4 MatrixUtility::CreateBasis(const Vector3& xAxis, const Vector3& yAxis, const Vector3& zAxis, const Vector3& origon)
    {
        //     | xAxis.x yAxis.x zAxis.x origon.x |
        // m = | xAxis.y yAxis.y zAxis.y origon.y |
        //     | xAxis.z yAxis.z zAxis.z origon.z |
        //     | 0       0       0       1        |

        return Matrix4x4(
            xAxis.x, yAxis.x, zAxis.x, origon.x,
            xAxis.y, yAxis.y, zAxis.y, origon.y,
            xAxis.z, yAxis.z, zAxis.z, origon.z,
            0.0f, 0.0f, 0.0f, 1.0f
        );
    }

    Matrix4x4 MatrixUtility::CreateRotationX(float angle)
    {
        float c = std::cos(angle);
        float s = std::sin(angle);

        return Matrix4x4(
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, c,    s,    0.0f,
            0.0f, -s,   c,    0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
        );
    }

    Matrix4x4 MatrixUtility::CreateRotationY(float angle)
    {
        float c = std::cos(angle);
        float s = std::sin(angle);

        return Matrix4x4(
            c,    0.0f, -s,   0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            s,    0.0f, c,    0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
        );
    }

    Matrix4x4 MatrixUtility::CreateRotationZ(float angle)
    {
        float c = std::cos(angle);
        float s = std::sin(angle);

        return Matrix4x4(
            c,    s,    0.0f, 0.0f,
            -s,   c,    0.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
        );
    }

    Matrix4x4 MatrixUtility::CreateScale(float x, float y, float z)
    {
        // https://registry.khronos.org/OpenGL-Refpages/gl2.1/xhtml/glScale.xml

        return Matrix4x4(
            x,    0.0f, 0.0f, 0.0f,
            0.0f, y,    0.0f, 0.0f,
            0.0f, 0.0f, z,    0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
        );
    }

    Matrix4x4 MatrixUtility::CreateShear(float xy, float xz, float yx, float yz, float zx, float zy)
    {
        return Matrix4x4(
            1.0f, yx,   zx,   0.0f,
            xy,   1.0f, zy,   0.0f,
            xz,   yz,   1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
        );
    }

    static Matrix4x4 CreateTranslate(float x, float y, float z)
    {
        // https://registry.khronos.org/OpenGL-Refpages/gl2.1/xhtml/glTranslate.xml

        return Matrix4x4(
            1.0f, 0.0f, 0.0f, x,
            0.0f, 1.0f, 0.0f, y,
            0.0f, 0.0f, 1.0f, z,
            0.0f, 0.0f, 0.0f, 1.0f
        );

    }


    Matrix4x4 MatrixUtility::CreateLookAt(const Vector3& eye, const Vector3& center, const Vector3& up)
    {
        const bool referenceImplementation = false;
        if (referenceImplementation)
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
            // note.
            // 
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

            // 
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
            Matrix4x4 cameraMatrix = MatrixUtility::CreateBasis(xAxis, yAxis, zAxis, eye);

            return cameraMatrix.getInverseMatrix();
        }
    }

    Matrix4x4 MatrixUtility::CreateFrustum(float left, float right, float bottom, float top, float nearVal, float farVal)
    {
        // note.
        // 
        // 次を満たす変換行列 m を返す
        //
        // mv = v'
        //
        // v.x = [ left,     right ]
        // v.y = [ bottom,   top   ]
        // v.z = [-nearVal, -farVal]  ※カメラ空間座標系は右手系（奥が -z） 
        // v.w = 1
        //
        // のとき
        //
        // v'.x = [-nearVal, nearVal]
        // v'.y = [-nearVal, nearVal]
        // v'.z = [-nearVal, farVal ]  ※クリップ空間座標系は左手手系（奥が +z） 
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

        const bool referenceImplementation = false;
        if (referenceImplementation)
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
            float m02 = (right + left) / (right - left);
            float m12 = (top + bottom) / (top - bottom);
            Matrix4x4 shearXY = MatrixUtility::CreateShear(0.0f, 0.0f, 0.0f, 0.0f, m02, m12);

            // ニアクリップ面の上下左右の範囲を [-1, 1] から [-near, near] にマップ
            float m00 = (2.0f / (right - left)) * nearVal;
            float m11 = (2.0f / (top - bottom)) * nearVal;
            Matrix4x4 scaleXY = MatrixUtility::CreateScale(m00, m11, 1.0f);

            // 視体積の奥行範囲を [-nearVal, -farVal] から [-nearVal, farVal] にマップ
            // z' = m22 * z + m23
            float m22 = -(farVal + nearVal) / (farVal - nearVal);
            float m23 = -(2.0f * farVal * nearVal) / (farVal - nearVal);

            // 変換後のベクトルの w' に変換前のベクトルの -z を入れる 
            // w' = m32 * z + m33
            float m32 = -1.0f;
            float m33 = 0.0f;

            // w' の式に z (z'ではない)が含まれているので、zw変換は１行列に直書き
            Matrix4x4 frustumZW(
                1.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 1.0f, 0.0f, 0.0f,
                0.0f, 0.0f, m22, m23,
                0.0f, 0.0f, m32, m33
            );

            return frustumZW * scaleXY * shearXY;
        }
    }

    Matrix4x4 MatrixUtility::CreatePerspective(float fovy, float aspect, float zNear, float zFar)
    {
        const bool referenceImplementation = false;
        if (referenceImplementation)
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
