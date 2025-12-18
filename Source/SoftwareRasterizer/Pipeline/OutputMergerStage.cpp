

// note.
//
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
#include "..\Modules\TextureOperations.h" 
#include "..\Modules\Comparator.h" 
#include "..\..\Lib\Algorithm.h"
#include <algorithm>// clamp

namespace SoftwareRasterizer
{
    OutputMergerStage::OutputMergerStage()
    {
    }

    void OutputMergerStage::execute(const IntVector2& texelCoord, const PixelData* pixel)
    {
        float normarizedDpeth = normalizeDepth(pixel->depth);

        if (_depthState->depthTestEnabled)
        {
            float storedDepth = fetchPixelDepth(texelCoord);

            bool passed = depthTest(normarizedDpeth, storedDepth);
            if (!passed)
            {
                return;
            }
        }

        storePixelColor(texelCoord, pixel->color);
        storePixelDepth(texelCoord, normarizedDpeth);
    }

    float OutputMergerStage::normalizeDepth(float depth) const
    {
        // [0,1] にマップする
        float a = _depthRange->depthRangeNearVal;
        float b = _depthRange->depthRangeFarVal;
        float t = Lib::InverseLerp(a, b, depth);
        return std::clamp(t, 0.0f, 1.0f);// saturate
    }

    bool OutputMergerStage::depthTest(float depth, float storedDepth) const
    {
        return Comparator::Evaluate(depth, _depthState->depthFunc, storedDepth);
    }

    void OutputMergerStage::storePixelColor(const IntVector2& texelCoord, const Vector4& color)
    {
        TextureOperations::StoreTexelColor(&(_renderTarget->colorBuffer), texelCoord, color);
    }

    float OutputMergerStage::fetchPixelDepth(const IntVector2& texelCoord) const
    {
        return TextureOperations::FetchTexelDepth(&(_renderTarget->depthBuffer), texelCoord);
    }

    void OutputMergerStage::storePixelDepth(const IntVector2& texelCoord, float depth)
    {
        TextureOperations::StoreTexelDepth(&(_renderTarget->depthBuffer), texelCoord, depth);
    }

}
