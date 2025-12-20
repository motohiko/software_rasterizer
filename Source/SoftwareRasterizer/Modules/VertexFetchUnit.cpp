#include "VertexFetchUnit.h"
#include "DataConversion.h"

namespace SoftwareRasterizer
{
    void VertexFetchUnit::FetchVertex(const InputLayout* inputLayout, const VertexBuffers* vertexBuffers, int vertexIndex, VertexDataA* vertex)
	{
        // Vertex Element Loop

        for (int i = 0; i < kMaxVertexAttributes; i++)
        {
            if (inputLayout->enabledVertexAttributeIndexBits & (1u << i))
            {
                // Source Element Fetch
                // Format Conversion
                // Destination Component Selection

                const InputElement* inputElement = &(inputLayout->elements[i]);
                const VertexBuffer* vertexBuffer = &(vertexBuffers->vertexBuffers[i]);

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
                            attribute.w = DataConversionRule::ConvertUnorm8ToFloat32(((const uint8_t*)ptr)[3]);
                            [[fallthrough]];
                        case 3:
                            attribute.z = DataConversionRule::ConvertUnorm8ToFloat32(((const uint8_t*)ptr)[2]);
                            [[fallthrough]];
                        case 2:
                            attribute.y = DataConversionRule::ConvertUnorm8ToFloat32(((const uint8_t*)ptr)[1]);
                            [[fallthrough]];
                        case 1:
                            attribute.x = DataConversionRule::ConvertUnorm8ToFloat32(((const uint8_t*)ptr)[0]);
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
