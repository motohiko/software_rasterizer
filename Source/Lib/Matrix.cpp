#include "Matrix.h"
#include <cmath>

namespace Lib
{

    Matrix1x1::Matrix1x1(
        float m00
    ) : 
        m00(m00)
    {
    };

    const Matrix1x1 Matrix1x1::kIdentity(
        1.0f
    );

    float Matrix1x1::getDeterminant() const
    {
        return m00;
    }

    Matrix2x2::Matrix2x2(
        float m00, float m01,
        float m10, float m11
    ) :
        m00(m00), m01(m01),
        m10(m10), m11(m11)
    {
    }

    const Matrix2x2 Matrix2x2::kIdentity(
        1.0f, 0.0f,
        0.0f, 1.0f
    );

    // 小行列
    Matrix1x1 Matrix2x2::getMatrixWithoutRowColumn(int rowIndex, int columnIndex) const
    {
        switch (rowIndex)
        {
        case 0:
            switch (columnIndex)
            {
            case 0:
                return Matrix1x1(
                    /**/ /**/
                    /**/ m11
                );
            case 1:
                return Matrix1x1(
                    /**/ /**/
                    m10  /**/
                );
            }
            break;
        case 1:
            switch (columnIndex)
            {
            case 0:
                return Matrix1x1(
                    /**/ m01
                    /**/ /**/
                );
            case 1:
                return Matrix1x1(
                    m00  /**/
                    /**/ /**/
                );
            }
            break;
        }

        return Matrix1x1::kIdentity;
    }

    // 余因子
    float Matrix2x2::getCofactorAt(int rowIndex, int columnIndex) const
    {
        Matrix1x1 subMatrix = getMatrixWithoutRowColumn(rowIndex, columnIndex);
        float det = subMatrix.getDeterminant();
        float sign = std::pow(-1.0f, (float)((1 + rowIndex) + (1 + columnIndex)));// 偶数 +、奇数 -
        return det * sign;
    }

    // 余因子展開
    float  Matrix2x2::computeCofactorExpansionByFirstRow() const
    {
        float c00 = getCofactorAt(0, 0);
        float c01 = getCofactorAt(0, 1);
        return (m00 * c00) + (m01 * c01);
    }

    float Matrix2x2::getDeterminant() const
    {
        return computeCofactorExpansionByFirstRow();
    }

    Matrix3x3::Matrix3x3(
        float m00, float m01, float m02,
        float m10, float m11, float m12,
        float m20, float m21, float m22
    ) :
        m00(m00), m01(m01), m02(m02),
        m10(m10), m11(m11), m12(m12),
        m20(m20), m21(m21), m22(m22)
    {
    }

    const Matrix3x3 Matrix3x3::kIdentity(
        1.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 1.0f
    );

    // 小行列
    Matrix2x2 Matrix3x3::getMatrixWithoutRowColumn(int rowIndex, int columnIndex) const
    {
        switch (rowIndex)
        {
        case 0:
            switch (columnIndex)
            {
            case 0:
                return Matrix2x2(
                    /**/ /**/ /**/
                    /**/ m11, m12,
                    /**/ m21, m22
                );
            case 1:
                return Matrix2x2(
                    /**/ /**/ /**/
                    m10, /**/ m12,
                    m20, /**/ m22
                );
            case 2:
                return Matrix2x2(
                    /**/ /**/ /**/
                    m10, m11, /**/
                    m20, m21  /**/
                );
            }
            break;
        case 1:
            switch (columnIndex)
            {
            case 0:
                return Matrix2x2(
                    /**/ m01, m02,
                    /**/ /**/ /**/
                    /**/ m21, m22
                );
            case 1:
                return Matrix2x2(
                    m00, /**/ m02,
                    /**/ /**/ /**/
                    m20, /**/ m22
                );
            case 2:
                return Matrix2x2(
                    m00, m01, /**/
                    /**/ /**/ /**/
                    m20, m21  /**/
                );
            }
            break;
        case 2:
            switch (columnIndex)
            {
            case 0:
                return Matrix2x2(
                    /**/ m01, m02,
                    /**/ m11, m12
                    /**/ /**/ /**/
                );
            case 1:
                return Matrix2x2(
                    m00, /**/ m02,
                    m10, /**/ m12
                    /**/ /**/ /**/
                );
            case 2:
                return Matrix2x2(
                    m00, m01, /**/
                    m10, m11  /**/
                    /**/ /**/ /**/
                );
            }
            break;
        }

        return Matrix2x2::kIdentity;
    }

    // 余因子
    float Matrix3x3::getCofactorAt(int rowIndex, int columnIndex) const
    {
        Matrix2x2 subMatrix = getMatrixWithoutRowColumn(rowIndex, columnIndex);
        float det = subMatrix.getDeterminant();
        float sign = std::pow(-1.0f, (float)((1 + rowIndex) + (1 + columnIndex)));// 偶数 +、奇数 -
        return det * sign;
    }

    // 余因子展開
    float Matrix3x3::computeCofactorExpansionByFirstRow() const
    {
        float c00 = getCofactorAt(0, 0);
        float c01 = getCofactorAt(0, 1);
        float c02 = getCofactorAt(0, 2);
        return (m00 * c00) + (m01 * c01) + (m02 * c02);
    }

    float Matrix3x3::getDeterminant() const
    {
        return computeCofactorExpansionByFirstRow();
    }

    Matrix4x1::Matrix4x1(
        float m00,
        float m10,
        float m20,
        float m30
    ) :
        m00(m00),
        m10(m10),
        m20(m20),
        m30(m30)
    {
    }

    Vector4 Matrix4x1::getColumn(int columnIndex) const
    {
        switch (columnIndex)
        {
        case 0:
            return Vector4(m00, m10, m20, m30);
        default:
            return Vector4::kZero;
        }
    }

    Matrix4x4::Matrix4x4(
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

    const Matrix4x4 Matrix4x4::kIdentity(
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    );

    Vector4 Matrix4x4::getRow(int rowIndex) const
    {
        switch (rowIndex)
        {
        case 0:
            return Vector4(m00, m01, m02, m03);
        case 1:
            return Vector4(m10, m11, m12, m13);
        case 2:
            return Vector4(m20, m21, m22, m23);
        case 3:
            return Vector4(m30, m31, m32, m33);
        default:
            return Vector4::kZero;
        }
    }

    Vector4 Matrix4x4::getColumn(int columnIndex) const
    {
        switch (columnIndex)
        {
        case 0:
            return Vector4(m00, m10, m20, m30);
        case 1:
            return Vector4(m01, m11, m21, m31);
        case 2:
            return Vector4(m02, m12, m22, m32);
        case 3:
            return Vector4(m03, m13, m23, m33);
        default:
            return Vector4::kZero;
        }
    }

    Matrix4x4 Matrix4x4::getTransposeMatrix() const
    {
        return Matrix4x4(
            m00, m10, m20, m30,
            m01, m11, m21, m31,
            m02, m12, m22, m32,
            m03, m13, m23, m33
        );
    }

    // 小行列
    Matrix3x3 Matrix4x4::getMatrixWithoutRowColumn(int rowIndex, int columnIndex) const
    {
        switch (rowIndex)
        {
        case 0:
            switch (columnIndex)
            {
            case 0:
                return Matrix3x3(
                    /**/ /**/ /**/ /**/
                    /**/ m11, m12, m13,
                    /**/ m21, m22, m23,
                    /**/ m31, m32, m33
                );
            case 1:
                return Matrix3x3(
                    /**/ /**/ /**/ /**/
                    m10, /**/ m12, m13,
                    m20, /**/ m22, m23,
                    m30, /**/ m32, m33
                );
            case 2:
                return Matrix3x3(
                    /**/ /**/ /**/ /**/
                    m10, m11, /**/ m13,
                    m20, m21, /**/ m23,
                    m30, m31, /**/ m33
                );
            case 3:
                return Matrix3x3(
                    /**/ /**/ /**/ /**/
                    m10, m11, m12, /**/
                    m20, m21, m22, /**/
                    m30, m31, m32  /**/
                );
            }
            break;
        case 1:
            switch (columnIndex)
            {
            case 0:
                return Matrix3x3(
                    /**/ m01, m02, m03,
                    /**/ /**/ /**/ /**/
                    /**/ m21, m22, m23,
                    /**/ m31, m32, m33
                );
            case 1:
                return Matrix3x3(
                    m00, /**/ m02, m03,
                    /**/ /**/ /**/ /**/
                    m20, /**/ m22, m23,
                    m30, /**/ m32, m33
                );
            case 2:
                return Matrix3x3(
                    m00, m01, /**/ m03,
                    /**/ /**/ /**/ /**/
                    m20, m21, /**/ m23,
                    m30, m31, /**/ m33
                );
            case 3:
                return Matrix3x3(
                    m00, m01, m02, /**/
                    /**/ /**/ /**/ /**/
                    m20, m21, m22, /**/
                    m30, m31, m32  /**/
                );
            }
            break;
        case 2:
            switch (columnIndex)
            {
            case 0:
                return Matrix3x3(
                    /**/ m01, m02, m03,
                    /**/ m11, m12, m13,
                    /**/ /**/ /**/ /**/
                    /**/ m31, m32, m33
                );
            case 1:
                return Matrix3x3(
                    m00, /**/ m02, m03,
                    m10, /**/ m12, m13,
                    /**/ /**/ /**/ /**/
                    m30, /**/ m32, m33
                );
            case 2:
                return Matrix3x3(
                    m00, m01, /**/ m03,
                    m10, m11, /**/ m13,
                    /**/ /**/ /**/ /**/
                    m30, m31, /**/ m33
                );
            case 3:
                return Matrix3x3(
                    m00, m01, m02, /**/
                    m10, m11, m12, /**/
                    /**/ /**/ /**/ /**/
                    m30, m31, m32  /**/
                );
            }
            break;
        case 3:
            switch (columnIndex)
            {
            case 0:
                return Matrix3x3(
                    /**/ m01, m02, m03,
                    /**/ m11, m12, m13,
                    /**/ m21, m22, m23
                    /**/ /**/ /**/ /**/
                );
            case 1:
                return Matrix3x3(
                    m00, /**/ m02, m03,
                    m10, /**/ m12, m13,
                    m20, /**/ m22, m23
                    /**/ /**/ /**/ /**/
                );
            case 2:
                return Matrix3x3(
                    m00, m01, /**/ m03,
                    m10, m11, /**/ m13,
                    m20, m21, /**/ m23
                    /**/ /**/ /**/ /**/
                );
            case 3:
                return Matrix3x3(
                    m00, m01, m02, /**/
                    m10, m11, m12, /**/
                    m20, m21, m22  /**/
                    /**/ /**/ /**/ /**/
                );
            }
            break;
        }
        return Matrix3x3::kIdentity;
    }

    // 余因子
    float Matrix4x4::getCofactorAt(int rowIndex, int columnIndex) const
    {
        Matrix3x3 subMatrix = getMatrixWithoutRowColumn(rowIndex, columnIndex);
        float det = subMatrix.getDeterminant();
        float sign = std::pow(-1.0f, (float)((1 + rowIndex) + (1 + columnIndex)));// 偶数 +、奇数 -
        return det * sign;
    }

    // 余因子行列
    Matrix4x4 Matrix4x4::getAdjugateMatrix() const
    {
        float c00 = getCofactorAt(0, 0);
        float c01 = getCofactorAt(0, 1);
        float c02 = getCofactorAt(0, 2);
        float c03 = getCofactorAt(0, 3);
        float c10 = getCofactorAt(1, 0);
        float c11 = getCofactorAt(1, 1);
        float c12 = getCofactorAt(1, 2);
        float c13 = getCofactorAt(1, 3);
        float c20 = getCofactorAt(2, 0);
        float c21 = getCofactorAt(2, 1);
        float c22 = getCofactorAt(2, 2);
        float c23 = getCofactorAt(2, 3);
        float c30 = getCofactorAt(3, 0);
        float c31 = getCofactorAt(3, 1);
        float c32 = getCofactorAt(3, 2);
        float c33 = getCofactorAt(3, 3);

        Matrix4x4 cofactorMatrix(
            c00, c01, c02, c03,
            c10, c11, c12, c13,
            c20, c21, c22, c23,
            c30, c31, c32, c33
        );

        return cofactorMatrix.getTransposeMatrix();
    }

    // 余因子展開
    float Matrix4x4::computeCofactorExpansionByFirstRow() const
    {
        float c00 = getCofactorAt(0, 0);
        float c01 = getCofactorAt(0, 1);
        float c02 = getCofactorAt(0, 2);
        float c03 = getCofactorAt(0, 3);
        return (m00 * c00) + (m01 * c01) + (m02 * c02) + (m03 * c03);
    }

    float Matrix4x4::getDeterminant() const
    {
        return computeCofactorExpansionByFirstRow();
    }

    Matrix4x4 Matrix4x4::getInverseMatrix() const
    {
        Matrix4x4 adj = getAdjugateMatrix();

        float det = getDeterminant();
        if (det == 0.0f)
        {
            return Matrix4x4::kIdentity;
        }

        return ScaleByScalar(adj, 1.0f / det);
    }

    // スカラー積
    Matrix4x4 Matrix4x4::ScaleByScalar(const Matrix4x4& lhs, float rhs)
    {
        return Matrix4x4(
            lhs.m00 * rhs, lhs.m01 * rhs, lhs.m02 * rhs, lhs.m03 * rhs,
            lhs.m10 * rhs, lhs.m11 * rhs, lhs.m12 * rhs, lhs.m13 * rhs,
            lhs.m20 * rhs, lhs.m21 * rhs, lhs.m22 * rhs, lhs.m23 * rhs,
            lhs.m30 * rhs, lhs.m31 * rhs, lhs.m32 * rhs, lhs.m33 * rhs
        );
    }

    // 行列積
    Matrix4x4 Matrix4x4::ComputeMatrixProduct(const Matrix4x4& lhs, const Matrix4x4& rhs)
    {
        Vector4 r0 = lhs.getRow(0);
        Vector4 r1 = lhs.getRow(1);
        Vector4 r2 = lhs.getRow(2);
        Vector4 r3 = lhs.getRow(3);

        Vector4 c0 = rhs.getColumn(0);
        Vector4 c1 = rhs.getColumn(1);
        Vector4 c2 = rhs.getColumn(2);
        Vector4 c3 = rhs.getColumn(3);

        return  Matrix4x4(
            r0.dot(c0), r0.dot(c1), r0.dot(c2), r0.dot(c3),
            r1.dot(c0), r1.dot(c1), r1.dot(c2), r1.dot(c3),
            r2.dot(c0), r2.dot(c1), r2.dot(c2), r2.dot(c3),
            r3.dot(c0), r3.dot(c1), r3.dot(c2), r3.dot(c3)
        );
    }

    Matrix4x1 Matrix4x4::ComputeMatrixProduct(const Matrix4x4& lhs, const Matrix4x1& rhs)
    {
        Vector4 r0 = lhs.getRow(0);
        Vector4 r1 = lhs.getRow(1);
        Vector4 r2 = lhs.getRow(2);
        Vector4 r3 = lhs.getRow(3);

        Vector4 c0 = rhs.getColumn(0);

        return  Matrix4x1(
            r0.dot(c0),
            r1.dot(c0),
            r2.dot(c0),
            r3.dot(c0)
        );
    }

    Vector4 Matrix4x4::ApplyMatrixToColumnVector(const Matrix4x4& lhs, const Vector4& rhs)
    {
        Matrix4x1 ret = ComputeMatrixProduct(lhs, (Matrix4x1::FromVector4(rhs)));
        return Matrix4x1::AsVector4(ret);
    }

}
