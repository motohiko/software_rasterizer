
// ウィンドウ座標（ビューポート変換後）
//
//       +y                
//         |
//         |
//         +---- +x
//   (0, 0)
//
//
// Windows GUI の座標系
//
//   (0, 0)
//         +---- +x
//         |
//         |
//       +y                
//
//
// DIBのメモリを直接参照したとき座標系
//
//       +y                
//         |
//         |
//         +---- +x
//   (0, 0)
//

#include "OutputMergerStage.h"
#include "..\RenderingContext.h"
#include "..\..\Lib\Algorithm.h"

namespace SoftwareRasterizer
{
    OutputMergerStage::OutputMergerStage()
    {
    }

    void OutputMergerStage::execute(int x, int y, const Vector4& color, float depth)
    {
        if (_depthState->depthEnable)
        {
            float storedDepth = readDepth(x, y);
            bool passed = depthTest(storedDepth, depth);
            if (!passed)
            {
                return;
            }
        }

        writePixel(x, y, color, depth);
    }

    bool OutputMergerStage::depthTest(float storedDepth, float depth)
    {
        bool passed;
        switch (_depthState->depthFunc)
        {
        case ComparisonType::kNever:
            passed = false;
            break;
        case ComparisonType::kLess:
            passed = (depth < storedDepth);
            break;
        case ComparisonType::kEqual:
            passed = (depth == storedDepth);
            break;
        case ComparisonType::kLessEqual:
            passed = (depth <= storedDepth);
            break;
        case ComparisonType::kGreater:
            passed = (depth > storedDepth);
            break;
        case ComparisonType::kNotEqual:
            passed = (depth != storedDepth);
            break;
        case ComparisonType::kGreaterEqual:
            passed = (depth >= storedDepth);
            break;
        case ComparisonType::kAlways:
            passed = true;
            break;
        default:
            passed = false;
            break;
        }

        return passed;
    }

    float OutputMergerStage::readDepth(int x, int y) const
    {
        // x, y はウィンドウ相対座標

        if (0 <= x && x < _renderTarget->depthBuffer.width || 0 <= y || y < _renderTarget->depthBuffer.height)
        {
            size_t depthOffset = (_renderTarget->depthBuffer.widthBytes * y) + (sizeof(float) * x);
            const float* depthSrc = (float*)(((uintptr_t)_renderTarget->depthBuffer.addr) + depthOffset);
            return *depthSrc;
        }
        else
        {
            return 0.0f;
        }
    }

    void OutputMergerStage::writePixel(int x, int y, const Vector4& color, float depth)
    {
        // x, y はウィンドウ座標

        if (0 <= x && x < _renderTarget->colorBuffer.width || 0 <= y || y < _renderTarget->colorBuffer.height)
        {
            // DIBも左下が(0,0)なので上下反転は不要
            size_t colorOffset = (_renderTarget->colorBuffer.widthBytes * y) + (sizeof(uint32_t) * x);
            uint32_t* colorDst = (uint32_t*)(((uintptr_t)_renderTarget->colorBuffer.addr) + colorOffset);
            uint32_t r = Lib::DenormalizeByte(color.x);
            uint32_t g = Lib::DenormalizeByte(color.y);
            uint32_t b = Lib::DenormalizeByte(color.z);

            //r += (*colorDst >> 16) & 0xff;
            //g += (*colorDst >> 8) & 0xff;
            //b += (*colorDst >> 0) & 0xff;
            //r = std::min(r, 0xffu);
            //g = std::min(g, 0xffu);
            //b = std::min(b, 0xffu);

            *colorDst = (r << 16) | (g << 8) | (b);// BI_RGB
        }

        if (0 <= x && x < _renderTarget->depthBuffer.width || 0 <= y || y < _renderTarget->depthBuffer.height)
        {
            size_t depthOffset = (_renderTarget->depthBuffer.widthBytes * y) + (sizeof(float) * x);
            float* depthDst = (float*)(((uintptr_t)_renderTarget->depthBuffer.addr) + (_renderTarget->depthBuffer.widthBytes * y) + (sizeof(float) * x));
            *depthDst = depth;
        }
    }

}
