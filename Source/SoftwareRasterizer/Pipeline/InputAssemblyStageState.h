#pragma once

#include "..\Types.h"// kMaxVertexAttributes
#include <cstdint>

namespace SoftwareRasterizer
{
    enum class PrimitiveTopologyType
    {
        kUndefined,
        kLineList,
        kTriangleList,
    };

    enum class Semantics
    {
        kPosition,
        kNormal,
        kTexCoord,
        kColor,
    };

    enum class ComponentType
    {
        kFloat,// GL_FLOAT
        kUnsignedByte,// GL_UNSIGNED_BYTE
    };

    struct VertexAttributeLayout
    {
        const void* buffer;
        Semantics semantics;
        int size;// 頂点属性のコンポーネントの数
        ComponentType type;// 頂点属性の各コンポーネントのデータ型
        bool normalized;// 整数値を[0,1]に正規化するか
        size_t stride;// 頂点属性間のバイトオフセット
    };

    struct IndexBuffer
    {
        const uint16_t* indices;
        int indexNum;
    };

    struct InputAssemblyStageState
    {

    public:

        PrimitiveTopologyType primitiveTopologyType = PrimitiveTopologyType::kUndefined;

        uint32_t vertexAttributeEnableBits = 0;
        VertexAttributeLayout vertexAttributeLayouts[kMaxVertexAttributes] = {};

        IndexBuffer indexBuffer;
    };
}
