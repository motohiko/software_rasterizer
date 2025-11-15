#include "InputAssemblyStage.h"
#include "..\RenderingContext.h"
#include "..\..\Lib\Algorithm.h"
#include "..\..\Lib\Vector.h"
#include <cassert>

namespace SoftwareRasterizer
{
    void InputAssemblyStage::validateState(const InputAssemblyStageState* state)
    {
    }

    InputAssemblyStage::InputAssemblyStage(RenderingContext* renderingContext) :
        _renderingContext(renderingContext),
        _inputAssemblyStageState(&(renderingContext->_inputAssemblyStageState))
    {
    }

    void InputAssemblyStage::prepareReadPrimitive()
    {
        switch (_inputAssemblyStageState->primitiveTopologyType)
        {
        case PrimitiveTopologyType::kLineList:
            _primitiveType = PrimitiveType::kLine;
            _primitiveVertexNum = 2;
            break;
        case PrimitiveTopologyType::kTriangleList:
            _primitiveType = PrimitiveType::kTriangle;
            _primitiveVertexNum = 3;
            break;
        default:
            _primitiveType = PrimitiveType::kNone;
            _primitiveVertexNum = 0;
            break;
        }

        _readVertexCount = 0;
    }

    bool InputAssemblyStage::readPrimitive(InputAssemblyStage::Primitive* primitive)
    {
        int remainingVertexCount = _inputAssemblyStageState->indexBuffer.indexNum - _readVertexCount;
        if (remainingVertexCount < _primitiveVertexNum)
        {
            primitive->primitiveType = PrimitiveType::kNone;
            primitive->vertexNum = 0;
            return false;
        }

        for (int i = 0; i < _primitiveVertexNum; i++)
        {
            uint16_t vertexIndex = _inputAssemblyStageState->indexBuffer.indices[_readVertexCount];
            _readVertexCount++;

            primitive->vertexIndices[i] = vertexIndex;
        }

        primitive->primitiveType = _primitiveType;
        primitive->vertexNum = _primitiveVertexNum;

        return true;
    }

    void InputAssemblyStage::prepareReadVertex()
    {
    }

    void InputAssemblyStage::readAttributeVertex(uint16_t vertexIndex, AttributeVertex* vertex) const
    {
        for (int i = 0; i < kMaxVertexAttributes; i++)
        {
            if (_inputAssemblyStageState->vertexAttributeEnableBits & (1u << i))
            {
                const VertexAttributeLayout* vertexAttributeLayout = &(_inputAssemblyStageState->vertexAttributeLayouts[i]);

                Vector4 attribute;
                switch (vertexAttributeLayout->semantics)
                {
                case SemanticsType::kPosition:
                    attribute = Vector4(0.0f, 0.0f, 0.0f, 1.0f);
                    break;
                default:
                    attribute = Vector4(0.0f, 0.0f, 0.0f, 0.0f);
                    break;
                }

                // 指定個数のコンポーネントを読み取る
                uintptr_t ptr = ((uintptr_t)vertexAttributeLayout->buffer) + (vertexAttributeLayout->stride * vertexIndex);
                switch (vertexAttributeLayout->type)
                {
                case ComponentType::kFloat:
                    switch (vertexAttributeLayout->size)
                    {
                    case 4:
                        attribute.w = ((const float*)ptr)[3];
                        [[fallthrough]];
                    case 3:
                        attribute.z = ((const float*)ptr)[2];
                        [[fallthrough]];
                    case 2:
                        attribute.y = ((const float*)ptr)[1];
                        [[fallthrough]];
                    case 1:
                        attribute.x = ((const float*)ptr)[0];
                        [[fallthrough]];
                    default:
                        break;
                    }
                    break;
                case ComponentType::kUnsignedByte:
                    if (vertexAttributeLayout->normalized)
                    {
                        switch (vertexAttributeLayout->size)
                        {
                        case 4:
                            attribute.w = Lib::NormalizeByte(((const uint8_t*)ptr)[3]);
                            [[fallthrough]];
                        case 3:
                            attribute.z = Lib::NormalizeByte(((const uint8_t*)ptr)[2]);
                            [[fallthrough]];
                        case 2:
                            attribute.y = Lib::NormalizeByte(((const uint8_t*)ptr)[1]);
                            [[fallthrough]];
                        case 1:
                            attribute.x = Lib::NormalizeByte(((const uint8_t*)ptr)[0]);
                            [[fallthrough]];
                        default:
                            break;
                        }
                    }
                    else
                    {
                        switch (vertexAttributeLayout->size)
                        {
                        case 4:
                            attribute.w = ((const uint8_t*)ptr)[3];
                            [[fallthrough]];
                        case 3:
                            attribute.z = ((const uint8_t*)ptr)[2];
                            [[fallthrough]];
                        case 2:
                            attribute.y = ((const uint8_t*)ptr)[1];
                            [[fallthrough]];
                        case 1:
                            attribute.x = ((const uint8_t*)ptr)[0];
                            [[fallthrough]];
                        default:
                            break;
                        }
                    }
                    break;
                default:
                    break;
                }

                vertex->attributes[i] = attribute;
            }
        }

        vertex->attributeEnableBits = _inputAssemblyStageState->vertexAttributeEnableBits;
    }
}
