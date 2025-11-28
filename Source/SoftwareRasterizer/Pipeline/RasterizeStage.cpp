#include "RasterizeStage.h"
#include "..\RenderingContext.h"
#include "..\..\Lib\Algorithm.h"
#include <cassert>
#include <cmath>// lerp floor ceil abs 
#include <algorithm>// min max clamp
#include <cfloat>//FLT_EPSILON

namespace SoftwareRasterizer
{
    void RasterizeStage::validateState(const Viewport* state)
    {
    }

    RasterizeStage::RasterizeStage()
    {
    }

    RasterizeStage::~RasterizeStage()
    {
    }

    void RasterizeStage::prepareRasterize()
    {
        int windowMaxX = _windowSize->windowWidth - 1;
        int windowMaxY = _windowSize->windowHeight - 1;
        int viewportMaxX = _viewport->viewportX + _viewport->viewportWidth - 1;
        int viewportMaxY = _viewport->viewportY + _viewport->viewportHeight - 1;

        _clipRectMinX = std::max(0, _viewport->viewportX);
        _clipRectMinY = std::max(0, _viewport->viewportY);
        _clipRectMaxX = std::min(windowMaxX, viewportMaxX);
        _clipRectMaxY = std::min(windowMaxY, viewportMaxY);

        _rasterizer.setClipRect(_clipRectMinX, _clipRectMinY, _clipRectMaxX, _clipRectMaxY);

        int scanlineNum = _windowSize->windowHeight;
        _rasterizer.setSsanlineNum(_windowSize->windowHeight);
    }

    // 正規化デバイス座標からウィンドウ座標へ変換
    Vector2 RasterizeStage::transformWindowSpace(const VertexDataC* ndcVertex) const
    {
        // 
        // 参考 https://registry.khronos.org/OpenGL-Refpages/gl2.1/xhtml/glViewport.xml
        // 

        // note.
        // 
        // ウィンドウ座標系(xy)
        // 
        //     +y
        //       |
        //       |
        //       +--- +x
        //  (0,0)


        //                   (x+width, y+height)
        //       +----------+
        //       |          |
        //       |          |
        //       |          |
        //       +----------+  
        //  (x,y)

        float x = (float)_viewport->viewportX;
        float y = (float)_viewport->viewportY;
        float width = (float)_viewport->viewportWidth;
        float height = (float)_viewport->viewportHeight;

        return Vector2(
            ((ndcVertex->ndcPosition.x + 1.0f) * (width / 2.0f)) + x,
            ((ndcVertex->ndcPosition.y + 1.0f) * (height / 2.0f)) + y
        );
    }

    // 正規化デバイス座標の z を深度範囲にマップ
    float RasterizeStage::mapDepthRange(const VertexDataC* ndcVertex) const
    {
        //
        // 参考 https://registry.khronos.org/OpenGL-Refpages/gl4/html/glDepthRange.xhtml
        //

        // note.
        // 
        // 正規化座標は左手系（奥に+Z）
        // 

        float t = (ndcVertex->ndcPosition.z + 1.0f) / 2.0f;
        return std::lerp(_depthRange->depthRangeNearVal, _depthRange->depthRangeFarVal, t);
    }

    void RasterizeStage::transformRasterVertex(const VertexDataB* clippedPrimitiveVertex, const VertexDataC* ndcVertex, VertexDataD* rasterizationPoint) const
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

        //
        // ビューポート変換
        //

        // 正規化デバイス座標からウィンドウ座標へ変換
        rasterizationPoint->wndPosition = transformWindowSpace(ndcVertex);

        // 正規化デバイス座標の z を深度範囲にマップ
        rasterizationPoint->depth = mapDepthRange(ndcVertex);

        //  1/W を保存（パースペクティブコレクト対応）
        float w = clippedPrimitiveVertex->clipPosition.w;
        assert(w != 0.0f);
        rasterizationPoint->invW = 1.0f / w;

        //  補間変数も W で除算（パースペクティブコレクト対応）
        for (int i = 0; i < clippedPrimitiveVertex->varyingNum; i++)
        {
            rasterizationPoint->varyingsDividedByW[i] = clippedPrimitiveVertex->varyings[i] / w;
        }

        // TODO: remove
        rasterizationPoint->varyingNum = clippedPrimitiveVertex->varyingNum;
    }

    void RasterizeStage::rasterizePrimitive(RasterPrimitive& rasterPrimitive)
    {
        // 各頂点を正規化デバイス座標へ変換（W除算）
        VertexDataC ndcVertices[3];
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
            switch (_rasterizerState->frontFaceMode)
            {
            case FrontFaceMode::kCounterClockwise:
                n = (p1 - p0).cross(p2 - p0);
                break;
            case FrontFaceMode::kClockwise:
                n = (p2 - p0).cross(p1 - p0);
                break;
            default:
                n = 0.0f;
                break;
            }

            bool passed;
            switch (_rasterizerState->cullFaceMode)
            {
            case CullFaceMode::kNone:
                passed = true;
                break;
            case CullFaceMode::kBack:
                passed = (0.0f < n);
                break;
            case CullFaceMode::kFront:
                passed = (n < 0.0f);
                break;
            case CullFaceMode::kFrontAndBack:
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
        VertexDataD rasterVertices[3];
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


    void RasterizeStage::rasterizeLine(const VertexDataD* p0, const VertexDataD* p1)
    {
        _rasterizer.begin();

        bool opt = true;
        if (opt)
        {
            _rasterizer.addEgde(&(p0->wndPosition), &(p1->wndPosition));
        }
        else
        {
            _rasterizer.addBoundingBox(&(p0->wndPosition), &(p1->wndPosition));
        }

        const Raster& _raster = *_rasterizer.getRaster();
        for (int y = _raster.minY; y <= _raster.maxY; y += 2)
        {
            int y0 = y;
            int y1 = y + 1;
            int y1clamped = std::clamp(y1, _raster.minY, _raster.maxY);
            int minX = std::min(_raster.scanlines[y0].minX, _raster.scanlines[y1clamped].minX);
            int maxX = std::max(_raster.scanlines[y0].maxX, _raster.scanlines[y1clamped].maxX);
            for (int x = minX; x <= maxX; x += 2)
            {
                int x0 = x;
                int x1 = x + 1;
                getLineFragment(x0, y0, p0, p1, _quadFragment->getQ00());
                getLineFragment(x1, y0, p0, p1, _quadFragment->getQ01());
                getLineFragment(x0, y1, p0, p1, _quadFragment->getQ10());
                getLineFragment(x1, y1, p0, p1, _quadFragment->getQ11());
                if (_quadFragment->getQ00()->isOnPrimitive ||
                    _quadFragment->getQ01()->isOnPrimitive ||
                    _quadFragment->getQ10()->isOnPrimitive ||
                    _quadFragment->getQ11()->isOnPrimitive)
                {
                    _renderingContext->outputFragment();
                }
            }
        }

        _rasterizer.end();
    }

    void RasterizeStage::rasterizeTriangle(const VertexDataD* p0, const VertexDataD* p1, const VertexDataD* p2)
    {
        _rasterizer.begin();
        bool opt = true;
        if (opt)
        {
            _rasterizer.addEgde(&(p0->wndPosition), &(p1->wndPosition));
            _rasterizer.addEgde(&(p1->wndPosition), &(p2->wndPosition));
            _rasterizer.addEgde(&(p2->wndPosition), &(p0->wndPosition));
        }
        else
        {
            _rasterizer.addBoundingBox(&(p0->wndPosition), &(p1->wndPosition), &(p2->wndPosition));
        }

        _sarea2 = edgeFunction(p0->wndPosition, p1->wndPosition, p2->wndPosition);

        const Raster& _raster = *_rasterizer.getRaster();
        for (int y = _raster.minY; y <= _raster.maxY; y += 2)
        {
            int y0 = y;
            int y1 = y + 1;
            int y1clamped = std::clamp(y1, _raster.minY, _raster.maxY);
            int xmin = std::min(_raster.scanlines[y0].minX, _raster.scanlines[y1clamped].minX);
            int xmax = std::max(_raster.scanlines[y0].maxX, _raster.scanlines[y1clamped].maxX);
            for (int x = xmin; x <= xmax; x += 2)
            {
                int x0 = x;
                int x1 = x + 1;
                getTriangleFragment(x0, y0, p0, p1, p2, _quadFragment->getQ00());
                getTriangleFragment(x1, y0, p0, p1, p2, _quadFragment->getQ01());
                getTriangleFragment(x0, y1, p0, p1, p2, _quadFragment->getQ10());
                getTriangleFragment(x1, y1, p0, p1, p2, _quadFragment->getQ11());
                if (_quadFragment->getQ00()->isOnPrimitive ||
                    _quadFragment->getQ01()->isOnPrimitive ||
                    _quadFragment->getQ10()->isOnPrimitive ||
                    _quadFragment->getQ11()->isOnPrimitive)
                {
                    _renderingContext->outputFragment();
                }
            }
        }

        _rasterizer.end();
    }


    bool CheckSegmentsIntersect(const Vector2& a, const Vector2& b, const Vector2& c, const Vector2& d)
    {
        // 線分ABを延長した直線と線分CDが交差するか？
        Vector2 ab = (b - a);
        Vector2 ac = (c - a);
        Vector2 ad = (d - a);
        float cSideVal = ab.cross(ac);
        float dSideVal = ab.cross(ad);
        if (FLT_EPSILON < cSideVal && FLT_EPSILON < dSideVal)
        {
            // 交差しない
            return false;
        }
        if (cSideVal < -FLT_EPSILON && dSideVal < -FLT_EPSILON)
        {
            // 交差しない
            return false;
        }

        // 線分CDを延長した直線と線分ABが交差するか？
        Vector2 cd = (d - c);
        Vector2 ca = (a - c);
        Vector2 cb = (b - c);
        float aSideVal = cd.cross(ca);
        float bSideVal = cd.cross(cb);
        if (FLT_EPSILON < aSideVal && FLT_EPSILON < bSideVal)
        {
            // 交差しない
            return false;
        }
        if (aSideVal < -FLT_EPSILON && bSideVal < -FLT_EPSILON)
        {
            // 交差しない
            return false;
        }

        return true;
    }

    void RasterizeStage::getLineFragment(int x, int y, const VertexDataD* p0, const VertexDataD* p1, FragmentDataA* fragment)
    {
        {
            // 菱形の各頂点
            Vector2 diamondCorner0(x + 0.5f, y + 0.0f);
            Vector2 diamondCorner1(x + 1.0f, y + 0.5f);
            Vector2 diamondCorner2(x + 0.5f, y + 1.0f);
            Vector2 diamondCorner3(x + 0.0f, y + 0.5f);

            // 菱形の各辺と交差判定
            fragment->isOnPrimitive = false;
            if (CheckSegmentsIntersect(p0->wndPosition, p1->wndPosition, diamondCorner0, diamondCorner1))
            {
                fragment->isOnPrimitive = true;
            }
            if (CheckSegmentsIntersect(p0->wndPosition, p1->wndPosition, diamondCorner1, diamondCorner2))
            {
                fragment->isOnPrimitive = true;
            }
            if (CheckSegmentsIntersect(p0->wndPosition, p1->wndPosition, diamondCorner2, diamondCorner3))
            {
                fragment->isOnPrimitive = true;
            }
            if (CheckSegmentsIntersect(p0->wndPosition, p1->wndPosition, diamondCorner3, diamondCorner0))
            {
                fragment->isOnPrimitive = true;
            }
        }

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
    }

    void RasterizeStage::getTriangleFragment(int x, int y, const VertexDataD* p0, const VertexDataD* p1, const VertexDataD* p2, FragmentDataA* fragment)
    {
        Vector2 p(x + 0.5f, y + 0.5f);// ピクセルの中心

        // 重心座標の重みを求める（外積は三角形の面積の２倍）
        assert(0.0f != _sarea2);
        float b0 = edgeFunction(p1->wndPosition, p2->wndPosition, p) / _sarea2;
        float b1 = edgeFunction(p2->wndPosition, p0->wndPosition, p) / _sarea2;
        float b2 = edgeFunction(p0->wndPosition, p1->wndPosition, p) / _sarea2;

        // ピクセルの中心を内外判定
        // TODO: up-left rule.
        fragment->isOnPrimitive = true;
        if (b0 < 0.0f)
        {
            fragment->isOnPrimitive = false;
        }
        if (b1 < 0.0f)
        {
            fragment->isOnPrimitive = false;
        }
        if (b2 < 0.0f)
        {
            fragment->isOnPrimitive = false;
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
        fragment->wndPosition = p;
        fragment->depth = depth;
        fragment->invW = invW;
        for (int i = 0; i < varyingNum; i++)
        {
            fragment->varyings[i] = varyingVariables[i] * w;
        }
        fragment->varyingNum = varyingNum;

    }
}
