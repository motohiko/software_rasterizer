#pragma once

#include "..\Types.h"
#include <cstdint>

namespace SoftwareRasterizer
{
    struct VertexAttributeLayout
    {
        const void* buffer;
        SemanticsType semantics;
        int size;// 頂点属性のコンポーネントの数
        ComponentType type;// 頂点属性の各コンポーネントのデータ型
        bool normalized;// 整数値を[0,1]に正規化するか
        size_t stride;// 頂点属性間のバイトオフセット
    };

    struct InputAssemblyStageState
    {

    public:

        PrimitiveTopologyType primitiveTopologyType = PrimitiveTopologyType::kUndefined;

        VertexAttributeLayout vertexAttributeLayouts[kMaxVertexAttributes] = {};

        uint32_t vertexAttributeEnableBits = 0;

        struct IndexBuffer
        {
            const uint16_t* indices;
            int indexNum;
        };
        IndexBuffer indexBuffer;

    };
}
