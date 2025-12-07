#include "RasterizeStage.h"
#include "..\RenderingContext.h"
#include "..\Modules\Interpolator.h"
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

    // 透視除算(W除算)
    void RasterizeStage::applyPerspectiveDivide(const VertexDataB* clipVertex, VertexDataC* ndcVertex)
    {
        assert(vertex->clipCoord.w != 0.0f);

        Vector4 ndcCoord = clipVertex->clipCoord / clipVertex->clipCoord.w;
        ndcVertex->ndcCoord = ndcCoord.getXYZ();
        ndcVertex->w = ndcCoord.w;//=1.0f

        // 補間変数もパースペクティブコレクトでW除算する必要があるので済ませておく
        for (int i = 0; i < kMaxVaryings; i++)
        {
            if (_varyingIndexState->enabledVaryingIndexBits & (1u << i))
            {
                ndcVertex->varyingsDividedByW[i] = clipVertex->varyings[i] / clipVertex->clipCoord.w;
            }
        }
    }

    // 正規化デバイス座標からウィンドウ座標へ変換
    Vector2 RasterizeStage::transformNdcToWindowCoord(const VertexDataC* ndcVertex) const
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
            ((ndcVertex->ndcCoord.x + 1.0f) * (width / 2.0f)) + x,
            ((ndcVertex->ndcCoord.y + 1.0f) * (height / 2.0f)) + y
        );
    }

    // 正規化デバイス座標の z を深度範囲にマップ
    float RasterizeStage::mapDepthRange(float z) const
    {
        //
        // 参考 https://registry.khronos.org/OpenGL-Refpages/gl4/html/glDepthRange.xhtml
        //

        // note.
        // 
        // 正規化座標は左手系（奥に+Z）
        // 

        float t = (z + 1.0f) / 2.0f;
        return std::lerp(_depthRange->depthRangeNearVal, _depthRange->depthRangeFarVal, t);
    }

    // ビューポート変換
    void RasterizeStage::applyViewportTransform(const VertexDataB* clipVertex, const VertexDataC* ndcVertex, VertexDataD* wndVertex) const
    {
        wndVertex->wndCoord = transformNdcToWindowCoord(ndcVertex);
        wndVertex->depth = mapDepthRange(ndcVertex->ndcCoord.z);

        // 補間対象にパースペクティブコレクト用の 1/W を加えておく
        wndVertex->invW = 1.0f / clipVertex->clipCoord.w;

        // 補間変数もパースペクティブコレクト用にW除算しておく
        for (int i = 0; i < kMaxVaryings; i++)
        {
            if (_varyingIndexState->enabledVaryingIndexBits & (1u << i))
            {
                wndVertex->varyingsDividedByW[i] = clipVertex->varyings[i] / clipVertex->clipCoord.w;
            }
        }
    }

    void RasterizeStage::rasterizePrimitive(RasterPrimitive& rasterPrimitive)
    {
        VertexDataC ndcVertices[3];
        for (int i = 0; i < rasterPrimitive.vertexNum; i++)
        {
            applyPerspectiveDivide(&(rasterPrimitive.vertices[i]), &(ndcVertices[i]));
        }

        // フェイスカリング
        if (PrimitiveType::kTriangle == rasterPrimitive.primitiveType)
        {
            Vector2 p0 = ndcVertices[0].ndcCoord.getXY();
            Vector2 p1 = ndcVertices[1].ndcCoord.getXY();
            Vector2 p2 = ndcVertices[2].ndcCoord.getXY();

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

        // ビューポート変換
        VertexDataD rasterVertices[3];
        int rasterVertexNum = rasterPrimitive.vertexNum;
        for (int i = 0; i < rasterVertexNum; i++)
        {
            applyViewportTransform(&(rasterPrimitive.vertices[i]), &(ndcVertices[i]), &rasterVertices[i]);
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
            _rasterizer.addEgde(&(p0->wndCoord), &(p1->wndCoord));
        }
        else
        {
            _rasterizer.addBoundingBox(&(p0->wndCoord), &(p1->wndCoord));
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
                getLineFragment(x0, y0, p0, p1, &(_quadFragment->q00));
                getLineFragment(x1, y0, p0, p1, &(_quadFragment->q01));
                getLineFragment(x0, y1, p0, p1, &(_quadFragment->q10));
                getLineFragment(x1, y1, p0, p1, &(_quadFragment->q11));
                if (_quadFragment->q00.pixelCovered ||
                    _quadFragment->q01.pixelCovered ||
                    _quadFragment->q10.pixelCovered ||
                    _quadFragment->q11.pixelCovered)
                {
                    _renderingContext->outputQuad();
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
            _rasterizer.addEgde(&(p0->wndCoord), &(p1->wndCoord));
            _rasterizer.addEgde(&(p1->wndCoord), &(p2->wndCoord));
            _rasterizer.addEgde(&(p2->wndCoord), &(p0->wndCoord));
        }
        else
        {
            _rasterizer.addBoundingBox(&(p0->wndCoord), &(p1->wndCoord), &(p2->wndCoord));
        }

        _sarea_abc = edgeFunction(p0->wndCoord, p1->wndCoord, p2->wndCoord);

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
                getTriangleFragment(x0, y0, p0, p1, p2, &(_quadFragment->q00));
                getTriangleFragment(x1, y0, p0, p1, p2, &(_quadFragment->q01));
                getTriangleFragment(x0, y1, p0, p1, p2, &(_quadFragment->q10));
                getTriangleFragment(x1, y1, p0, p1, p2, &(_quadFragment->q11));
                if (_quadFragment->q00.pixelCovered ||
                    _quadFragment->q01.pixelCovered ||
                    _quadFragment->q10.pixelCovered ||
                    _quadFragment->q11.pixelCovered)
                {
                    _renderingContext->outputQuad();
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
            return false;
        }
        if (cSideVal < -FLT_EPSILON && dSideVal < -FLT_EPSILON)
        {
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
            return false;
        }
        if (aSideVal < -FLT_EPSILON && bSideVal < -FLT_EPSILON)
        {
            return false;
        }

        return true;
    }

    void RasterizeStage::getLineFragment(int x, int y, const VertexDataD* a, const VertexDataD* b, FragmentData* fragment)
    {
        fragment->pixelCoord = IntVector2(x, y);

        // 菱形の各辺と交差判定
        Vector2 diamondCorner0(x + 0.5f, y + 0.0f);
        Vector2 diamondCorner1(x + 1.0f, y + 0.5f);
        Vector2 diamondCorner2(x + 0.5f, y + 1.0f);
        Vector2 diamondCorner3(x + 0.0f, y + 0.5f);
        fragment->pixelCovered = false;
        if (CheckSegmentsIntersect(a->wndCoord, b->wndCoord, diamondCorner0, diamondCorner1))
        {
            fragment->pixelCovered = true;
        }
        if (CheckSegmentsIntersect(a->wndCoord, b->wndCoord, diamondCorner1, diamondCorner2))
        {
            fragment->pixelCovered = true;
        }
        if (CheckSegmentsIntersect(a->wndCoord, b->wndCoord, diamondCorner2, diamondCorner3))
        {
            fragment->pixelCovered = true;
        }
        if (CheckSegmentsIntersect(a->wndCoord, b->wndCoord, diamondCorner3, diamondCorner0))
        {
            fragment->pixelCovered = true;
        }

        // ピクセルの中心
        Vector2 p_wndCoord((float)x + 0.5f, (float)y + 0.5f);

        // 線分上の最寄り位置を求める
        Vector2 ab(b->wndCoord - a->wndCoord);
        Vector2 ap(p_wndCoord - a->wndCoord);
        float apLengthClosest = Vector2::Normalize(ab).dot(ap);
        float t = apLengthClosest / ab.getNorm();
        t = std::clamp(t, 0.0f, 1.0f);

        VertexDataD p;
        Interpolator::InterpolateLinear(&p, a, b, t, _varyingIndexState);

        assert(0.0f != p.invW);
        float w = 1.0f / p.invW;

        fragment->wndCoord = p.wndCoord;
        fragment->depth = p.depth;
        fragment->invW = p.invW;

        for (int i = 0; i < kMaxVaryings; i++)
        {
            if (_varyingIndexState->enabledVaryingIndexBits & (1u << i))
            {
                fragment->varyings[i] = p.varyingsDividedByW[i] * w;
            }
        }
    }

    void RasterizeStage::getTriangleFragment(int x, int y, const VertexDataD* a, const VertexDataD* b, const VertexDataD* c, FragmentData* fragment)
    {
        fragment->pixelCoord = IntVector2(x, y);

        // ピクセルの中心
        Vector2 p_wndCoord(x + 0.5f, y + 0.5f);

        // 重心座標
        BarycentricCoord baryCoord;
        assert(0.0f != _sarea_abc);
        baryCoord.r1 = edgeFunction(b->wndCoord, c->wndCoord, p_wndCoord) / _sarea_abc;
        baryCoord.r2 = edgeFunction(c->wndCoord, a->wndCoord, p_wndCoord) / _sarea_abc;
        baryCoord.r3 = edgeFunction(a->wndCoord, b->wndCoord, p_wndCoord) / _sarea_abc;

        // ピクセルの中心を内外判定
        fragment->pixelCovered = true;
        if (baryCoord.r1 < 0.0f)
        {
            fragment->pixelCovered = false;
        }
        if (baryCoord.r2 < 0.0f)
        {
            fragment->pixelCovered = false;
        }
        if (baryCoord.r3 < 0.0f)
        {
            fragment->pixelCovered = false;
        }

        VertexDataD p;
        Interpolator::InterpolateBarycentric(&p, a, b, c, &baryCoord, _varyingIndexState);

        assert(0.0f != p.invW);
        float w = 1.0f / p.invW;

        fragment->wndCoord = p.wndCoord;
        fragment->depth = p.depth;
        fragment->invW = p.invW;

        for (int i = 0; i < kMaxVaryings; i++)
        {
            if (_varyingIndexState->enabledVaryingIndexBits & (1u << i))
            {
                fragment->varyings[i] = p.varyingsDividedByW[i] * w;
            }
        }
    }

}
