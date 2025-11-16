#include "RasterizeStage.h"
#include "..\RenderingContext.h"
#include "..\..\Lib\Algorithm.h"
#include <cassert>
#include <cmath>
#include <algorithm>

namespace SoftwareRasterizer
{
    void RasterizeStage::validateState(const Viewport* state)
    {
    }

    RasterizeStage::RasterizeStage()
    {
    }

    void RasterizeStage::setWindowSize(int width, int height)
    {
        _windowWidth = width;
        _windowHeight = height;
    }

    void RasterizeStage::prepareRasterize()
    {
        int windowMaxX = _windowWidth - 1;
        int windowMaxY = _windowHeight - 1;
        int viewportMaxX = _viewport->x + _viewport->width - 1;
        int viewportMaxY = _viewport->y + _viewport->height - 1;

        _clipRectMinX = std::max(0, _viewport->x);
        _clipRectMinY = std::max(0, _viewport->y);
        _clipRectMaxX = std::min(windowMaxX, viewportMaxX);
        _clipRectMaxY = std::min(windowMaxY, viewportMaxY);
    }


    void RasterizeStage::transformRasterVertex(const ShadedVertex* clippedPrimitiveVertex, const NdcVertex* ndcVertex, RasterVertex* rasterizationPoint) const
    {
#ifndef NDEBUG
        if (false)
        {
            float lazyW = std::abs(clippedPrimitiveVertex->clipPosition.w) + 0.00001f;
            assert(-lazyW <= clippedPrimitiveVertex->clipPosition.x && clippedPrimitiveVertex->clipPosition.x <= lazyW);
            assert(-lazyW <= clippedPrimitiveVertex->clipPosition.y && clippedPrimitiveVertex->clipPosition.y <= lazyW);
            assert(-lazyW <= clippedPrimitiveVertex->clipPosition.z && clippedPrimitiveVertex->clipPosition.z <= lazyW);
        }
#endif

        // 正規化デバイス座標からウィンドウ座標へ変換（ビューポート変換）

        // note.
        // 
        // 参考 https://registry.khronos.org/OpenGL-Refpages/gl2.1/xhtml/glViewport.xml
        // 
        // ウィンドウ座標系(xy)
        // 
        //     +y
        //       |
        //       |
        //       +--- +x
        //  (0,0)
        // 
        // ウィンドウの大きさ
        // 
        //                   (ViewportWidth, ViewportHeight)
        //       +----------+
        //       |          |
        //       |          |
        //       |          |
        //       +----------+  
        //  (0,0)
        // 
        float halfWidth = (float)_viewport->width / 2.0f;
        float halfHeight = (float)_viewport->height / 2.0f;
        rasterizationPoint->wndPosition.x = ((ndcVertex->ndcPosition.x + 1.0f) * halfWidth) + (float)_viewport->x;
        rasterizationPoint->wndPosition.y = ((ndcVertex->ndcPosition.y + 1.0f) * halfHeight) + (float)_viewport->y;

        // 正規化デバイス座標の z を深度範囲にマップ
        float t = (ndcVertex->ndcPosition.z + 1.0f) / 2.0f;
        rasterizationPoint->depth = std::lerp(_rasterizerState->depthRangeNearVal, _rasterizerState->depthRangeFarVal, t);

        // パースペクティブコレクト用に 1/W を保存
        float w = clippedPrimitiveVertex->clipPosition.w;
        assert(w != 0.0f);
        rasterizationPoint->invW = 1.0f / w;
  
        //  パースペクティブコレクト用に補間変数を W で除算
        for (int i = 0; i < clippedPrimitiveVertex->varyingNum; i++)
        {
            rasterizationPoint->varyingsDividedByW[i] = clippedPrimitiveVertex->varyings[i] / w;
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

            float n;
            switch (_rasterizerState->frontFacetype)
            {
            case FrontFaceType::kCounterClockwise:
                n = (p1 - p0).cross(p2 - p0);
                break;
            case FrontFaceType::kClockwise:
                n = (p2 - p0).cross(p1 - p0);
                break;
            default:
                n = 0.0f;
                break;
            }

            bool passed;
            switch (_rasterizerState->cullFaceType)
            {
            case CullFaceType::kNone:
                passed = true;
                break;
            case CullFaceType::kBack:
                passed = (0.0f < n);
                break;
            case CullFaceType::kFront:
                passed = (n < 0.0f);
                break;
            case CullFaceType::kFrontAndBack:
            default:
                passed = false;
                break;
            }

            if (!passed)
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

        // 形状ごとの処理
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
        int x0 = (int)std::floor(p0->wndPosition.x);// 小数点以下切り捨て
        int y0 = (int)std::floor(p0->wndPosition.y);
        int x1 = (int)std::floor(p1->wndPosition.x);
        int y1 = (int)std::floor(p1->wndPosition.y);

        Lib::BresenhamLine bresenhamLine;
        bresenhamLine.setup(x0, y0, x1, y1);
        do
        {
            int x = bresenhamLine.x;
            int y = bresenhamLine.y;

            if (x < _clipRectMinX || _clipRectMaxX < x ||
                y < _clipRectMinY || _clipRectMaxY < y)
            {
                continue;
            }

            Fragment fragment;
            getLineFragment(x, y, p0, p1, &fragment);

            _renderingContext->outputFragment(&fragment);

        } while (bresenhamLine.next());
    }

    void RasterizeStage::rasterizeTriangle(const RasterVertex* p0, const RasterVertex* p1, const RasterVertex* p2)
    {
        // TODO：完全なエッジ関数
        // 参考 Juan Pineda 1988 A Parallel Algorithm for Polygon Rasterization. 

        _sarea2 = edgeFunction(p0->wndPosition, p1->wndPosition, p2->wndPosition);

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
        boundingBox.addPoint(p0->wndPosition);
        boundingBox.addPoint(p1->wndPosition);
        boundingBox.addPoint(p2->wndPosition);

        int minX = (int)std::floor(boundingBox.minX);// 切り捨て
        int minY = (int)std::floor(boundingBox.minY);
        int maxX = (int)std::ceil(boundingBox.maxX);// 切り上げ
        int maxY = (int)std::ceil(boundingBox.maxY);

        minX = std::max(minX, _clipRectMinX);
        minY = std::max(minY, _clipRectMinY);
        maxX = std::min(maxX, _clipRectMaxX);
        maxY = std::min(maxY, _clipRectMaxY);
        for (int y = maxY; minY <= y; y--)
        {
            for (int x = minX; x <= maxX; x++)
            {
                Fragment fragment;
                if (!getTriangleFragment(x, y, p0, p1, p2, &fragment))
                {
                    continue;
                }

                _renderingContext->outputFragment(&fragment);
            }
        }
    }

    void RasterizeStage::getLineFragment(int x, int y, const RasterVertex* p0, const RasterVertex* p1, Fragment* fragment)
    {
        Vector2 a(p0->wndPosition.x, p0->wndPosition.y);
        Vector2 b(p1->wndPosition.x, p1->wndPosition.y);
        Vector2 c((float)x + 0.5f, (float)y + 0.5f);// ピクセルの中心
        Vector2 ab(b - a);
        Vector2 ac(c - a);
        float acLengthClosest = Vector2::Normalize(ab).dot(ac);

        float t = acLengthClosest / ab.getNorm();
        t = std::clamp(t, 0.0f, 1.0f);

        // ２点間を補間
        Vector2 wndPosition = Vector2::Lerp(p0->wndPosition, p1->wndPosition, t);
        float depth = std::lerp(p0->depth, p1->depth, t);
        float invW = std::lerp(p0->invW, p1->invW, t);

        Vector4 varyingVariables[kMaxVaryings] = {};
        int varyingNum = p0->varyingNum;// TODO:
        for (int i = 0; i < varyingNum; i++)
        {
            const Vector4& v0 = p0->varyingsDividedByW[i];
            const Vector4& v1 = p1->varyingsDividedByW[i];
            varyingVariables[i] = Vector4::Lerp(v0, v1, t);
        }

        assert(0.0f != invW);
        float w = 1.0f / invW;

        const bool noperspective = false;
        if (noperspective)
        {
            w = 1.0f;
        }

        fragment->x = x;
        fragment->y = y;
        fragment->wndPosition = c;// wndPosition;
        fragment->depth = depth;
        fragment->invW = invW;
        for (int i = 0; i < varyingNum; i++)
        {
            fragment->varyings[i] = varyingVariables[i] * w;
        }
        fragment->varyingNum = varyingNum;
    }

    bool RasterizeStage::getTriangleFragment(int x, int y, const RasterVertex* p0, const RasterVertex* p1, const RasterVertex* p2, Fragment* fragment)
    {
        Vector2 p(x + 0.5f, y + 0.5f);// ピクセルの中心

        // 重心座標の重みを求める（外積は三角形の面積の２倍）
        assert(0.0f != _sarea2);
        float b0 = edgeFunction(p1->wndPosition, p2->wndPosition, p) / _sarea2;
        float b1 = edgeFunction(p2->wndPosition, p0->wndPosition, p) / _sarea2;
        float b2 = edgeFunction(p0->wndPosition, p1->wndPosition, p) / _sarea2;

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
        Vector2 wndPosition = (p0->wndPosition * b0) + (p1->wndPosition * b1) + (p2->wndPosition * b2);
        float depth = (b0 * p0->depth) + (b1 * p1->depth) + (b2 * p2->depth);
        float invW = (b0 * p0->invW) + (b1 * p1->invW) + (b2 * p2->invW);

        Vector4 varyingVariables[kMaxVaryings] = {};
        int varyingNum = p0->varyingNum;// TODO:
        for (int i = 0; i < varyingNum; i++)
        {
            const Vector4& v0 = p0->varyingsDividedByW[i];
            const Vector4& v1 = p1->varyingsDividedByW[i];
            const Vector4& v2 = p2->varyingsDividedByW[i];
            varyingVariables[i] = ((v0 * b0) + (v1 * b1) + (v2 * b2));
        }

        assert(0.0f != invW);
        float w = 1.0f / invW;

        const bool noperspective = false;
        if (noperspective)
        {
            w = 1.0f;
        }

        fragment->x = x;
        fragment->y = y;
        fragment->wndPosition = p;// wndPosition;
        fragment->depth = depth;
        fragment->invW = invW;
        for (int i = 0; i < varyingNum; i++)
        {
            fragment->varyings[i] = varyingVariables[i] * w;
        }
        fragment->varyingNum = varyingNum;

        return true;
    }
}
