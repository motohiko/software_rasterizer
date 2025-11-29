#pragma once

#include "..\..\Lib\Vector.h"
#include "..\..\Lib\Matrix.h"
#include <cstdint>

namespace SoftwareRasterizer
{
    using Lib::Vector2;
    using Lib::Vector3;
    using Lib::Vector4;
    using Lib::Matrix4x4;

    enum class ComparisonFuncType// TODO: renmae
    {
        kNone,
        kNever,         // GL_NEVER
        kLess,          // GL_LESS
        kEqual,         // GL_EQUAL
        kLessEqual,     // GL_LEQUAL
        kGreater,       // GL_GREATER
        kNotEqual,      // GL_NOTEQUAL
        kGreaterEqual,  // GL_GEQUAL
        kAlways,        // GL_ALWAYS
        kDefault = kLess,
    };

    enum class PrimitiveTopologyType
    {
        kNone,
        kLineList,      // GL_LINES
        kTriangleList,  // GL_TRIANGLES
    };

    enum class PrimitiveType
    {
        kNone,
        kLine,
        kTriangle,
    };

    const int kMaxVertexAttributes = 16;// GL_MAX_VERTEX_ATTRIBS
    const int kMaxVaryings = 15;        // GL_MAX_VARYING_VECTORS

    struct VertexDataA// TODO: renmae
    {
        Vector4 attributes[kMaxVertexAttributes];
        uint32_t attributeEnableBits;// TODO: remove
    };

    struct VertexDataB// TODO: renmae
    {
        Vector4 clipPosition;// 頂点座標（クリップ空間座標系）
        Vector4 varyings[kMaxVaryings];
        int varyingNum;// TODO: remove
    };

    struct VertexDataC// TODO: renmae
    {
        Vector3 ndcPosition;// 頂点座標（正規化デバイス座標系）
    };

    struct VertexDataD// TODO: renmae
    {
        Vector2 wndPosition;// 頂点座標（ウィンドウ座標系）
        float depth;// 深度
        float invW;// = 1 / clipPosition.w

        Vector4 varyingsDividedByW[kMaxVaryings];
        int varyingNum;// TODO: remove
    };

    struct FragmentDataA// TODO: renmae
    {
        int x;// フラグメントの座標（ウィンドウ座標系）
        int y;

        bool pixelCovered;

        Vector2 wndPosition;// フラグメントの中心座標（ウィンドウ座標系）
        float depth;// 深度
        float invW;// = 1 / clipPosition.w

        Vector4 varyings[kMaxVaryings];
        int varyingNum;// TODO: remove

        // フラグメントシェーダーからの出力
        Vector4 color;// TODO: 別の構造体に分離
    };

    struct QuadFragmentDataA// TODO: renmae
    {

    private:

        // const はメンバのポインタ変数には伝播しない為、隠蔽しておく

        FragmentDataA* _q00;
        FragmentDataA* _q01;
        FragmentDataA* _q10;
        FragmentDataA* _q11;

    public:

        void setQ00(FragmentDataA* fragment) { _q00 = fragment; };
        void setQ01(FragmentDataA* fragment) { _q01 = fragment; };
        void setQ10(FragmentDataA* fragment) { _q10 = fragment; };
        void setQ11(FragmentDataA* fragment) { _q11 = fragment; };

        FragmentDataA* getQ00() { return _q00; };
        FragmentDataA* getQ01() { return _q01; };
        FragmentDataA* getQ10() { return _q10; };
        FragmentDataA* getQ11() { return _q11; };

        const FragmentDataA* getQ00() const { return _q00; };
        const FragmentDataA* getQ01() const { return _q01; };
        const FragmentDataA* getQ10() const { return _q10; };
        const FragmentDataA* getQ11() const { return _q11; };

    };

    struct FragmentDataB// TODO: renmae
    {
        int x;// フラグメントの座標（ウィンドウ座標系）
        int y;

        Vector4 color;  // 色
        float depth;    // 深度
    };

}
