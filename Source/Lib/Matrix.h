#pragma once

#include "Vector.h"

//namespace Lib
//{
    // m = | m00 m01 |
    //     | m10 m11 |
     struct Matrix2x2
    {
        float m00, m01;
        float m10, m11;

        static const Matrix2x2 kIdentity;

        Matrix2x2() = default;

        Matrix2x2(
            float m00, float m01,
            float m10, float m11
        );

        // 特定の行および列を取り除いた小行列
        float removeRowAndColumn(int rowIndex, int columnIndex) const;

        // 余因子
        float getCofactor(int row, int column) const;

        // 行列式
        float getDeterminant() const;
    };

    //     | m00 m01 m02 |
    // m = | m10 m11 m12 |
    //     | m20 m21 m22 |
    struct Matrix3x3
    {
        float m00, m01, m02;
        float m10, m11, m12;
        float m20, m21, m22;

        static const Matrix3x3 kIdentity;

        Matrix3x3() = default;

        Matrix3x3(
            float m00, float m01, float m02,
            float m10, float m11, float m12,
            float m20, float m21, float m22
        );

        // 特定の行および列を取り除いた小行列
        Matrix2x2 removeRowAndColumn(int rowIndex, int columnIndex) const;

        // 余因子
        float getCofactor(int row, int column) const;

        // 行列式
        float getDeterminant() const;
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
        );

        void setRow(int index, const Vector4& row);
        Vector4 getRow(int index) const;

        void setColumn(int index, const Vector4& column);

        // 転置
        Matrix4x4 transpose() const;

        // 特定の行および列を取り除いた小行列
        Matrix3x3 removeRowAndColumn(int rowIndex, int columnIndex) const;

        // 余因子
        float getCofactor(int row, int column) const;

        // 行列式
        float getDeterminant() const;

        // 余因子行列
        Matrix4x4 getAdjugateMatrix() const;

        // 逆行列
        Matrix4x4 inverse() const;

        static Matrix4x4 createBasis(const Vector3& xAxis, const Vector3& yAxis, const Vector3& zAxis, const Vector3& origon);
        static Matrix4x4 createRotationX(float angle);
        static Matrix4x4 createRotationY(float angle);
        static Matrix4x4 createScale(float x, float y, float z, float w);
        static Matrix4x4 createShear(float xy, float xz, float yx, float yz, float zx, float zy);

        Matrix4x4 operator*(const Matrix4x4& r) const;

        friend Vector4 operator*(const Matrix4x4& mat, const Vector4& vec);
    };
//}
