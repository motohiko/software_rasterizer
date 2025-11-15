#pragma once

#include "InputAssemblyStageState.h"
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
        const InputAssemblyStageState* _inputAssemblyStageState;

        PrimitiveType _primitiveType = PrimitiveType::kNone;
        int _primitiveVertexNum = 0;

        int _readVertexCount = 0;

    public:

        static void validateState(const InputAssemblyStageState* state);

        InputAssemblyStage(RenderingContext* renderingContext);

        void prepareReadPrimitive();
        bool readPrimitive(Primitive* primitive);

        void prepareReadVertex();
        void readAttributeVertex(uint16_t vertexIndex, AttributeVertex* vertex) const;

    };
}
