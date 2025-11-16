#pragma once

#include "..\State\InputLayout.h"
#include "..\State\VertexBuffers.h"
#include "..\State\IndexBuffer.h"
#include "..\Types.h"
#include <cstdint>

namespace SoftwareRasterizer
{
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

        const InputLayout* _inputLayout = nullptr;
        const VertexBuffers* _vertexBuffers = nullptr;
        const IndexBuffer* _indexBuffer = nullptr;
        PrimitiveTopologyType _primitiveTopologyType = PrimitiveTopologyType::kNone;

        PrimitiveType _primitiveType = PrimitiveType::kNone;
        int _primitiveVertexNum = 0;

        int _readVertexCount = 0;

    public:

        static void validateState(const InputLayout* state);

        InputAssemblyStage();

        void input(const InputLayout* inputLayout) { _inputLayout = inputLayout;  }
        void input(const VertexBuffers* vertexBuffers) { _vertexBuffers = vertexBuffers; }
        void input(const IndexBuffer* indexBuffer) { _indexBuffer = indexBuffer; }
        void input(PrimitiveTopologyType primitiveTopologyType) { _primitiveTopologyType = primitiveTopologyType; }
        
        void prepareReadPrimitive();
        bool readPrimitive(Primitive* primitive);

        void readAttributeVertex(uint16_t vertexIndex, AttributeVertex* vertex) const;

    };
}
