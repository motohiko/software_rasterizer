#pragma once

#include "InputAssemblyStageState.h"
#include "..\Types.h"// PrimitiveType, kMaxVertexAttributes
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

        const InputAssemblyStageState* _inputAssemblyStageState;

        PrimitiveType _primitiveType = PrimitiveType::kUndefined;
        int _primitiveVertexNum = 0;

        int _vertexAttributeNum = 0;
        int _readVertexCount = 0;

    public:

        InputAssemblyStage(const InputAssemblyStageState* state);

        void validateState();

        void prepareReadPrimitive();
        bool readPrimitive(Primitive* primitive);

        void prepareReadVertex();
        void readAttributeVertex(uint16_t vertexIndex, AttributeVertex* vertex) const;
    };
}
