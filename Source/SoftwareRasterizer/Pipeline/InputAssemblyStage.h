#pragma once

#include "..\State\InputLayout.h"
#include "..\State\VertexBuffers.h"
#include "..\State\IndexBuffer.h"
#include "..\Types.h"
#include <cstdint>

namespace SoftwareRasterizer
{
    class RenderingContext;

    class InputAssemblyStage
    {

    public:

        struct Primitive
        {
            PrimitiveType primitiveType;
            uint16_t vertexIndices[3];
            int vertexNum;
        };

    private:

        RenderingContext* _renderingContext;
        const InputLayout* _inputLayout;
        const VertexBuffers* _vertexBuffers;
        const IndexBuffer* _indexBuffer;

        PrimitiveTopologyType _primitiveTopologyType = PrimitiveTopologyType::kNone;

        PrimitiveType _primitiveType = PrimitiveType::kNone;
        int _primitiveVertexNum = 0;

        int _readVertexCount = 0;

    public:

        static void validateState(const InputLayout* state);

        InputAssemblyStage(RenderingContext* renderingContext);

        void prepareReadPrimitive(PrimitiveTopologyType primitiveTopologyType);
        bool readPrimitive(Primitive* primitive);

        void prepareReadVertex();
        void readAttributeVertex(uint16_t vertexIndex, AttributeVertex* vertex) const;

    };
}
