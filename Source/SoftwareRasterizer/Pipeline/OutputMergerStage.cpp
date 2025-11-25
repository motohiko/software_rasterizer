

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
#include <cmath>// lerp

namespace SoftwareRasterizer
{
    OutputMergerStage::OutputMergerStage()
    {
    }

    void OutputMergerStage::execute(int x, int y, const Vector4& color, float depth)
    {
        // x, y はウィンドウ座標

        if (_depthState->depthTestEnabled)
        {
            float storedDepth = fetchTexelDepth(x, y);
            bool passed = depthTest(depth, storedDepth);
            if (!passed)
            {
                return;
            }
        }

        storeTexelColor(x, y, color);
        storeTexelDepth(x, y, depth);
    }

    float OutputMergerStage::fetchTexelDepth(int x, int y) const
    {
        float normarizedDpeth = TextureOperations::FetchTexelDepth(&(_renderTarget->depthBuffer), x, y);

        // [0,1] -> DepthRange にマップする
        float a = _depthRange->depthRangeNearVal;
        float b = _depthRange->depthRangeFarVal;
        float t = normarizedDpeth;
        float dpeth = std::lerp(a, b, t);

        return dpeth;
    }

    bool OutputMergerStage::depthTest(float depth, float storedDepth)
    {
        return Comparator::Evaluate(depth, _depthState->depthFunc, storedDepth);
    }

    void OutputMergerStage::storeTexelColor(int x, int y, const Vector4& color)
    {
        TextureOperations::StoreTexelColor(&(_renderTarget->colorBuffer), x, y, color);
    }

    void OutputMergerStage::storeTexelDepth(int x, int y, float depth)
    {
        // DepthRange -> [0,1] にマップする
        float a = _depthRange->depthRangeNearVal;
        float b = _depthRange->depthRangeFarVal;
        float t = Lib::InverseLerp(a, b, depth);
        float normarizedDpeth = std::clamp(t, 0.0f, 1.0f);// saturate

        TextureOperations::StoreTexelDepth(&(_renderTarget->depthBuffer), x,y, normarizedDpeth);
    }

}
