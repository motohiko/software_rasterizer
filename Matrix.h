#pragma once
#include <cstdint>
#include <cmath>
#include <cassert>
#include <stdexcept>
#include "Vector.h"

namespace MyApp
{

    struct Matrix2x2
    {
        float m00, m01;
        float m10, m11;

        float getDeterminant() const
        {
            return m00 * m11 - m01 * m10;
        }
    };

    struct Matrix3x3
    {
        float m00, m01, m02;
        float m10, m11, m12;
        float m20, m21, m22;

        // 小行列/submatrix
        // 特定の行および列を取り除いた行列
        Matrix2x2 removeRowAndColumn(int row, int column) const
        {
            Matrix2x2 sm;
            switch (row)
            {
            case 0:
                switch (column)
                {
                case 0:
                    sm.m00 = m11; sm.m01 = m12;
                    sm.m10 = m21; sm.m11 = m22;
                    return sm;
                case 1:
                    sm.m00 = m10; sm.m01 = m12;
                    sm.m10 = m20; sm.m11 = m22;
                    return sm;
                case 2:
                    sm.m00 = m10; sm.m01 = m11;
                    sm.m10 = m20; sm.m11 = m21;
                    return sm;
                }
                break;
            case 1:
                switch (column)
                {
                case 0:
                    sm.m00 = m01; sm.m01 = m02;
                    sm.m10 = m21; sm.m11 = m22;
                    return sm;
                case 1:
                    sm.m00 = m00; sm.m01 = m02;
                    sm.m10 = m20; sm.m11 = m22;
                    return sm;
                case 2:
                    sm.m00 = m00; sm.m01 = m01;
                    sm.m10 = m20; sm.m11 = m21;
                    return sm;
                }
                break;
            case 2:
                switch (column)
                {
                case 0:
                    sm.m00 = m01; sm.m01 = m02;
                    sm.m10 = m11; sm.m11 = m12;
                    return sm;
                case 1:
                    sm.m00 = m00; sm.m01 = m02;
                    sm.m10 = m10; sm.m11 = m12;
                    return sm;
                case 2:
                    sm.m00 = m00; sm.m01 = m01;
                    sm.m10 = m10; sm.m11 = m11;
                    return sm;
                }
                break;
            }
            throw std::out_of_range("Index out of range");
        }

        // 余因子
        float getCofactor(int row, int column) const
        {
            return removeRowAndColumn(row, column).getDeterminant() * (float)std::pow(-1, row + column);// 偶数 +、奇数 -
        }

        // 行列式
        float getDeterminant() const
        {
            float c00 = getCofactor(0, 0);
            float c01 = getCofactor(0, 1);// *-1.0f;
            float c02 = getCofactor(0, 2);
            return m00 * c00 + m01 * c01 + m02 * c02;
        }
    };

    //     | m00 m01 m02 m03 |
    // m = | m10 m11 m12 m13 |
    //     | m20 m21 m22 m23 |
    //     | m30 m31 m32 m33 |
    struct Matrix4x4
    {
        float m00, m01, m02, m03;
        float m10, m11, m12, m13;
        float m20, m21, m22, m23;
        float m30, m31, m32, m33;

        static const Matrix4x4 kIdentity;

        Matrix4x4() = default;

        Matrix4x4(const Vector4& row0, const Vector4& row1, const Vector4& row2, const Vector4& row3)
        {
            m00 = row0.x; m01 = row0.y; m02 = row0.z; m03 = row0.w;
            m10 = row1.x; m11 = row1.y; m12 = row1.z; m13 = row1.w;
            m20 = row2.x; m21 = row2.y; m22 = row2.z; m23 = row2.w;
            m30 = row3.x; m31 = row3.y; m32 = row3.z; m33 = row3.w;
        }

        static Matrix4x4 createIdentity()
        {
            Matrix4x4 id;
            id.m00 = 1.0f; id.m01 = 0.0f; id.m02 = 0.0f; id.m03 = 0.0f;
            id.m10 = 0.0f; id.m11 = 1.0f; id.m12 = 0.0f; id.m13 = 0.0f;
            id.m20 = 0.0f; id.m21 = 0.0f; id.m22 = 1.0f; id.m23 = 0.0f;
            id.m30 = 0.0f; id.m31 = 0.0f; id.m32 = 0.0f; id.m33 = 1.0f;
            return id;
        }

        static Matrix4x4 createRotationX(float angle)
        {
            Matrix4x4 rot = Matrix4x4::kIdentity;
            rot.m11 = cos(angle); rot.m12 = sin(angle);
            rot.m21 = -sin(angle); rot.m22 = cos(angle);
            return rot;
        }

        static Matrix4x4 createRotationY(float angle)
        {
            Matrix4x4 rot = Matrix4x4::kIdentity;
            rot.m00 = cos(angle); rot.m02 = -sin(angle);
            rot.m20 = sin(angle); rot.m22 = cos(angle);
            return rot;
        }

        // | x 0 0 0 |
        // | 0 y 0 0 |
        // | 0 0 z 0 |
        // | 0 0 0 1 |
        static Matrix4x4 createScale(float x, float y, float z)
        {
            Matrix4x4 scale;
            scale.m00 = x;    scale.m01 = 0.0f; scale.m02 = 0.0f; scale.m03 = 0.0f;
            scale.m10 = 0.0f; scale.m11 = y;    scale.m12 = 0.0f; scale.m13 = 0.0f;
            scale.m20 = 0.0f; scale.m21 = 0.0f; scale.m22 = z;    scale.m23 = 0.0f;
            scale.m30 = 0.0f; scale.m31 = 0.0f; scale.m32 = 0.0f; scale.m33 = 1.0f;
            return scale;
        }

        // |  1 yx zx  0 |
        // | xy  1 zy  0 |
        // | xz yz  1  0 |
        // |  0  0  0  1 |
        static Matrix4x4 createShear(float xy, float xz, float yx, float yz, float zx, float zy)
        {
            Matrix4x4 shear;
            shear.m00 = 1.0f; shear.m01 = yx;   shear.m02 = zx;   shear.m03 = 0.0f;
            shear.m10 = xy;   shear.m11 = 1.0f; shear.m12 = zy;   shear.m13 = 0.0f;
            shear.m20 = xz;   shear.m21 = yz;   shear.m22 = 1.0f; shear.m23 = 0.0f;
            shear.m30 = 0.0f; shear.m31 = 0.0f; shear.m32 = 0.0f; shear.m33 = 1.0f;
            return shear;
        }

        Matrix4x4 operator*(const Matrix4x4& other) const
        {
            Matrix4x4 ret;
            ret.m00 = m00 * other.m00 + m01 * other.m10 + m02 * other.m20 + m03 * other.m30;
            ret.m01 = m00 * other.m01 + m01 * other.m11 + m02 * other.m21 + m03 * other.m31;
            ret.m02 = m00 * other.m02 + m01 * other.m12 + m02 * other.m22 + m03 * other.m32;
            ret.m03 = m00 * other.m03 + m01 * other.m13 + m02 * other.m23 + m03 * other.m33;
            ret.m10 = m10 * other.m00 + m11 * other.m10 + m12 * other.m20 + m13 * other.m30;
            ret.m11 = m10 * other.m01 + m11 * other.m11 + m12 * other.m21 + m13 * other.m31;
            ret.m12 = m10 * other.m02 + m11 * other.m12 + m12 * other.m22 + m13 * other.m32;
            ret.m13 = m10 * other.m03 + m11 * other.m13 + m12 * other.m23 + m13 * other.m33;
            ret.m20 = m20 * other.m00 + m21 * other.m10 + m22 * other.m20 + m23 * other.m30;
            ret.m21 = m20 * other.m01 + m21 * other.m11 + m22 * other.m21 + m23 * other.m31;
            ret.m22 = m20 * other.m02 + m21 * other.m12 + m22 * other.m22 + m23 * other.m32;
            ret.m23 = m20 * other.m03 + m21 * other.m13 + m22 * other.m23 + m23 * other.m33;
            ret.m30 = m30 * other.m00 + m31 * other.m10 + m32 * other.m20 + m33 * other.m30;
            ret.m31 = m30 * other.m01 + m31 * other.m11 + m32 * other.m21 + m33 * other.m31;
            ret.m32 = m30 * other.m02 + m31 * other.m12 + m32 * other.m22 + m33 * other.m32;
            ret.m33 = m30 * other.m03 + m31 * other.m13 + m32 * other.m23 + m33 * other.m33;
            return ret;
        }

        // フレンド関数として右項にベクトルを持つ乗算演算子のオーバーロード
        friend Vector4 operator*(const Matrix4x4& mat, const Vector4& vec)
        {
            Vector4 ret;
            ret.x = mat.m00 * vec.x + mat.m01 * vec.y + mat.m02 * vec.z + mat.m03 * vec.w;
            ret.y = mat.m10 * vec.x + mat.m11 * vec.y + mat.m12 * vec.z + mat.m13 * vec.w;
            ret.z = mat.m20 * vec.x + mat.m21 * vec.y + mat.m22 * vec.z + mat.m23 * vec.w;
            ret.w = mat.m30 * vec.x + mat.m31 * vec.y + mat.m32 * vec.z + mat.m33 * vec.w;
            return ret;
        }

        Matrix4x4 transpose() const
        {
            Matrix4x4 t;
            t.m00 = m00; t.m01 = m10; t.m02 = m20; t.m03 = m30;
            t.m10 = m01; t.m11 = m11; t.m12 = m21; t.m13 = m31;
            t.m20 = m02; t.m21 = m12; t.m22 = m22; t.m23 = m32;
            t.m30 = m03; t.m31 = m13; t.m32 = m23; t.m33 = m33;
            return t;
        }

        void SetColumn(int index, const Vector4& column)
        {
            switch (index)
            {
            case 0:
                m00 = column.x;
                m10 = column.y;
                m20 = column.z;
                m30 = column.w;
                break;
            case 1:
                m01 = column.x;
                m11 = column.y;
                m21 = column.z;
                m31 = column.w;
                break;
            case 2:
                m02 = column.x;
                m12 = column.y;
                m22 = column.z;
                m32 = column.w;
                break;
            case 3:
                m03 = column.x;
                m13 = column.y;
                m23 = column.z;
                m33 = column.w;
                break;
            }
        }

        Vector4 GetRow(int index) const
        {
            switch (index)
            {
            case 0: return Vector4(m00, m01, m02, m03);
            case 1: return Vector4(m10, m11, m12, m13);
            case 2: return Vector4(m20, m21, m22, m23);
            case 3: return Vector4(m30, m31, m32, m33);
            default:
                throw std::out_of_range("Index out of range");
            }
        }

        // 小行列
        // submatrix
        // 特定の行および列を取り除いた行列
        Matrix3x3 removeRowAndColumn(int rowIndex, int columnIndex) const
        {
            Matrix3x3 sm;
            switch (rowIndex)
            {
            case 0:
                switch (columnIndex)
                {
                case 0:
                    sm.m00 = m11; sm.m01 = m12; sm.m02 = m13;
                    sm.m10 = m21; sm.m11 = m22; sm.m12 = m23;
                    sm.m20 = m31; sm.m21 = m32; sm.m22 = m33;
                    return sm;
                case 1:
                    sm.m00 = m10; sm.m01 = m12; sm.m02 = m13;
                    sm.m10 = m20; sm.m11 = m22; sm.m12 = m23;
                    sm.m20 = m30; sm.m21 = m32; sm.m22 = m33;
                    return sm;
                case 2:
                    sm.m00 = m10; sm.m01 = m11; sm.m02 = m13;
                    sm.m10 = m20; sm.m11 = m21; sm.m12 = m23;
                    sm.m20 = m30; sm.m21 = m31; sm.m22 = m33;
                    return sm;
                case 3:
                    sm.m00 = m10; sm.m01 = m11; sm.m02 = m12;
                    sm.m10 = m20; sm.m11 = m21; sm.m12 = m22;
                    sm.m20 = m30; sm.m21 = m31; sm.m22 = m32;
                    return sm;
                }
                break;
            case 1:
                switch (columnIndex)
                {
                case 0:
                    sm.m00 = m01; sm.m01 = m02; sm.m02 = m03;
                    sm.m10 = m21; sm.m11 = m22; sm.m12 = m23;
                    sm.m20 = m31; sm.m21 = m32; sm.m22 = m33;
                    return sm;
                case 1:
                    sm.m00 = m00; sm.m01 = m02; sm.m02 = m03;
                    sm.m10 = m20; sm.m11 = m22; sm.m12 = m23;
                    sm.m20 = m30; sm.m21 = m32; sm.m22 = m33;
                    return sm;
                case 2:
                    sm.m00 = m00; sm.m01 = m01; sm.m02 = m03;
                    sm.m10 = m20; sm.m11 = m21; sm.m12 = m23;
                    sm.m20 = m30; sm.m21 = m31; sm.m22 = m33;
                    return sm;
                case 3:
                    sm.m00 = m00; sm.m01 = m01; sm.m02 = m02;
                    sm.m10 = m20; sm.m11 = m21; sm.m12 = m22;
                    sm.m20 = m30; sm.m21 = m31; sm.m22 = m32;
                    return sm;
                }
                break;
            case 2:
                switch (columnIndex)
                {
                case 0:
                    sm.m00 = m01; sm.m01 = m02; sm.m02 = m03;
                    sm.m10 = m11; sm.m11 = m12; sm.m12 = m13;
                    sm.m20 = m31; sm.m21 = m32; sm.m22 = m33;
                    return sm;
                case 1:
                    sm.m00 = m00; sm.m01 = m02; sm.m02 = m03;
                    sm.m10 = m10; sm.m11 = m12; sm.m12 = m13;
                    sm.m20 = m30; sm.m21 = m32; sm.m22 = m33;
                    return sm;
                case 2:
                    sm.m00 = m00; sm.m01 = m01; sm.m02 = m03;
                    sm.m10 = m10; sm.m11 = m11; sm.m12 = m13;
                    sm.m20 = m30; sm.m21 = m31; sm.m22 = m33;
                    return sm;
                case 3:
                    sm.m00 = m00; sm.m01 = m01; sm.m02 = m02;
                    sm.m10 = m10; sm.m11 = m11; sm.m12 = m12;
                    sm.m20 = m30; sm.m21 = m31; sm.m22 = m32;
                    return sm;
                }
                break;
            case 3:
                switch (columnIndex)
                {
                case 0:
                    sm.m00 = m01; sm.m01 = m02; sm.m02 = m03;
                    sm.m10 = m11; sm.m11 = m12; sm.m12 = m13;
                    sm.m20 = m21; sm.m21 = m22; sm.m22 = m23;
                    return sm;
                case 1:
                    sm.m00 = m00; sm.m01 = m02; sm.m02 = m03;
                    sm.m10 = m10; sm.m11 = m12; sm.m12 = m13;
                    sm.m20 = m20; sm.m21 = m22; sm.m22 = m23;
                    return sm;
                case 2:
                    sm.m00 = m00; sm.m01 = m01; sm.m02 = m03;
                    sm.m10 = m10; sm.m11 = m11; sm.m12 = m13;
                    sm.m20 = m20; sm.m21 = m21; sm.m22 = m23;
                    return sm;
                case 3:
                    sm.m00 = m00; sm.m01 = m01; sm.m02 = m02;
                    sm.m10 = m10; sm.m11 = m11; sm.m12 = m12;
                    sm.m20 = m20; sm.m21 = m21; sm.m22 = m22;
                    return sm;
                }
                break;
            }
            throw std::out_of_range("Index out of range");
        }

        // 余因子
        float getCofactor(int row, int column) const
        {
            return removeRowAndColumn(row, column).getDeterminant() * (float)std::pow(-1, row + column);// 偶数 +、奇数 -;
        }

        // 行列式
        float getDeterminant() const
        {
            float c00 = getCofactor(0, 0);
            float c01 = getCofactor(0, 1);// *-1.0f;
            float c02 = getCofactor(0, 2);
            float c03 = getCofactor(0, 3);// * -1.0f;
            return m00 * c00 + m01 * c01 + m02 * c02 + m03 * c03;
        }

        // 逆行列
        Matrix4x4 inverse() const
        {
            float c00 = getCofactor(0, 0);
            float c01 = getCofactor(0, 1);// * -1.0f;
            float c02 = getCofactor(0, 2);
            float c03 = getCofactor(0, 3);// * -1.0f;
            float det = m00 * c00 + m01 * c01 + m02 * c02 + m03 * c03;
            if (det == 0.0f)
            {
                //throw runtime_error("Matrix is singular and cannot be inverted.");
            }

            float c10 = getCofactor(1, 0);// * -1.0f;
            float c11 = getCofactor(1, 1);
            float c12 = getCofactor(1, 2);// * -1.0f;
            float c13 = getCofactor(1, 3);
            float c20 = getCofactor(2, 0);
            float c21 = getCofactor(2, 1);// * -1.0f;
            float c22 = getCofactor(2, 2);
            float c23 = getCofactor(2, 3);// * -1.0f;
            float c30 = getCofactor(3, 0);// * -1.0f;
            float c31 = getCofactor(3, 1);
            float c32 = getCofactor(3, 2);// * -1.0f;
            float c33 = getCofactor(3, 3);

            // 余因子行列
            Matrix4x4 adj;
            adj.m00 = c00; adj.m01 = c10; adj.m02 = c20; adj.m03 = c30;
            adj.m10 = c01; adj.m11 = c11; adj.m12 = c21; adj.m13 = c31;
            adj.m20 = c02; adj.m21 = c12; adj.m22 = c22; adj.m23 = c32;
            adj.m30 = c03; adj.m31 = c13; adj.m32 = c23; adj.m33 = c33;

            // 逆行行列
            float invDet = (1.0f / det);
            Matrix4x4 inv;
            inv.m00 = adj.m00 * invDet; inv.m01 = adj.m01 * invDet; inv.m02 = adj.m02 * invDet; inv.m03 = adj.m03 * invDet;
            inv.m10 = adj.m10 * invDet; inv.m11 = adj.m11 * invDet; inv.m12 = adj.m12 * invDet; inv.m13 = adj.m13 * invDet;
            inv.m20 = adj.m20 * invDet; inv.m21 = adj.m21 * invDet; inv.m22 = adj.m22 * invDet; inv.m23 = adj.m23 * invDet;
            inv.m30 = adj.m30 * invDet; inv.m31 = adj.m31 * invDet; inv.m32 = adj.m32 * invDet; inv.m33 = adj.m33 * invDet;
            return inv;
        }




        // OpenGL系は手前が z プラス
        static Matrix4x4 createLockAt(const Vector3& eye, const Vector3& center, const Vector3& up)
        {
            // see https://registry.khronos.org/OpenGL-Refpages/gl2.1/xhtml/gluLookAt.xml

            Vector3 f = center - eye;
            Vector3 f2 = f.normalize();
            Vector3 up2 = up.normalize();
            // khronos.org の式がちょっと間違ってる
            //Vector3 s = f2.cross(up2);
            //Vector3 u = s.normalize().cross(f2);
            Vector3 s = f2.cross(up2).normalize();
            Vector3 u = s.cross(f2);
#if 0// 転置を使う場合
            Matrix4x4 r;
            r.m00 = s.x; r.m01 = s.y; r.m02 = s.z; r.m03 = 0.0f;
            r.m10 = u.x; r.m11 = u.y; r.m12 = u.z; r.m13 = 0.0f;
            r.m20 = -f2.x; r.m21 = -f2.y; r.m22 = -f2.z; r.m23 = 0.0f;
            r.m30 = 0.0f; r.m31 = 0.0f; r.m32 = 0.0f; r.m33 = 1.0f;
            Matrix4x4 t;
            t.m00 = 1.0f; t.m01 = 0.0f; t.m02 = 0.0f; t.m03 = -eye.x;
            t.m10 = 0.0f; t.m11 = 1.0f; t.m12 = 0.0f; t.m13 = -eye.y;
            t.m20 = 0.0f; t.m21 = 0.0f; t.m22 = 1.0f; t.m23 = -eye.z;
            t.m30 = 0.0f; t.m31 = 0.0f; t.m32 = 0.0f; t.m33 = 1.0f;
            Matrix4x4 view = r * t;
#else
            // camera matrix
            // 
            //          * c
            //    y+   / 
            //      | /
            //      |/
            //    e *---- x+ 
            //     /
            //    /
            //  z+
            // 
            // e : eye
            // c : cetner
            // x+ : s2
            // y+ : up2
            // z+ : -f2
            // 
            Matrix4x4 cam;
            cam.m00 = s.x; cam.m01 = u.x; cam.m02 = -f2.x; cam.m03 = eye.x;
            cam.m10 = s.y; cam.m11 = u.y; cam.m12 = -f2.y; cam.m13 = eye.y;
            cam.m20 = s.z; cam.m21 = u.z; cam.m22 = -f2.z; cam.m23 = eye.z;
            cam.m30 = 0.0f; cam.m31 = 0.0f; cam.m32 = 0.0f; cam.m33 = 1.0f;
            Matrix4x4 view = cam.inverse();
#endif
            return view;
        }

        // OpenGL系は z が [-1, 1]
        static Matrix4x4 createFrustum(float left, float right, float bottom, float top, float near, float far)
        {
            // see https://registry.khronos.org/OpenGL-Refpages/gl2.1/xhtml/glFrustum.xml

#if !0// 直書きする場合
            Matrix4x4 frust;
            float a = (right + left) / (right - left);
            float b = (top + bottom) / (top - bottom);
            float c = -(far + near) / (far - near);
            float d = -(2.0f * far * near) / (far - near);
            frust.m00 = (2.0f * near) / (right - left); frust.m01 = 0.0f; frust.m02 = a; frust.m03 = 0.0f;
            frust.m10 = 0.0f; frust.m11 = (2.0f * near) / (top - bottom); frust.m12 = b; frust.m13 = 0.0f;
            frust.m20 = 0.0f; frust.m21 = 0.0f; frust.m22 = c; frust.m23 = d;
            frust.m30 = 0.0f; frust.m31 = 0.0f; frust.m32 = -1.0f; frust.m33 = 0.0f;
            return frust;
#else// TODO
            Matrix4x4 shear = createShear(0.0f, 0.0f, 0.0f, 0.0f, (left + right) / (2.0f * near), (top + bottom) / (2.0f * near));
            Matrix4x4 scale = createScale(2.0f / (right - left), 2.0f / (top - bottom), 2.0f / (far - near));
            Matrix4x4 perspective;
            perspective.m00 = near; perspective.m01 = 0.0f; perspective.m02 = 0.0f; perspective.m03 = 0.0f;
            perspective.m10 = 0.0f; perspective.m11 = near; perspective.m12 = 0.0f; perspective.m13 = 0.0f;
            perspective.m20 = 0.0f; perspective.m21 = 0.0f; perspective.m22 = (far + near) / 2.0f; perspective.m23 = far * near;
            perspective.m30 = 0.0f; perspective.m31 = 0.0f; perspective.m32 = -1.0f; perspective.m33 = 1.0f;
            return shear * scale * perspective;
#endif
        }

        static Matrix4x4 createProjection(float fovy, float aspect, float near, float far)
        {
            // see https://registry.khronos.org/OpenGL-Refpages/gl2.1/xhtml/gluPerspective.xml

#if 0// 直書きする場合
            Matrix4x4 proj = Matrix4x4::kIdentity;
            float f = 1.0f / tan(fovy / 2.0f);
            proj.m00 = f / aspect;
            proj.m11 = f;
            proj.m22 = (far + near) / (near - far);
            proj.m23 = (2.0f * far * near) / (near - far);
            proj.m32 = -1.0f;
            proj.m33 = 0.0f;
            return proj;
#else
            float top = near * tan(fovy / 2.0f);
            float bottom = -top;
            float right = top * aspect;
            float left = -right;
            return createFrustum(left, right, bottom, top, near, far);
#endif
        }

    };
}
