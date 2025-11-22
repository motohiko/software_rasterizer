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

            //struct
            //{
            //    float _11;
            //};

        };

    public:

        static const Matrix1x1 kIdentity;

        Matrix1x1() = default;
        Matrix1x1(float m00);

        float getDeterminant() const;

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

            //struct
            //{
            //    float _11, _12;
            //    float _21, _22;
            //};
        };

    public:

        static const Matrix2x2 kIdentity;

        Matrix2x2() = default;
        Matrix2x2(
            float m00, float m01,
            float m10, float m11
        );

    private:

        Matrix1x1 getMatrixWithoutRowColumn(int rowIndex, int columnIndex) const;

        float getCofactor(int rowIndex, int columnIndex) const;

        float computeCofactorExpansionByFirstRow() const;

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

            //struct
            //{
            //    float _11, _12, _13;
            //    float _21, _22, _23;
            //    float _31, _32, _33;
            //};
        };

    public:

        static const Matrix3x3 kIdentity;

        Matrix3x3() = default;
        Matrix3x3(
            float m00, float m01, float m02,
            float m10, float m11, float m12,
            float m20, float m21, float m22
        );

    private:

        Matrix2x2 getMatrixWithoutRowColumn(int rowIndex, int columnIndex) const;

        float getCofactor(int rowIndex, int columnIndex) const;

        float computeCofactorExpansionByFirstRow() const;

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

            //struct
            //{
            //    float _11;
            //    float _21;
            //    float _31;
            //    float _41;
            //};
        };

    public:

        Matrix4x1() = default;
        Matrix4x1(
            float m00,
            float m10,
            float m20,
            float m30
        );

        Vector4 getColumn(int columnIndex) const;

    };

    // 1x4行列
    //
    // m = | m00 m01 m02 m03 |
    //
    struct Matrix1x4
    {

    private:

        union
        {
            struct
            {
                float m00, m01, m02, m03;
            };

            //struct
            //{
            //    float _11, _12, _13, _14;
            //};
        };

    public:

        Matrix1x4() = default;
        Matrix1x4(
            float m00, float m01, float m02, float m03
        );

        Vector4 getRow(int rowIndex) const;

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

            //struct
            //{
            //    float _11, _12, _13, _14;
            //    float _21, _22, _23, _24;
            //    float _31, _32, _33, _34;
            //    float _41, _42, _43, _44;
            //};
        };

    public:

        static const Matrix4x4 kIdentity;

        Matrix4x4() = default;
        Matrix4x4(
            float m00, float m01, float m02, float m03,
            float m10, float m11, float m12, float m13,
            float m20, float m21, float m22, float m23,
            float m30, float m31, float m32, float m33
        );

        Vector4 getRow(int rowIndex) const;
        Vector4 getColumn(int columnIndex) const;

        Matrix4x4 getTransposeMatrix() const;

    private:

        Matrix3x3 getMatrixWithoutRowColumn(int rowIndex, int columnIndex) const;

        float getCofactor(int rowIndex, int columnIndex) const;
        Matrix4x4 getCofactorMatrix() const;
        Matrix4x4 getAdjugateMatrix() const;

        float computeCofactorExpansionByFirstRow() const;

    public:

        float getDeterminant() const;

        Matrix4x4 getInverseMatrix() const;

        // スカラー倍
        static Matrix4x4 ScaleByScalar(const Matrix4x4& lhs, float rhs);

        // 行列積
        static Matrix4x4 ComputeMatrixProduct(const Matrix4x4& lhs, const Matrix4x4& rhs);
        static Matrix4x1 ComputeMatrixProduct(const Matrix4x4& lhs, const Matrix4x1& rhs);
        static Matrix1x4 ComputeMatrixProduct(const Matrix1x4& lhs, const Matrix4x4& rhs);

        // 変換
        static Vector4 ApplyMatrixToColumnVector(const Matrix4x4& lhs, const Vector4& rhs);
        static Vector4 ApplyMatrixToRowVector(const Vector4& lhs, const Matrix4x4& rhs);

    public:

        // スカラー倍
        Matrix4x4 operator*(float rhs) const { return ScaleByScalar(*this, rhs); }
        Matrix4x4 operator/(float rhs) const { return ScaleByScalar(*this, 1.0f / rhs); }

        // 行列積
        Matrix4x4 operator*(const Matrix4x4& rhs) const { return ComputeMatrixProduct(*this, rhs); }
        Matrix4x1 operator*(const Matrix4x1& rhs) const { return ComputeMatrixProduct(*this, rhs); }

        // ベクトル変換
        Vector4 operator*(const Vector4& rhs) const { return ApplyMatrixToColumnVector(*this, rhs); }

    };
}
