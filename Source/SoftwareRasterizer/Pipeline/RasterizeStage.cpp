#include "RasterizeStage.h"
#include "..\RenderingContext.h"
#include "..\..\Lib\Algorithm.h"
#include <cassert>
#include <cmath>// lerp floor ceil abs 
#include <algorithm>// min max
#include <cfloat>//FLT_EPSILON

namespace SoftwareRasterizer
{
    void RasterizeStage::validateState(const Viewport* state)
    {
    }

    RasterizeStage::RasterizeStage()
    {
        _raster.scanlines = nullptr;
        _raster.scanlineNum = 0;
    }

    RasterizeStage::~RasterizeStage()
    {
        delete _raster.scanlines;
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

        int scanlineNum = _windowSize->windowHeight;
        if (_raster.scanlineNum != scanlineNum)
        {
            delete _raster.scanlines;
            _raster.scanlines = new Scanline[scanlineNum];
            _raster.scanlineNum = scanlineNum;
        }

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


    float XatY(const Vector2* a, const Vector2* b, float y)
    {
        return a->x + (y - a->y) * (b->x - a->x) / (b->y - a->y);
    }

    void RasterizeStage::rasterizeLine(const VertexDataD* p0, const VertexDataD* p1)
    {
#if !0
        const VertexDataD* a = p0;
        const VertexDataD* b = p1;

        if (a->wndPosition.y > b->wndPosition.y)
        {
            std::swap(a, b);
        }

        int baseX = (int)(a->wndPosition.x);
        int baseY = (int)(a->wndPosition.y);

        int x, y;

        y = baseY;
        for (;;)
        {
            x = baseX;
            getLineFragment(x + 0, y + 0, p0, p1, _quadFragment->getQ00());
            getLineFragment(x + 1, y + 0, p0, p1, _quadFragment->getQ01());
            getLineFragment(x + 0, y + 1, p0, p1, _quadFragment->getQ10());
            getLineFragment(x + 1, y + 1, p0, p1, _quadFragment->getQ11());
            if (_quadFragment->getQ00()->isOnPrimitive ||
                _quadFragment->getQ01()->isOnPrimitive ||
                _quadFragment->getQ10()->isOnPrimitive ||
                _quadFragment->getQ11()->isOnPrimitive)
            {
                _renderingContext->outputFragment();

                for (x = baseX - 2; ; x -= 2)
                {
                    getLineFragment(x + 0, y + 0, p0, p1, _quadFragment->getQ00());
                    getLineFragment(x + 1, y + 0, p0, p1, _quadFragment->getQ01());
                    getLineFragment(x + 0, y + 1, p0, p1, _quadFragment->getQ10());
                    getLineFragment(x + 1, y + 1, p0, p1, _quadFragment->getQ11());
                    if (_quadFragment->getQ00()->isOnPrimitive ||
                        _quadFragment->getQ01()->isOnPrimitive ||
                        _quadFragment->getQ10()->isOnPrimitive ||
                        _quadFragment->getQ11()->isOnPrimitive)
                    {
                        _renderingContext->outputFragment();
                    }
                    else
                    {
                        break;
                    }
                }

                for (x = baseX + 2; ; x += 2)
                {
                    getLineFragment(x + 0, y + 0, p0, p1, _quadFragment->getQ00());
                    getLineFragment(x + 1, y + 0, p0, p1, _quadFragment->getQ01());
                    getLineFragment(x + 0, y + 1, p0, p1, _quadFragment->getQ10());
                    getLineFragment(x + 1, y + 1, p0, p1, _quadFragment->getQ11());
                    if (_quadFragment->getQ00()->isOnPrimitive ||
                        _quadFragment->getQ01()->isOnPrimitive ||
                        _quadFragment->getQ10()->isOnPrimitive ||
                        _quadFragment->getQ11()->isOnPrimitive)
                    {
                        _renderingContext->outputFragment();
                    }
                    else
                    {
                        break;
                    }
                }

                y += 2;
            }
            else
            {
                y += 2;

                break;
            }
        }
        
        if (b->wndPosition.x < a->wndPosition.x)
        {
            for (;;)
            {
                int cnt = 0;
                for (x = baseX - 2; 0 <= x; x -= 2)
                {
                    getLineFragment(x + 0, y + 0, p0, p1, _quadFragment->getQ00());
                    getLineFragment(x + 1, y + 0, p0, p1, _quadFragment->getQ01());
                    getLineFragment(x + 0, y + 1, p0, p1, _quadFragment->getQ10());
                    getLineFragment(x + 1, y + 1, p0, p1, _quadFragment->getQ11());
                    if (_quadFragment->getQ00()->isOnPrimitive ||
                        _quadFragment->getQ01()->isOnPrimitive ||
                        _quadFragment->getQ10()->isOnPrimitive ||
                        _quadFragment->getQ11()->isOnPrimitive)
                    {
                        _renderingContext->outputFragment();
                        cnt++;
                        baseX = x;
                        break;
                    }
                }

                for (x = baseX - 2; 0 <= x; x -= 2)
                {
                    getLineFragment(x + 0, y + 0, p0, p1, _quadFragment->getQ00());
                    getLineFragment(x + 1, y + 0, p0, p1, _quadFragment->getQ01());
                    getLineFragment(x + 0, y + 1, p0, p1, _quadFragment->getQ10());
                    getLineFragment(x + 1, y + 1, p0, p1, _quadFragment->getQ11());
                    if (_quadFragment->getQ00()->isOnPrimitive ||
                        _quadFragment->getQ01()->isOnPrimitive ||
                        _quadFragment->getQ10()->isOnPrimitive ||
                        _quadFragment->getQ11()->isOnPrimitive)
                    {
                        _renderingContext->outputFragment();
                        cnt++;
                    }
                    else
                    {
                        break;
                    }
                }

                if (cnt == 0)
                {
                    break;
                }

                y += 2;
            }
        }
        else
        {
            for (;;)
            {
                int cnt = 0;
                for (x = baseX + 2; x <= (_windowSize->windowWidth - 1); x += 2)
                {
                    getLineFragment(x + 0, y + 0, p0, p1, _quadFragment->getQ00());
                    getLineFragment(x + 1, y + 0, p0, p1, _quadFragment->getQ01());
                    getLineFragment(x + 0, y + 1, p0, p1, _quadFragment->getQ10());
                    getLineFragment(x + 1, y + 1, p0, p1, _quadFragment->getQ11());
                    if (_quadFragment->getQ00()->isOnPrimitive ||
                        _quadFragment->getQ01()->isOnPrimitive ||
                        _quadFragment->getQ10()->isOnPrimitive ||
                        _quadFragment->getQ11()->isOnPrimitive)
                    {
                        _renderingContext->outputFragment();
                        cnt++;
                        baseX = x;
                        break;
                    }
                }

                for (x = baseX + 2; x <= (_windowSize->windowWidth - 1); x += 2)
                {
                    getLineFragment(x + 0, y + 0, p0, p1, _quadFragment->getQ00());
                    getLineFragment(x + 1, y + 0, p0, p1, _quadFragment->getQ01());
                    getLineFragment(x + 0, y + 1, p0, p1, _quadFragment->getQ10());
                    getLineFragment(x + 1, y + 1, p0, p1, _quadFragment->getQ11());
                    if (_quadFragment->getQ00()->isOnPrimitive ||
                        _quadFragment->getQ01()->isOnPrimitive ||
                        _quadFragment->getQ10()->isOnPrimitive ||
                        _quadFragment->getQ11()->isOnPrimitive)
                    {
                        _renderingContext->outputFragment();
                        cnt++;
                    }
                    else
                    {
                        break;
                    }
                }

                if (cnt == 0)
                {
                    break;
                }

                y += 2;
            }
        }

#endif

#if 0
        for (int qy = 0; qy < _raster.scanlineNum; qy++)
        {
            _raster.scanlines[qy].min = 0x7fffffff;
            _raster.scanlines[qy].max = -1;
        }
        _raster.min = 0x7fffffff;
        _raster.max = -1;


        {
            int ax = p0->wndPosition.x / 2;
            int ay = p0->wndPosition.y / 2;
            int bx = p1->wndPosition.x / 2;
            int by = p1->wndPosition.y / 2;

            Lib::BresenhamLine bresenhamLine;
            bresenhamLine.setup(ax, ay, bx, by);
            do
            {
                int qx = bresenhamLine.x;
                int qy = bresenhamLine.y;
                if (0 <= qy && qy <= (_raster.scanlineNum - 1))
                {
                    _raster.scanlines[qy].min = std::min(_raster.scanlines[qy].min, qx - 1);
                    _raster.scanlines[qy].max = std::max(_raster.scanlines[qy].max, qx + 1);
                    _raster.min = std::min(_raster.min, qy);
                    _raster.max = std::max(_raster.max, qy);

                }
            } while (bresenhamLine.next());
        }

        for (int qy = _raster.max; _raster.min <= qy; qy--)// 上から下へ
        {
            for (int qx = _raster.scanlines[qy].min; qx <= _raster.scanlines[qy].max; qx++)
            {
                int fx = 2 * qx;
                int fy = 2 * qy;
                getLineFragment(fx + 0, fy - 0, p0, p1, _quadFragment->getQ00());
                getLineFragment(fx + 1, fy - 0, p0, p1, _quadFragment->getQ01());
                getLineFragment(fx + 0, fy - 1, p0, p1, _quadFragment->getQ10());
                getLineFragment(fx + 1, fy - 1, p0, p1, _quadFragment->getQ11());

                if (_quadFragment->getQ00()->isOnPrimitive ||
                    _quadFragment->getQ01()->isOnPrimitive ||
                    _quadFragment->getQ10()->isOnPrimitive ||
                    _quadFragment->getQ11()->isOnPrimitive)
                {
                    _renderingContext->outputFragment();
                }
            }
        }
#endif
    }

    void RasterizeStage::rasterizeTriangle(const VertexDataD* p0, const VertexDataD* p1, const VertexDataD* p2)
    {
        _sarea2 = edgeFunction(p0->wndPosition, p1->wndPosition, p2->wndPosition);

        // ラスタライズの範囲を絞り込む
        Lib::BoundingBox2d boundingBox = {};
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

        for (int y = maxY; minY <= y; y-=2)// 上から下へ
        {
            for (int x = minX; x <= maxX; x+=2)
            {
                getTriangleFragment(x + 0, y - 0, p0, p1, p2, _quadFragment->getQ00());
                getTriangleFragment(x + 1, y - 0, p0, p1, p2, _quadFragment->getQ01());
                getTriangleFragment(x + 0, y - 1, p0, p1, p2, _quadFragment->getQ10());
                getTriangleFragment(x + 1, y - 1, p0, p1, p2, _quadFragment->getQ11());

                if (_quadFragment->getQ00()->isOnPrimitive ||
                    _quadFragment->getQ01()->isOnPrimitive ||
                    _quadFragment->getQ10()->isOnPrimitive ||
                    _quadFragment->getQ11()->isOnPrimitive)
                {
                    _renderingContext->outputFragment();
                }
            }
        }
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
