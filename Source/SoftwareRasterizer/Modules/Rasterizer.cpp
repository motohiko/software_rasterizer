#include "Rasterizer.h"
#include <cmath>// floor ceil abs 
#include <algorithm>// min max clamp
#include <cfloat>//FLT_EPSILON

namespace SoftwareRasterizer
{
    static float XatY(const Vector2& a, const Vector2& b, float y)
    {
        float slope = (b.y - a.y) / (b.x - a.x);
        float x = a.x + (y - a.y) / slope;
        return x;
    }

    static void ScanLine(const Vector2* a, const Vector2* b, int y, int* xMin, int* xMax)
    {
        Vector2 p0;
        Vector2 p1;
        if (a->y <= b->y)
        {
            p0 = *a;
            p1 = *b;
        }
        else
        {
            p0 = *b;
            p1 = *a;
        }

        // セルの上下
        float y0 = (float)y;
        float y1 = (float)y + 1.0f;

        // y0とy1の範囲で線分をクリップ
        if (p1.y < y0)
        {
            p0 = p1;
        }
        else if (y1 < p0.y)
        {
            p1 = p0;
        }
        else
        {
            if (p0.y < y0 && y0 < p1.y)
            {
                p0.x = XatY(p0, p1, y0);
                p0.y = y0;
            }
            if (p0.y < y1 && y1 < p1.y)
            {
                p1.x = XatY(p0, p1, y1);
                p1.y = y1;
            }
        }

        *xMin = (int)std::floor(std::min(p0.x, p1.x));
        *xMax = (int)std::floor(std::max(p0.x, p1.x));
    }

    Rasterizer::Rasterizer()
    {
        _raster.scanlines = nullptr;
        _raster.scanlineNum = 0;
        _raster.minY = 0x7fffffff;
        _raster.maxY = 0;
    }

    Rasterizer::~Rasterizer()
    {
        delete _raster.scanlines;
    }

    void Rasterizer::setClipRect(int minX, int minY, int maxX, int maxY)
    {
        _clipRectMinX = minX;
        _clipRectMinY = minY;
        _clipRectMaxX = maxX;
        _clipRectMaxY = maxY;
    }

    void Rasterizer::setSsanlineNum(int scanlineNum)
    {
        if (_raster.scanlineNum != scanlineNum)
        {
            delete _raster.scanlines;
            _raster.scanlines = new Scanline[scanlineNum];
            _raster.scanlineNum = scanlineNum;
            _raster.minY = 0x7fffffff;
            _raster.maxY = 0;
            for (int y = 0; y < _raster.scanlineNum; y++)
            {
                Scanline* scanline = &(_raster.scanlines[y]);
                scanline->minX = 0x7fffffff;
                scanline->maxX = 0;
            }
        }
    }

    void Rasterizer::begin()
    {
    }

    void Rasterizer::addEgde(const Vector2* p0, const Vector2* p1)
    {
        struct Seg
        {
            const Vector2* p0;
            const Vector2* p1;
            int minY;
            int maxY;
        };

        Seg seg;

        seg.p0 = p0;
        seg.p1 = p1;

        int i = 0;

        seg.minY = (int)std::floor(std::min(seg.p0->y, seg.p1->y));
        seg.maxY = (int)std::floor(std::max(seg.p0->y, seg.p1->y));

        seg.minY = std::clamp(seg.minY, _clipRectMinY, _clipRectMaxY);
        seg.maxY = std::clamp(seg.maxY, _clipRectMinY, _clipRectMaxY);

        _raster.minY = std::min(_raster.minY, seg.minY);
        _raster.maxY = std::max(_raster.maxY, seg.maxY);

        for (int y = seg.minY; y <= seg.maxY; y++)
        {
            int minX, maxX;
            ScanLine(seg.p0, seg.p1, y, &minX, &maxX);

            minX = std::clamp(minX, _clipRectMinX, _clipRectMaxX);
            maxX = std::clamp(maxX, _clipRectMinX, _clipRectMaxX);

            Scanline* scanline = &(_raster.scanlines[y]);
            scanline->minX = std::min(scanline->minX, minX);
            scanline->maxX = std::max(scanline->maxX, maxX);
        }
    }

    void Rasterizer::addBoundingBox(const Vector2* p0, const Vector2* p1)
    {
        int minX = (int)std::floor(std::min(p0->x, p1->x));
        int maxX = (int)std::floor(std::max(p0->x, p1->x));
        int minY = (int)std::floor(std::min(p0->y, p1->y));
        int maxY = (int)std::floor(std::max(p0->y, p1->y));

        minX = std::clamp(minX, _clipRectMinX, _clipRectMaxX);
        maxX = std::clamp(maxX, _clipRectMinX, _clipRectMaxX);
        minY = std::clamp(minY, _clipRectMinY, _clipRectMaxY);
        maxY = std::clamp(maxY, _clipRectMinY, _clipRectMaxY);

        _raster.minY = std::min(_raster.minY, minY);
        _raster.maxY = std::max(_raster.maxY, maxY);
        for (int y = _raster.minY; y <= _raster.maxY; y++)
        {
            Scanline* scanline = &(_raster.scanlines[y]);
            scanline->minX = minX;
            scanline->maxX = maxX;
        }
    }

    void Rasterizer::addBoundingBox(const Vector2* p0, const Vector2* p1, const Vector2* p2)
    {
        int minX = (int)std::floor(std::min(p0->x, std::min(p1->x, p2->x)));
        int maxX = (int)std::floor(std::max(p0->x, std::max(p1->x, p2->x)));
        int minY = (int)std::floor(std::min(p0->y, std::min(p1->y, p2->y)));
        int maxY = (int)std::floor(std::max(p0->y, std::max(p1->y, p2->y)));
  
        minX = std::clamp(minX, _clipRectMinX, _clipRectMaxX);
        maxX = std::clamp(maxX, _clipRectMinX, _clipRectMaxX);
        minY = std::clamp(minY, _clipRectMinY, _clipRectMaxY);
        maxY = std::clamp(maxY, _clipRectMinY, _clipRectMaxY);

        _raster.minY = std::min(_raster.minY, minY);
        _raster.maxY = std::max(_raster.maxY, maxY);
        for (int y = _raster.minY; y <= _raster.maxY; y++)
        {
            Scanline* scanline = &(_raster.scanlines[y]);
            scanline->minX = minX;
            scanline->maxX = maxX;
        }
    }

    void Rasterizer::end()
    {
        for (int y = _raster.minY; y <= _raster.maxY; y++)
        {
            Scanline* scanline = &(_raster.scanlines[y]);
            scanline->minX = 0x7fffffff;
            scanline->maxX = 0;
        }
        _raster.minY = 0x7fffffff;
        _raster.maxY = 0;
    }

}
