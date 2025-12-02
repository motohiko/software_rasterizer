#include "TextureUnit.h"
#include "TextureOperations.h" 
#include <cmath>// floor, modff
#include <algorithm>//clamp

namespace SoftwareRasterizer
{
    Vector4 TextureUnit::SamplePoint(const Sampler2D* sampler, const IntVector2& texelCoord)
    {
        int width = sampler->texture->width;
        int height = sampler->texture->height;

        // TODO: wrap mode
        // クランプ
        IntVector2 tmp(
            std::clamp(texelCoord.x, 0, width - 1),
            std::clamp(texelCoord.y, 0, height - 1)
        );

        return TextureOperations::FetchTexelColor(sampler->texture, tmp);
    }

    Vector4 TextureUnit::SampleNearestPoint(const Sampler2D* sampler, const Vector2& texcoord)
    {
        int width = sampler->texture->width;
        int height = sampler->texture->height;
        float x = texcoord.x * width;
        float y = texcoord.y * height;

        // 小数切り捨て
        int xi = (int)std::floor(x);
        int yi = (int)std::floor(y);

        Vector4 color = SamplePoint(sampler, IntVector2(xi, yi));

        return color;
    }

    Vector4 TextureUnit::SampleBilinear(const Sampler2D* sampler, const Vector2& texcoord)
    {
        int width = sampler->texture->width;
        int height = sampler->texture->height;
        float x = texcoord.x * width;
        float y = texcoord.y * height;

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
        x = x - 0.5f;
        y = y - 0.5f;

        // 整数部（テクセルq00の位置）と小数部（補間割合）に分解
        float i;
        float xf = std::modf(x, &i);
        int  xi = (int)i;
        float yf = std::modf(y, &i);
        int yi = (int)i;

        // 補間対象のテクセルを取得
        Vector4 q00 = SamplePoint(sampler, IntVector2(xi + 0, yi + 0));
        Vector4 q01 = SamplePoint(sampler, IntVector2(xi + 1, yi + 0));
        Vector4 q10 = SamplePoint(sampler, IntVector2(xi + 0, yi + 1));
        Vector4 q11 = SamplePoint(sampler, IntVector2(xi + 1, yi + 1));

        // 水平方向に補間
        Vector4 r0 = Vector4::Lerp(q00, q01, xf);
        Vector4 r1 = Vector4::Lerp(q10, q11, xf);

        // 垂直方向に補間
        Vector4 color = Vector4::Lerp(r0, r1, yf);

        return color;
    }

}
