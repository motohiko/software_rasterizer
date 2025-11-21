
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
#include "TextureOperations.h" 
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


        // color blend - add
        //r += (*dst >> 16) & 0xff;
        //g += (*dst >> 8) & 0xff;
        //b += (*dst >> 0) & 0xff;
        //r = std::min(r, 0xffu);
        //g = std::min(g, 0xffu);
        //b = std::min(b, 0xffu);

        storeTexelColor(x, y, color);
        storeTexelDepth(x, y, depth);
    }

    float OutputMergerStage::fetchTexelDepth(int x, int y) const
    {
        // DepthRange の範囲にマップ
        float a = _depthRange->depthRangeNearVal;
        float b = _depthRange->depthRangeFarVal;
        float t = TextureOperations::fetchTexelDepth(&(_renderTarget->depthBuffer), x, y);
        t = std::clamp(t, 0.0f, 1.0f);
        return std::lerp(a, b, t);
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
        TextureOperations::storeTexelColor(&(_renderTarget->colorBuffer), x, y, color);
    }

    void OutputMergerStage::storeTexelDepth(int x, int y, float depth)
    {
        // [0,1] の範囲にマップ
        float a = _depthRange->depthRangeNearVal;
        float b = _depthRange->depthRangeFarVal;
        float t = (depth - a) / (b - a);// 逆lerp
        t = std::clamp(t, 0.0f, 1.0f);
        TextureOperations::storeTexelDepth(&(_renderTarget->depthBuffer), x,y, t);
    }

}
