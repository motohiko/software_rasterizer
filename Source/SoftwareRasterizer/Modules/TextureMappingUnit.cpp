#include "TextureMappingUnit.h"
#include "TextureOperations.h" 
#include <cmath>// floor
#include <algorithm>//clamp

namespace SoftwareRasterizer
{
    Vector4 TextureMappingUnit::SamplePoint(const Sampler2D* sampler, const IntVector2& texelCoord)
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

    Vector4 TextureMappingUnit::SampleNearestPoint(const Sampler2D* sampler, const Vector2& texcoord)
    {
        int width = sampler->texture->width;
        int height = sampler->texture->height;

        IntVector2 texelCoord(
            (int)std::floor(texcoord.x * width),
            (int)std::floor(texcoord.y * height)
        );

        Vector4 color = SamplePoint(sampler, texelCoord);

        return color;
    }

    static Vector4 LerpColor(const Vector4& a, const Vector4& b, float t)
    {
        return a + ((b - a) * t);
    }

    Vector4 TextureMappingUnit::SampleBilinearInterpolation(const Sampler2D* sampler, const Vector2& texcoord)
    {
        int width = sampler->texture->width;
        int height = sampler->texture->height;

        // note.
        //
        //  +---------+---------+
        //  |         |         |
        //  |   q00   |   q01   |
        //  |         |         |
        //  +---------+---------+
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

        // q00の位置と補間割合を求める

        float x = texcoord.x * width;
        float y = texcoord.y * height;

        // 0.5(半テクセル)だけずらす
        x = x - 0.5f;
        y = y - 0.5f;

        // 整数部（正負）と小数部(正)に分解
        int xi = (int)std::floor(x);
        int yi = (int)std::floor(y);
        float xf = x - xi;
        float yf = y - yi;

        // 補間対象のテクセルを取得
        Vector4 q00 = SamplePoint(sampler, IntVector2(xi + 0, yi + 0));
        Vector4 q01 = SamplePoint(sampler, IntVector2(xi + 1, yi + 0));
        Vector4 q10 = SamplePoint(sampler, IntVector2(xi + 0, yi + 1));
        Vector4 q11 = SamplePoint(sampler, IntVector2(xi + 1, yi + 1));

        // 水平方向に補間
        Vector4 r0 = LerpColor(q00, q01, xf);
        Vector4 r1 = LerpColor(q10, q11, xf);

        // 垂直方向に補間
        Vector4 color = LerpColor(r0, r1, yf);

        return color;
    }

}
