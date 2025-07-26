#pragma once

#include "Common.h"
#include "Vector.h"
#include <cstdint>

namespace MyApp
{
    enum PrimitiveTopologyType
    {
        Undefined,
        LineList,
        TriangleList,
    };

    enum Semantics
    {
        Position,
        TexCoord,
        Color,
    };

    enum ComponentType
    {
        Float,// GL_FLOAT
        UnsignedByte,// GL_UNSIGNED_BYTE
    };

    class InputAssemblyStage
    {

    private:

        struct IndexBuffer
        {
            const uint16_t* indices;
            int indexNum;
        };

        struct InputVertexAttributeLayout
        {
            const void* buffer;
            Semantics semantics;
            int size;// 頂点属性のコンポーネントの数
            ComponentType type;// 頂点属性の各コンポーネントのデータ型
            bool normalized;// 整数値を[0,1]に正規化
            size_t stride;// 頂点属性間のバイトオフセット
        };

        int _primitiveVertexNum = 0;

        uint32_t _vertexAttributeEnableBits = 0;
        InputVertexAttributeLayout _vertexAttributeLayouts[kMaxVertexAttributes] = {};

        IndexBuffer _indexBuffer;

        int _readVertexCount = 0;
        int _vertexAttributeNum = 0;

    public:

        void setPrimitiveTopologyType(PrimitiveTopologyType primitiveTopologyType);

        void enableVertexAttribute(int index);// glEnableVertexAttribArray
        void disableVertexAttribute(int index);// glDIsableVertexAttribArray
        void setVertexBuffer(int index, const void* buffer);
        void setVertexAttribute(int index, Semantics semantics, int size, ComponentType type, size_t stride);// glVertexAttribPointer

        void setIndexBuffer(const uint16_t* indices, int indexNum);

        void resetReadCount();
        bool readPrimitive(AssembledPrimitive* assembledPrimitive);

    private:

        Vector4 readVertexAttribute(const InputVertexAttributeLayout* vertexAttributeLayout, uint16_t vertexIndex) const;

    };
}
