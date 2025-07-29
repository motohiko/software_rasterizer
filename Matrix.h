#pragma once

#include "Vector.h"
#include <cstdint>
#include <cmath>
#include <cassert>
#include <stdexcept>

namespace SoftwareRenderer
{

    // m = | m00 m01 |
    //     | m10 m11 |
     struct Matrix2x2
    {
        float m00, m01;
        float m10, m11;

        Matrix2x2() = default;

        Matrix2x2(
            float m00, float m01,
            float m10, float m11
        ) :
            m00(m00), m01(m01),
            m10(m10), m11(m11)
        {
        }

        // 行列式
        float getDeterminant() const
        {
            float c00 = m11;
            float c01 = m10 * -1.0f;
            return m00 * c00 + m01 * c01;
        }
    };

    //     | m00 m01 m02 |
    // m = | m10 m11 m12 |
    //     | m20 m21 m22 |
    struct Matrix3x3
    {
        float m00, m01, m02;
        float m10, m11, m12;
        float m20, m21, m22;

        Matrix3x3() = default;

        Matrix3x3(
            float m00, float m01, float m02,
            float m10, float m11, float m12,
            float m20, float m21, float m22
        ) :
            m00(m00), m01(m01), m02(m02),
            m10(m10), m11(m11), m12(m12),
            m20(m20), m21(m21), m22(m22)
        {
        }

        // 特定の行および列を取り除いた小行列
        Matrix2x2 removeRowAndColumn(int rowIndex, int columnIndex) const
        {
            switch (rowIndex)
            {
            case 0:
                switch (columnIndex)
                {
                case 0:
                    return Matrix2x2(
                        m11, m12,
                        m21, m22
                    );
                case 1:
                    return Matrix2x2(
                        m10, m12,
                        m20, m22
                    );
                case 2:
                    return Matrix2x2(
                        m10, m11,
                        m20, m21
                    );
                }
                break;
            case 1:
                switch (columnIndex)
                {
                case 0:
                    return Matrix2x2(
                        m01, m02,
                        m21, m22
                    );
                case 1:
                    return Matrix2x2(
                        m00, m02,
                        m20, m22
                    );
                case 2:
                    return Matrix2x2(
                        m00, m01,
                        m20, m21
                    );
                }
                break;
            case 2:
                switch (columnIndex)
                {
                case 0:
                    return Matrix2x2(
                        m01, m02,
                        m11, m12
                    );
                case 1:
                    return Matrix2x2(
                        m00, m02,
                        m10, m12
                    );
                case 2:
                    return Matrix2x2(
                        m00, m01,
                        m10, m11
                    );
                }
                break;
            }

            throw std::out_of_range("Index out of range");
            return Matrix2x2(
                0.0f, 0.0f,
                0.0f, 0.0f
            );
        }

        // 余因子
        float getCofactor(int row, int column) const
        {
            Matrix2x2 subMatrix = removeRowAndColumn(row, column);
            return subMatrix.getDeterminant() * std::powf(-1.0f, (float)(row + column));// 偶数 +、奇数 -
        }

        // 行列式
        float getDeterminant() const
        {
            float c00 = getCofactor(0, 0);
            float c01 = getCofactor(0, 1);
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

        Matrix4x4(
            float m00, float m01, float m02, float m03,
            float m10, float m11, float m12, float m13,
            float m20, float m21, float m22, float m23,
            float m30, float m31, float m32, float m33
        ) :
            m00(m00), m01(m01), m02(m02), m03(m03),
            m10(m10), m11(m11), m12(m12), m13(m13),
            m20(m20), m21(m21), m22(m22), m23(m23),
            m30(m30), m31(m31), m32(m32), m33(m33)
        {
        }

        void setRow(int index, const Vector4& row)
        {
            switch (index)
            {
            case 0: m00 = row.x; m01 = row.y; m02 = row.z; m03 = row.w; break;
            case 1: m11 = row.x; m11 = row.y; m12 = row.z; m13 = row.w; break;
            case 2: m20 = row.x; m21 = row.y; m22 = row.z; m23 = row.w; break;
            case 3: m30 = row.x; m31 = row.y; m32 = row.z; m33 = row.w; break;
            }
        }

        Vector4 getRow(int index) const
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

        void setColumn(int index, const Vector4& column)
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

        Matrix4x4 operator*(const Matrix4x4& r) const
        {
            return  Matrix4x4(

                // m00, m01, m02, m03
                m00 * r.m00 + m01 * r.m10 + m02 * r.m20 + m03 * r.m30,
                m00 * r.m01 + m01 * r.m11 + m02 * r.m21 + m03 * r.m31,
                m00 * r.m02 + m01 * r.m12 + m02 * r.m22 + m03 * r.m32,
                m00 * r.m03 + m01 * r.m13 + m02 * r.m23 + m03 * r.m33,

                // m10, m11, m12, m13
                m10 * r.m00 + m11 * r.m10 + m12 * r.m20 + m13 * r.m30,
                m10 * r.m01 + m11 * r.m11 + m12 * r.m21 + m13 * r.m31,
                m10 * r.m02 + m11 * r.m12 + m12 * r.m22 + m13 * r.m32,
                m10 * r.m03 + m11 * r.m13 + m12 * r.m23 + m13 * r.m33,

                // m20, m21, m22, m23
                m20 * r.m00 + m21 * r.m10 + m22 * r.m20 + m23 * r.m30,
                m20 * r.m01 + m21 * r.m11 + m22 * r.m21 + m23 * r.m31,
                m20 * r.m02 + m21 * r.m12 + m22 * r.m22 + m23 * r.m32,
                m20 * r.m03 + m21 * r.m13 + m22 * r.m23 + m23 * r.m33,

                // m30, m31, m32, m33
                m30 * r.m00 + m31 * r.m10 + m32 * r.m20 + m33 * r.m30,
                m30 * r.m01 + m31 * r.m11 + m32 * r.m21 + m33 * r.m31,
                m30 * r.m02 + m31 * r.m12 + m32 * r.m22 + m33 * r.m32,
                m30 * r.m03 + m31 * r.m13 + m32 * r.m23 + m33 * r.m33 
            );
        }

        // 右項にベクトルを持つ乗算演算子
        friend Vector4 operator*(const Matrix4x4& mat, const Vector4& vec)
        {
            return Vector4(
                mat.m00 * vec.x + mat.m01 * vec.y + mat.m02 * vec.z + mat.m03 * vec.w,
                mat.m10 * vec.x + mat.m11 * vec.y + mat.m12 * vec.z + mat.m13 * vec.w,
                mat.m20 * vec.x + mat.m21 * vec.y + mat.m22 * vec.z + mat.m23 * vec.w,
                mat.m30 * vec.x + mat.m31 * vec.y + mat.m32 * vec.z + mat.m33 * vec.w
            );
        }

        Matrix4x4 transpose() const
        {
            return Matrix4x4(
                m00, m10, m20, m30,
                m01, m11, m21, m31,
                m02, m12, m22, m32,
                m03, m13, m23, m33
            );
        }

        // 特定の行および列を取り除いた小行列
        Matrix3x3 removeRowAndColumn(int rowIndex, int columnIndex) const
        {
            switch (rowIndex)
            {
            case 0:
                switch (columnIndex)
                {
                case 0:
                    return Matrix3x3(
                        m11, m12, m13,
                        m21, m22, m23,
                        m31, m32, m33
                    );
                case 1:
                    return Matrix3x3(
                        m10, m12, m13,
                        m20, m22, m23,
                        m30, m32, m33
                    );
                case 2:
                    return Matrix3x3(
                        m10, m11, m13,
                        m20, m21, m23,
                        m30, m31, m33
                    );
                case 3:
                    return Matrix3x3(
                        m10, m11, m12,
                        m20, m21, m22,
                        m30, m31, m32
                    );
                }
                break;
            case 1:
                switch (columnIndex)
                {
                case 0:
                    return Matrix3x3(
                        m01, m02, m03,
                        m21, m22, m23,
                        m31, m32, m33
                    );
                case 1:
                    return Matrix3x3(
                        m00, m02, m03,
                        m20, m22, m23,
                        m30, m32, m33
                    );
                case 2:
                    return Matrix3x3(
                        m00, m01, m03,
                        m20, m21, m23,
                        m30, m31, m33
                    );
                case 3:
                    return Matrix3x3(
                        m00, m01, m02,
                        m20, m21, m22,
                        m30, m31, m32
                    );
                }
                break;
            case 2:
                switch (columnIndex)
                {
                case 0:
                    return Matrix3x3(
                        m01, m02, m03,
                        m11, m12, m13,
                        m31, m32, m33
                    );
                case 1:
                    return Matrix3x3(
                        m00, m02, m03,
                        m10, m12, m13,
                        m30, m32, m33
                    );
                case 2:
                    return Matrix3x3(
                        m00, m01, m03,
                        m10, m11, m13,
                        m30, m31, m33
                    );
                case 3:
                    return Matrix3x3(
                        m00, m01, m02,
                        m10, m11, m12,
                        m30, m31, m32
                    );
                }
                break;
            case 3:
                switch (columnIndex)
                {
                case 0:
                    return Matrix3x3(
                        m01, m02, m03,
                        m11, m12, m13,
                        m21, m22, m23
                    );
                case 1:
                    return Matrix3x3(
                        m00, m02, m03,
                        m10, m12, m13,
                        m20, m22, m23
                    );
                case 2:
                    return Matrix3x3(
                        m00, m01, m03,
                        m10, m11, m13,
                        m20, m21, m23
                    );
                case 3:
                    return Matrix3x3(
                        m00, m01, m02,
                        m10, m11, m12,
                        m20, m21, m22
                    );
                }
                break;
            }
            throw std::out_of_range("Index out of range");
        }

        // 余因子
        float getCofactor(int row, int column) const
        {
            Matrix3x3 subMatrix = removeRowAndColumn(row, column);
            return subMatrix.getDeterminant() * std::powf(-1.0f, (float)(row + column));// 偶数 +、奇数 -;
        }

        // 行列式
        float getDeterminant() const
        {
            float c00 = getCofactor(0, 0);
            float c01 = getCofactor(0, 1);
            float c02 = getCofactor(0, 2);
            float c03 = getCofactor(0, 3);
            return m00 * c00 + m01 * c01 + m02 * c02 + m03 * c03;
        }

        // 余因子行列
        Matrix4x4 getAdjugateMatrix() const;

        // 逆行列
        Matrix4x4 inverse() const;


        // 以下、3D 用

        static Matrix4x4 createBasis(const Vector3& xAxis, const Vector3& yAxis, const Vector3& zAxis, const Vector3& origon);

        static Matrix4x4 createRotationX(float angle);
        static Matrix4x4 createRotationY(float angle);

        static Matrix4x4 createScale(float x, float y, float z);

        static Matrix4x4 createShear(float xy, float xz, float yx, float yz, float zx, float zy);

        static Matrix4x4 lockAt(const Vector3& eye, const Vector3& center, const Vector3& up);
        static Matrix4x4 createFrustum(float left, float right, float bottom, float top, float nearVal, float farVal);
        static Matrix4x4 createProjection(float fovy, float aspect, float zNear, float zFar);
    };
}
