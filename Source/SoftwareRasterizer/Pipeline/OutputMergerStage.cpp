

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

        if (_depthState->depthEnable)
        {
            float storedDepth = fetchTexelDepth(x, y);
            bool passed = depthTest(storedDepth, depth);
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

    void OutputMergerStage::storeTexelColor(int x, int y, const Vector4& color)
    {
        TextureOperations::StoreTexelColor(&(_renderTarget->colorBuffer), x, y, color);
    }

    // 逆lerp
    static inline float InverseLerp(float a, float b, float val)
    {
        return (val - a) / (b - a);
    }

    void OutputMergerStage::storeTexelDepth(int x, int y, float depth)
    {
        // DepthRange -> [0,1] にマップする
        float a = _depthRange->depthRangeNearVal;
        float b = _depthRange->depthRangeFarVal;
        float t = InverseLerp(a, b, depth);
        float normarizedDpeth = std::clamp(t, 0.0f, 1.0f);// saturate

        TextureOperations::StoreTexelDepth(&(_renderTarget->depthBuffer), x,y, normarizedDpeth);
    }

}
