#include "InputAssemblyStage.h"
#include "..\Modules\NormalizedConverter.h"

namespace SoftwareRasterizer
{
    void InputAssemblyStage::validateState(const InputLayout* state)
    {
    }

    InputAssemblyStage::InputAssemblyStage()
    {
        _cache.reserve(0xffff);
    }

    void InputAssemblyStage::prepareReadPrimitive()
    {
        switch (_primitiveTopologyType)
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
        int remainingVertexCount = _indexBuffer->indexNum - _readVertexCount;
        if (remainingVertexCount < _primitiveVertexNum)
        {
            primitive->primitiveType = PrimitiveType::kNone;
            primitive->vertexNum = 0;
            return false;
        }

        for (int i = 0; i < _primitiveVertexNum; i++)
        {
            uint16_t vertexIndex = _indexBuffer->indices[_readVertexCount];
            _readVertexCount++;

            primitive->vertexIndices[i] = vertexIndex;
        }

        primitive->primitiveType = _primitiveType;
        primitive->vertexNum = _primitiveVertexNum;

        return true;
    }

    void InputAssemblyStage::readAttributeVertex(uint16_t vertexIndex, VertexDataA* vertex) const
    {
        for (int i = 0; i < kMaxVertexAttributes; i++)
        {
            if (_inputLayout->enabledVertexAttributeIndexBits & (1u << i))
            {
                const InputElement* inputElement = &(_inputLayout->elements[i]);
                const VertexBuffer* vertexBuffer = &(_vertexBuffers->vertexBuffers[i]);

                Vector4 attribute = Vector4::kZero;

                // 指定個数のコンポーネントを読み取る
                uintptr_t ptr = ((uintptr_t)vertexBuffer->addr) + (inputElement->stride * vertexIndex);
                switch (inputElement->type)
                {
                case ComponentDataType::kFloat:
                    switch (inputElement->size)
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
                case ComponentDataType::kUnsignedByte:
                    if (inputElement->normalized)
                    {
                        switch (inputElement->size)
                        {
                        case 4:
                            attribute.w = NormalizedConverter::NormalizeByte(((const uint8_t*)ptr)[3]);
                            [[fallthrough]];
                        case 3:
                            attribute.z = NormalizedConverter::NormalizeByte(((const uint8_t*)ptr)[2]);
                            [[fallthrough]];
                        case 2:
                            attribute.y = NormalizedConverter::NormalizeByte(((const uint8_t*)ptr)[1]);
                            [[fallthrough]];
                        case 1:
                            attribute.x = NormalizedConverter::NormalizeByte(((const uint8_t*)ptr)[0]);
                            [[fallthrough]];
                        default:
                            break;
                        }
                    }
                    else
                    {
                        switch (inputElement->size)
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
    }

}
