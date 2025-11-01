#include "RasterizeStage.h"
#include "..\Algorithm.h"
#include <cassert>
#include <cmath>

namespace SoftwareRasterizer
{
    RasterizeStage::RasterizeStage(const RasterizeStageState* state) :
        _rasterizeStageState(state)
    {
    }

    void RasterizeStage::validateState()
    {
    }

    void RasterizeStage::transformToNdcVertex(const ShadedVertex* vertex, NdcVertex* ndcVertex) const
    {
        ndcVertex->ndcPosition = vertex->clipSpacePosition.getXYZ() / vertex->clipSpacePosition.w;
    }

    void RasterizeStage::transformRasterVertex(const ShadedVertex* clippedPrimitiveVertex, const NdcVertex* ndcVertex, RasterVertex* rasterizationPoint) const
    {
        // 正規化デバイス座標からウィンドウ座標へ変換（ビューポート変換）
        // 
        // 参考 https://registry.khronos.org/OpenGL-Refpages/gl2.1/xhtml/glViewport.xml
        // 
        // ウィンドウ座標系(xy)
        // 
        //                         (x+w,y+h)
        //               +--------+
        //               |        |
        //               |        |
        //     +y        +--------+
        //       |  (x,y)
        //       |
        //       +--- +x
        //  (0,0)
        // 

#ifndef NDEBUG
        float lazyW = std::abs(clippedPrimitiveVertex->clipSpacePosition.w) + 0.00001f;
        assert(-lazyW <= clippedPrimitiveVertex->clipSpacePosition.x && clippedPrimitiveVertex->clipSpacePosition.x <= lazyW);
        assert(-lazyW <= clippedPrimitiveVertex->clipSpacePosition.y && clippedPrimitiveVertex->clipSpacePosition.y <= lazyW);
        assert(-lazyW <= clippedPrimitiveVertex->clipSpacePosition.z && clippedPrimitiveVertex->clipSpacePosition.z <= lazyW);
#endif

        float halfWidth = (float)_rasterizeStageState->viewportWidth / 2.0f;
        float halfHeight = (float)_rasterizeStageState->viewportHeight / 2.0f;
        rasterizationPoint->wrcPosition.x = (ndcVertex->ndcPosition.x * halfWidth) + halfWidth + (float)_rasterizeStageState->viewportX;
        rasterizationPoint->wrcPosition.y = (ndcVertex->ndcPosition.y * halfHeight) + halfHeight + (float)_rasterizeStageState->viewportY;

        // 正規化デバイス座標(z)から深度に変換
        float t = (ndcVertex->ndcPosition.z + 1.0f) / 2.0f;
        rasterizationPoint->depth = lerp(_rasterizeStageState->depthRangeNearVal, _rasterizeStageState->depthRangeFarVal, t);

        // パースペクティブコレクト用の 1/W を保存
        float w = clippedPrimitiveVertex->clipSpacePosition.w;
        assert(w != 0.0f);
        rasterizationPoint->invW = 1.0f / w;
  
        //  パースペクティブコレクト用に補間変数をW除算
        for (int i = 0; i < clippedPrimitiveVertex->varyingNum; i++)
        {
            rasterizationPoint->varyingVariablesDividedByW[i] = clippedPrimitiveVertex->varyings[i] / w;
        }
        rasterizationPoint->varyingNum = clippedPrimitiveVertex->varyingNum;
    }

    void RasterizeStage::getLineFragment(int x, int y, const RasterVertex* p0, const RasterVertex* p1, Fragment* fragment)
    {
        Vector2 a(p0->wrcPosition.x, p0->wrcPosition.y);
        Vector2 b(p1->wrcPosition.x, p1->wrcPosition.y);
        Vector2 c((float)x + 0.5f, (float)y + 0.5f);// ピクセルの中心
        Vector2 ab(b - a);
        Vector2 ac(c - a);
        float acLengthClosest = ab.normalize().dot(ac);

        float t = acLengthClosest / ab.getLength();
        t = clamp(t, 0.0f, 1.0f);

        // ２点間を補間
        Vector2 wrcPosition = Vector2::Lerp(p0->wrcPosition, p1->wrcPosition, t);
        float depth = lerp(p0->depth, p1->depth, t);
        float invW = lerp(p0->invW, p1->invW, t);

        assert(0.0f != invW);
        float w = 1.0f / invW;

        // Varying 変数 も補間
        Vector4 varyingVariables[kMaxVaryings] = {};
        int varyingNum = p0->varyingNum;// TODO:
        for (int i = 0; i < varyingNum; i++)
        {
            const Vector4& v0 = p0->varyingVariablesDividedByW[i];
            const Vector4& v1 = p1->varyingVariablesDividedByW[i];
            varyingVariables[i] = Vector4::Lerp(v0, v1, t) * w;
        }

        fragment->wrcPosition = wrcPosition;
        fragment->depth = depth;
        fragment->invW = invW;
        for (int i = 0; i < varyingNum; i++)
        {
            fragment->varyings[i] = varyingVariables[i];
        }
        fragment->varyingNum = varyingNum;
        fragment->helperInvocation = false;
    }

    void RasterizeStage::getTriangleFragment(int x, int y, const RasterVertex* p0, const RasterVertex* p1, const RasterVertex* p2, Fragment* fragment)
    {
        // 参考 Juan Pineda 1988 A Parallel Algorithm for Polygon Rasterization. 

        Vector2 p(x + 0.5f, y + 0.5f);// ピクセルの中心

        // 重心座標の重みを求める（外積は三角形の面積の２倍）
        float area2 = edgeFunction(p0->wrcPosition, p1->wrcPosition, p2->wrcPosition);
        assert(0.0f != area2);
        float b0 = edgeFunction(p1->wrcPosition, p2->wrcPosition, p) / area2;
        float b1 = edgeFunction(p2->wrcPosition, p0->wrcPosition, p) / area2;
        float b2 = edgeFunction(p0->wrcPosition, p1->wrcPosition, p) / area2;

        // 補間
        Vector2 wrcPosition = (b0 * p0->wrcPosition) + (b1 * p1->wrcPosition) + (b2 * p2->wrcPosition);
        float depth = (b0 * p0->depth) + (b1 * p1->depth) + (b2 * p2->depth);
        float invW = (b0 * p0->invW) + (b1 * p1->invW) + (b2 * p2->invW);

        assert(0.0f != invW);
        float w = 1.0f / invW;

        const bool noperspective = false;
        if (noperspective)
        {
            w = 1.0f;
        }

        Vector4 varyingVariables[kMaxVaryings] = {};
        int varyingNum = p0->varyingNum;// TODO:
        for (int i = 0; i < varyingNum; i++)
        {
            const Vector4& v0 = p0->varyingVariablesDividedByW[i];
            const Vector4& v1 = p1->varyingVariablesDividedByW[i];
            const Vector4& v2 = p2->varyingVariablesDividedByW[i];
            varyingVariables[i] = ((b0 * v0) + (b1 * v1) + (b2 * v2)) * w;
        }

        // ピクセルの中心を内外判定
        bool outside = false;
        if (b0 < 0.0f)
        {
            outside = true;
        }
        if (b1 < 0.0f)
        {
            outside = true;
        }
        if (b2 < 0.0f)
        {
            outside = true;
        }

        fragment->wrcPosition = wrcPosition;
        fragment->depth = depth;
        fragment->invW = invW;
        for (int i = 0; i < varyingNum; i++)
        {
            fragment->varyings[i] = varyingVariables[i];
        }
        fragment->varyingNum = varyingNum;
        fragment->helperInvocation = outside;
    }
}
