#include "RasterizeStage.h"
#include "..\Lib\Algorithm.h"
#include <cassert>
#include <cmath>

namespace SoftwareRasterizer
{
    void RasterizeStage::validateState(const RasterizeStageState* state)
    {
    }

    RasterizeStage::RasterizeStage(const RasterizeStageState* state) :
        _rasterizeStageState(state)
    {
    }

    void RasterizeStage::setFragmentOutput(IFragmentOutput* output)
    {
        _outputFragment = output;
    }

    void RasterizeStage::setFrameSize(int width, int height)
    {
        _frameWidth = width;
        _frameHeight = height;
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
        if (false)
        {
            float lazyW = std::abs(clippedPrimitiveVertex->clipSpacePosition.w) + 0.00001f;
            assert(-lazyW <= clippedPrimitiveVertex->clipSpacePosition.x && clippedPrimitiveVertex->clipSpacePosition.x <= lazyW);
            assert(-lazyW <= clippedPrimitiveVertex->clipSpacePosition.y && clippedPrimitiveVertex->clipSpacePosition.y <= lazyW);
            assert(-lazyW <= clippedPrimitiveVertex->clipSpacePosition.z && clippedPrimitiveVertex->clipSpacePosition.z <= lazyW);
        }
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

    void RasterizeStage::rasterizePrimitive(RasterPrimitive& rasterPrimitive)
    {
        // 各頂点を正規化デバイス座標へ変換（W除算）
        NdcVertex ndcVertices[3];
        for (int i = 0; i < rasterPrimitive.vertexNum; i++)
        {
            RasterizeStage::transformToNdcVertex(&(rasterPrimitive.vertices[i]), &(ndcVertices[i]));
        }

        // フェイスカリング
        if (PrimitiveType::kTriangle == rasterPrimitive.primitiveType)
        {
            Vector2 p0 = ndcVertices[0].ndcPosition.getXY();
            Vector2 p1 = ndcVertices[1].ndcPosition.getXY();
            Vector2 p2 = ndcVertices[2].ndcPosition.getXY();

            // glFrontFace(GL_CCW) // OpenGL default
            // glEnable(GL_CULL_FACE)

            float n = (p1 - p0).cross(p2 - p0);// CCW
            if (n <= 0.0f)// GL_CULL_FACE
            {
                return;
            }
        }

        // ビューポート変換とデプス値へのマッピング
        RasterVertex rasterVertices[3];
        int rasterVertexNum = rasterPrimitive.vertexNum;
        for (int i = 0; i < rasterVertexNum; i++)
        {
            transformRasterVertex(&(rasterPrimitive.vertices[i]), &(ndcVertices[i]), &rasterVertices[i]);
        }

        // ラスタライズ
        switch (rasterVertexNum)
        {
        case 2:
            rasterizeLine(&rasterVertices[0], &rasterVertices[1]);
            break;
        case 3:
            bool wireframe = false;
            if (!wireframe)
            {
                rasterizeTriangle(&rasterVertices[0], &rasterVertices[1], &rasterVertices[2]);
            }
            else
            {
                rasterizeLine(&rasterVertices[0], &rasterVertices[1]);
                rasterizeLine(&rasterVertices[1], &rasterVertices[2]);
                rasterizeLine(&rasterVertices[2], &rasterVertices[0]);
            }
            break;
        }
    }

    void RasterizeStage::rasterizeLine(const RasterVertex* p0, const RasterVertex* p1)
    {
        int x0 = (int)std::floor(p0->wrcPosition.x);// 小数点以下切り捨て
        int y0 = (int)std::floor(p0->wrcPosition.y);
        int x1 = (int)std::floor(p1->wrcPosition.x);
        int y1 = (int)std::floor(p1->wrcPosition.y);

        int width = _frameWidth;
        int height = _frameHeight;

        BresenhamLine bresenhamLine;
        bresenhamLine.setup(x0, y0, x1, y1);
        do
        {
            int x = bresenhamLine.x;
            int y = bresenhamLine.y;

            if (x < 0 || width <= x || y < 0 || height <= y)
            {
                continue;
            }

            Fragment fragment;
            getLineFragment(x, y, p0, p1, &fragment);

            _outputFragment->outputFragment(&fragment);

        } while (bresenhamLine.next());
    }

    void RasterizeStage::rasterizeTriangle(const RasterVertex* p0, const RasterVertex* p1, const RasterVertex* p2)
    {
        // TODO：完全なエッジ関数
        // 参考 Juan Pineda 1988 A Parallel Algorithm for Polygon Rasterization. 

        int width = _frameWidth;
        int height = _frameHeight;

        // 処理が重すぎるのでラスタライズの範囲を絞り込む
        struct BoundingBox2d
        {
            float minX;
            float minY;
            float maxX;
            float maxY;
            void init()
            {
                minX = FLT_MAX;
                minY = FLT_MAX;
                maxX = -FLT_MAX;
                maxY = -FLT_MAX;
            }
            void addPoint(const Vector2& p)
            {
                minX = std::min(minX, p.x);
                minY = std::min(minY, p.y);
                maxX = std::max(maxX, p.x);
                maxY = std::max(maxY, p.y);
            }
        };
        BoundingBox2d boundingBox = {};
        boundingBox.init();
        boundingBox.addPoint(p0->wrcPosition);
        boundingBox.addPoint(p1->wrcPosition);
        boundingBox.addPoint(p2->wrcPosition);

        int minX = (int)std::floor(boundingBox.minX);// 切り捨て
        int minY = (int)std::floor(boundingBox.minY);
        int maxX = (int)std::ceil(boundingBox.maxX);// 切り上げ
        int maxY = (int)std::ceil(boundingBox.maxY);
        minX = clamp(minX, 0, width - 1);
        minY = clamp(minY, 0, height - 1);
        maxX = clamp(maxX, 0, width - 1);
        maxY = clamp(maxY, 0, height - 1);

        for (int y = maxY; minY <= y; y--)
        {
            for (int x = minX; x <= maxX; x++)
            {
                Fragment fragment;
                if (!getTriangleFragment(x, y, p0, p1, p2, &fragment))
                {
                    continue;
                }

                _outputFragment->outputFragment(&fragment);
            }
        }
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

        fragment->x = x;
        fragment->y = y;
        fragment->wrcPosition = wrcPosition;
        fragment->depth = depth;
        fragment->invW = invW;
        for (int i = 0; i < varyingNum; i++)
        {
            fragment->varyings[i] = varyingVariables[i];
        }
        fragment->varyingNum = varyingNum;
    }

    bool RasterizeStage::getTriangleFragment(int x, int y, const RasterVertex* p0, const RasterVertex* p1, const RasterVertex* p2, Fragment* fragment)
    {
        Vector2 p(x + 0.5f, y + 0.5f);// ピクセルの中心

        // 重心座標の重みを求める（外積は三角形の面積の２倍）
        float area2 = edgeFunction(p0->wrcPosition, p1->wrcPosition, p2->wrcPosition);
        assert(0.0f != area2);
        float b0 = edgeFunction(p1->wrcPosition, p2->wrcPosition, p) / area2;
        float b1 = edgeFunction(p2->wrcPosition, p0->wrcPosition, p) / area2;
        float b2 = edgeFunction(p0->wrcPosition, p1->wrcPosition, p) / area2;

        // ピクセルの中心を内外判定
        if (b0 < 0.0f)
        {
            return false;
        }
        if (b1 < 0.0f)
        {
            return false;
        }
        if (b2 < 0.0f)
        {
            return false;
        }

        // 補間
        Vector2 wrcPosition = (b0 * p0->wrcPosition) + (b1 * p1->wrcPosition) + (b2 * p2->wrcPosition);
        float depth = (b0 * p0->depth) + (b1 * p1->depth) + (b2 * p2->depth);
        float invW = (b0 * p0->invW) + (b1 * p1->invW) + (b2 * p2->invW);

        assert(0.0f != invW);
        if (0.0f == invW)
        {
            return false;
        }

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

        fragment->x = x;
        fragment->y = y;
        fragment->wrcPosition = wrcPosition;
        fragment->depth = depth;
        fragment->invW = invW;
        for (int i = 0; i < varyingNum; i++)
        {
            fragment->varyings[i] = varyingVariables[i];
        }
        fragment->varyingNum = varyingNum;

        return true;
    }
}
