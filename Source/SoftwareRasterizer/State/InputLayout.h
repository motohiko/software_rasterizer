#pragma once

#include "..\Core\Types.h"
#include <cstdint>

namespace SoftwareRasterizer
{
    //enum class SemanticsType
    //{
    //    kNone,
    //    kPosition,
    //    kNormal,
    //    kTexCoord,
    //    kColor,
    //};

    enum class ComponentType
    {
        kNone,
        kFloat,         // GL_FLOAT
        kUnsignedByte,  // GL_UNSIGNED_BYTE
    };

    struct InputElement
    {
        int size = 0;                                   // 頂点属性のコンポーネントの数
        ComponentType type = ComponentType::kNone;      // 頂点属性の各コンポーネントのデータ型
        bool normalized = false;                        // 整数値を[0,1]に正規化するか
        size_t stride = 0;                              // 頂点属性間のバイトオフセット
    };

    struct InputLayout
    {
        InputElement elements[kMaxVertexAttributes] = {};

        uint32_t vertexAttributeEnableBits = 0;// TODO:
    };
}
