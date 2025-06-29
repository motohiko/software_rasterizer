#include "Matrix.h"

// note.
// 
// 右手座標系の覚え方
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
//   座標軸は xyz = rgb で描画される
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
// 

namespace MyApp
{
    const Matrix4x4 Matrix4x4::kIdentity(
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    );

    // 余因子行列
    Matrix4x4 Matrix4x4::getAdjugateMatrix() const
    {
        float c00 = getCofactor(0, 0);
        float c01 = getCofactor(0, 1);
        float c02 = getCofactor(0, 2);
        float c03 = getCofactor(0, 3);
        float c10 = getCofactor(1, 0);
        float c11 = getCofactor(1, 1);
        float c12 = getCofactor(1, 2);
        float c13 = getCofactor(1, 3);
        float c20 = getCofactor(2, 0);
        float c21 = getCofactor(2, 1);
        float c22 = getCofactor(2, 2);
        float c23 = getCofactor(2, 3);
        float c30 = getCofactor(3, 0);
        float c31 = getCofactor(3, 1);
        float c32 = getCofactor(3, 2);
        float c33 = getCofactor(3, 3);

        Matrix4x4 cofactorMatrix(
            c00, c01, c02, c03,
            c10, c11, c12, c13,
            c20, c21, c22, c23,
            c30, c31, c32, c33
        );

        return cofactorMatrix.transpose();
    }

    // 逆行列
    Matrix4x4 Matrix4x4::inverse() const
    {
        Matrix4x4 a = getAdjugateMatrix();

        float det = getDeterminant();
        if (det == 0.0f)
        {
            throw std::domain_error("Matrix is singular and cannot be inverted.");
        }

        float invDet = (1.0f / det);

        return Matrix4x4(
            a.m00 * invDet, a.m01 * invDet, a.m02 * invDet, a.m03 * invDet,
            a.m10 * invDet, a.m11 * invDet, a.m12 * invDet, a.m13 * invDet,
            a.m20 * invDet, a.m21 * invDet, a.m22 * invDet, a.m23 * invDet,
            a.m30 * invDet, a.m31 * invDet, a.m32 * invDet, a.m33 * invDet
        );
    }

    Matrix4x4 Matrix4x4::createBasis(const Vector3& xAxis, const Vector3& yAxis, const Vector3& zAxis, const Vector3& origon)
    {
        return Matrix4x4(
            xAxis.x, yAxis.x, zAxis.x, origon.x,
            xAxis.y, yAxis.y, zAxis.y, origon.y,
            xAxis.z, yAxis.z, zAxis.z, origon.z,
            0.0f,    0.0f,    0.0f,    1.0f
        );
    }

    Matrix4x4 Matrix4x4::createRotationX(float angle)
    {
        float c = std::cosf(angle);
        float s = std::sinf(angle);

        return Matrix4x4(
            1.0f,  0.0f, 0.0f, 0.0f,
            0.0f,  c,    s,    0.0f,
            0.0f, -s,    c,    0.0f,
            0.0f,  0.0f, 0.0f, 1.0f
        );
    }

    Matrix4x4 Matrix4x4::createRotationY(float angle)
    {
        float c = std::cosf(angle);
        float s = std::sinf(angle);

        return Matrix4x4(
            c,    0.0f, -s,    0.0f,
            0.0f, 1.0f,  0.0f, 0.0f,
            s,    0.0f,  c,    0.0f,
            0.0f, 0.0f,  0.0f, 1.0f
        );
    }

    Matrix4x4 Matrix4x4::createScale(float x, float y, float z)
    {
        return Matrix4x4(
            x,    0.0f, 0.0f, 0.0f,
            0.0f, y,    0.0f, 0.0f,
            0.0f, 0.0f, z,    0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
        );
    }

    Matrix4x4 Matrix4x4::createShear(float xy, float xz, float yx, float yz, float zx, float zy)
    {
        return Matrix4x4(
            1.0f, yx,   zx,   0.0f,
            xy,   1.0f, zy,   0.0f,
            xz,   yz,   1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
        );
    }

    Matrix4x4 Matrix4x4::lockAt(const Vector3& eye, const Vector3& center, const Vector3& up)
    {
        if (false)
        {
            // see https://registry.khronos.org/OpenGL-Refpages/gl2.1/xhtml/gluLookAt.xml

            Vector3 F = center - eye;

            Vector3 f = F.normalize();
            Vector3 UP = up.normalize();

            Vector3 s = f.cross(UP).normalize();// fix
            Vector3 u = s.cross(f);

            Matrix4x4 r(
                 s.x,  s.y,  s.z,  0.0f,
                 u.x,  u.y,  u.z,  0.0f,
                -f.x, -f.y, -f.z,  0.0f,
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

            Vector3 zAxis = (eye - center).normalize();
            Vector3 xAxis = (up.cross(zAxis)).normalize();
            Vector3 yAxis = zAxis.cross(xAxis);
            Matrix4x4 cameraMatrix = createBasis(xAxis, yAxis, zAxis, eye);

            return cameraMatrix.inverse();// view matrix
        }
    }

    Matrix4x4 Matrix4x4::createFrustum(float left, float right, float bottom, float top, float nearVal, float farVal)
    {
        // 次を満たす変換行列 m を返す
        //
        // mv = v'
        //
        // v.x = [ left,     right ]
        // v.y = [ bottom,   top   ]
        // v.z = [-nearVal, -farVal]  ※カメラ空間座標系では視点より奥は -z になる
        // v.w = 1
        //
        // のとき
        //
        // v'.x = [-nearVal, nearVal]
        // v'.y = [-nearVal, nearVal]
        // v'.z = [-nearVal, farVal ]
        // v'.w = -v.z
        // 
        // また
        // 
        // v,xyz = (0, 0, 0)
        //
        // のとき
        //
        // v'xyz = (0, 0, 0)
        //
        //
        // v' はクリップ座標系となり、左手系に変わる
        //

        if (false)
        {
            // see https://registry.khronos.org/OpenGL-Refpages/gl2.1/xhtml/glFrustum.xml

            float A = (right + left) / (right - left);
            float B = (top + bottom) / (top - bottom);
            float C = -(farVal + nearVal) / (farVal - nearVal);
            float D = -(2.0f * farVal * nearVal) / (farVal - nearVal);

            return Matrix4x4(
                (2.0f * nearVal) / (right - left),  0.0f,                               A,    0.0f,
                0.0f,                               (2.0f * nearVal) / (top - bottom),  B,    0.0f,
                0.0f,                               0.0f,                               C,    D,
                0.0f,                               0.0f,                              -1.0f, 0.0f
            );
        }
        else
        {
            // 視体積の奥行範囲を [-nearVal, -farVal] から [-nearVal, farVal] にマップ
            // z' = m22 * z + m23
            float m22 = -(farVal + nearVal) / (farVal - nearVal);// C
            float m23 = -(2.0f * farVal * nearVal) / (farVal - nearVal);// D

            // 変換後のベクトルの w' に変換前のベクトルの -z を入れる 
            // w' = m32 * z + m33
            float m32 = -1.0f;
            float m33 = 0.0f;

            Matrix4x4 frustumZ(
                1.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 1.0f, 0.0f, 0.0f,
                0.0f, 0.0f, m22,  m23,
                0.0f, 0.0f, m32,  m33
            );

            // ニアクリップ面が左右非対称なら解消
            // ・視点（原点）はそのままにニアクリップ面を移動
            // ・視体積の中心軸＝Z軸になる
            float zx = (right + left) / (right - left);// A
            float zy = (top + bottom) / (top - bottom);// B
            Matrix4x4 shearXY = createShear(0.0f, 0.0f, 0.0f, 0.0f, zx, zy);

            // ニアクリップ面の寸法を [-near, near] にマップ
            float sx = (2.0f / (right - left)) * nearVal;
            float sy = (2.0f / (top - bottom)) * nearVal;
            Matrix4x4 scaleXY = createScale(sx, sy, 1.0f);

            return frustumZ * (shearXY * scaleXY);
        }
    }

    Matrix4x4 Matrix4x4::createProjection(float fovy, float aspect, float zNear, float zFar)
    {
        if (false)
        {
            // see https://registry.khronos.org/OpenGL-Refpages/gl2.1/xhtml/gluPerspective.xml

            float f = 1.0f / tan(fovy / 2.0f);

            return Matrix4x4(
                f / aspect, 0.0f, 0.0f,                            0.0f,
                0.0f,       f,    0.0f,                            0.0f,
                0.0f,       0.0f, (zFar + zNear) / (zNear - zFar), (2.0f * zFar * zNear) / (zNear - zFar),
                0.0f,       0.0f, -1.0f,                           0.0f
            );
        }
        else
        {
            float halfHeight = zNear * tan(fovy / 2.0f);
            float halfWidth = halfHeight * aspect;
            return createFrustum(-halfWidth, halfWidth, -halfHeight, halfHeight, zNear, zFar);
        }
    }
}
