#pragma once

#include "Vector.h"

namespace Lib
{
    // 1x1行列
    //
    // m = | m00 |
    //
    struct Matrix1x1
    {
    private:

        union
        {
            struct
            {
                float m00;
            };

            // TODO:
        };

    public:

        Matrix1x1() = default;
        Matrix1x1(float m00) : m00(m00) {};

        float getDeterminant() const { return m00; }

    };

    // 2x2行列
    //
    // m = | m00 m01 |
    //     | m10 m11 |
    //
    struct Matrix2x2
    {

    private:

        union
        {
            struct
            {
                float m00, m01;
                float m10, m11;
            };

            // TODO:
        };

    public:

        Matrix2x2() = default;
        Matrix2x2(
            float m00, float m01,
            float m10, float m11
        );

        static const Matrix2x2 kIdentity;

    private:

        Matrix1x1 getMatrixWithoutRowColumn(int rowIndex, int columnIndex) const;
        Matrix1x1 getSubMatrix(int rowIndex, int columnIndex) const;
        float getCofactor(int rowIndex, int columnIndex) const;

    public:

        float getDeterminant() const;

    };

    // 3x3行列
    //
    //     | m00 m01 m02 |
    // m = | m10 m11 m12 |
    //     | m20 m21 m22 |
    //
    struct Matrix3x3
    {

    private:

        union
        {
            struct
            {
                float m00, m01, m02;
                float m10, m11, m12;
                float m20, m21, m22;
            };

            // TODO:
        };

    public:

        Matrix3x3() = default;
        Matrix3x3(
            float m00, float m01, float m02,
            float m10, float m11, float m12,
            float m20, float m21, float m22
        );

        static const Matrix3x3 kIdentity;

    private:

        Matrix2x2 getMatrixWithoutRowColumn(int rowIndex, int columnIndex) const;
        Matrix2x2 getSubMatrix(int rowIndex, int columnIndex) const;
        float getCofactor(int rowIndex, int columnIndex) const;

    public:

        float getDeterminant() const;

    };

    // 4x1行列
    //
    //     | m00 |
    // m = | m10 |
    //     | m20 |
    //     | m30 |
    //
    struct Matrix4x1
    {

    private:

        union
        {
            struct
            {
                float m00;
                float m10;
                float m20;
                float m30;
            };

            // TODO:
        };

    public:

        Matrix4x1() = default;
        Matrix4x1(
            float m00,
            float m10,
            float m20,
            float m30
        );

        Vector4 getColumn(int column) const;

        static const Matrix4x1& FromVector4(const Vector4& v)
        {
            return *reinterpret_cast<const Matrix4x1*>(&v);
        }

        static const Vector4& AsVector4(const Matrix4x1& m)
        {
            return *reinterpret_cast<const Vector4*>(&m);
        }

    };

    // 4x4行列
    //
    //     | m00 m01 m02 m03 |
    // m = | m10 m11 m12 m13 |
    //     | m20 m21 m22 m23 |
    //     | m30 m31 m32 m33 |
    //
    struct Matrix4x4
    {

    private:

        union
        {
            struct
            {
                float m00, m01, m02, m03;
                float m10, m11, m12, m13;
                float m20, m21, m22, m23;
                float m30, m31, m32, m33;
            };

            // TODO:
        };

    public:

        Matrix4x4() = default;
        Matrix4x4(
            float m00, float m01, float m02, float m03,
            float m10, float m11, float m12, float m13,
            float m20, float m21, float m22, float m23,
            float m30, float m31, float m32, float m33
        );

        static const Matrix4x4 kIdentity;

        Vector4 getRow(int rowIndex) const;
        Vector4 getColumn(int columnIndex) const;

    private:

        Matrix3x3 getMatrixWithoutRowColumn(int rowIndex, int columnIndex) const;
        Matrix3x3 getSubMatrix(int rowIndex, int columnIndex) const;
        float getCofactor(int rowIndex, int columnIndex) const;
        Matrix4x4 getAdjugateMatrix() const;

    public:

        float getDeterminant() const;

        Matrix4x4 getTransposeMatrix() const;
        Matrix4x4 getInverseMatrix() const;

        // スカラー積
        static Matrix4x4 ScalarProduct(const Matrix4x4& lhs, float rhs);

        // 行列積
        static Matrix4x4 Multiply(const Matrix4x4& lhs, const Matrix4x4& rhs);
        static Matrix4x1 Multiply(const Matrix4x4& lhs, const Matrix4x1& rhs);
        static Vector4 Multiply(const Matrix4x4& lhs, const Vector4& rhs);

        // スカラー積
        Matrix4x4 operator*(float rhs) const { return ScalarProduct(*this, rhs); }
        Matrix4x4 operator/(float rhs) const { return ScalarProduct(*this, 1.0f / rhs); }

        // 行列積
        Matrix4x4 operator*(const Matrix4x4& rhs) const { return Multiply(*this, rhs); }
        Matrix4x1 operator*(const Matrix4x1& rhs) const { return Multiply(*this, rhs); }
        Vector4 operator*(const Vector4& rhs) const { return Multiply(*this, rhs); }

    };
}
