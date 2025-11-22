#include "SamplerUtility.h"
#include "Pipeline\TextureOperations.h" 
#include <cmath>// floor, modff
#include <algorithm>//clamp

namespace SoftwareRasterizer
{
    Vector4 SamplerUtility::SamplePoint(const Texture2D* texture, int tx, int ty)
    {
        int width = texture->width;
        int height = texture->height;

        // TODO: wrap mode
        // クランプ
        tx = std::clamp(tx, 0, width - 1);
        ty = std::clamp(ty, 0, height - 1);

        return TextureOperations::fetchTexelColor(texture, tx, ty);
    }

    Vector4 SamplerUtility::SampleNearestPoint(const Sampler2D* sampler, const Vector2& uv)
    {
        int width = sampler->texture->width;
        int height = sampler->texture->height;

        float tx = uv.x * width;
        float ty = uv.y * height;

        // 小数切り捨て
        int txi = (int)std::floor(tx);
        int tyi = (int)std::floor(ty);

        Vector4 color = SamplePoint(sampler->texture, txi, tyi);

        return color;
    }

    Vector4 SamplerUtility::SampleBilinear(const Sampler2D* sampler, const Vector2& uv)
    {
        int width = sampler->texture->width;
        int height = sampler->texture->height;

        float tx = uv.x * width;
        float ty = uv.y * height;

        // note.
        //
        //  +---------+---------+
        //  |         |         |
        //  |   q00   |   q01   |
        //  |         |         |
        //  +---------+----+----+
        //  |         |         |
        //  |   q00   |   q01   |
        //  |         |         |
        //  +---------+---------+
        //
        //
        //  +---------+---------+
        //  |         |         |
        //  |    +----+----+    | -0.5
        //  |    |    |    |    |
        //  +----+----+----+----+  0.0
        //  |    |    |    |    |
        //  |    +----+----+    |  0.5
        //  |         |         |
        //  +---------+---------+
        //     -0.5   0   0.5    
        //

        // 0.5(半テクセル)だけずらす
        float x = tx - 0.5f;
        float y = ty - 0.5f;

        // 整数部（テクセルq00の位置）と小数部（補間割合）に分解
        float i;
        float xf = std::modf(x, &i);
        int  xi = i;
        float yf = std::modf(y, &i);
        int yi = i;

        // 補間対象のテクセルを取得
        Vector4 q00 = SamplePoint(sampler->texture, xi + 0, yi + 0);
        Vector4 q01 = SamplePoint(sampler->texture, xi + 1, yi + 0);
        Vector4 q10 = SamplePoint(sampler->texture, xi + 0, yi + 1);
        Vector4 q11 = SamplePoint(sampler->texture, xi + 1, yi + 1);

        // 水平方向に補間
        Vector4 r0 = Vector4::Lerp(q00, q01, xf);
        Vector4 r1 = Vector4::Lerp(q10, q11, xf);

        // 垂直方向に補間
        Vector4 color = Vector4::Lerp(r0, r1, yf);

        return color;
    }

    Vector4 SamplerUtility::SampleTexture2d(const Sampler2D* sampler, const Vector2& uv)
    {

        FilterType filter = sampler->minFilter;

        switch(filter)
        {
            case FilterType::kPoint:
                return SampleNearestPoint(sampler, uv);
            case FilterType::kBilinear:
                return SampleBilinear(sampler, uv);
            default:
                return Vector4::kZero;
        }
    }
}
