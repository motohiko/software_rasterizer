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


    // Pipeline Data

    const int kMaxVertexAttributes = 16;// GL_MAX_VERTEX_ATTRIBS
    const int kMaxVaryings = 15;        // GL_MAX_VARYING_VECTORS

    struct VertexDataA// TODO: renmae
    {
        Vector4 attributes[kMaxVertexAttributes];
    };

    struct VertexDataB// TODO: renmae
    {
        Vector4 clipCoord;// 頂点座標（クリップ空間）
        Vector4 varyings[kMaxVaryings];
    };

    struct VertexDataC// TODO: renmae
    {
        //Vector3 ndCoord;
        Vector3 ndcPosition;// 頂点座標（正規化デバイス空間）
    };

    struct VertexDataD// TODO: renmae
    {
        Vector2 wndCoord;// 頂点座標（ウィンドウ空間）
        float depth;
        float invW;// = 1 / clipCoord.w
        Vector4 varyingsDividedByW[kMaxVaryings];
    };

    struct FragmentData
    {
        int x;// フラグメント座標（ウィンドウ空間）
        int y;

        bool pixelCovered;

        Vector2 wndCoord;// ピクセルの中心座標（ウィンドウ空間）
        float depth;
        float invW;// = 1 / clipCoord.w
        Vector4 varyings[kMaxVaryings];
    };

    struct QuadFragmentData
    {
        FragmentData q00;
        FragmentData q01;
        FragmentData q10;
        FragmentData q11;
    };

    struct PixelData
    {
        Vector4 color;
        float depth;
    };

    struct QuadPixelData
    {
        PixelData q00;
        PixelData q01;
        PixelData q10;
        PixelData q11;
    };

}
