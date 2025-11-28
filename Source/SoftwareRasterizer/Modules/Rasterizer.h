#pragma once

#include "..\Core\Types.h"

namespace SoftwareRasterizer
{
    struct Scanline
    {
        int minX;
        int maxX;
    };

    struct Raster
    {
        Scanline* scanlines;
        int scanlineNum;
        int minY;
        int maxY;
    };

	class Rasterizer
	{

    public:

        Rasterizer();
        ~Rasterizer();
 
        void setClipRect(int minX, int minY, int maxX, int maxY);
        void setSsanlineNum(int scanlineNum);
        
        void begin();

        void addEgde(const Vector2* p0, const Vector2* p1);

        void addBoundingBox(const Vector2* p0, const Vector2* p1);
        void addBoundingBox(const Vector2* p0, const Vector2* p1, const Vector2* p2);

        const Raster* getRaster() const { return &_raster; }

        void end();

    private:

        Raster _raster;

        int _clipRectMinX = 0;
        int _clipRectMinY = 0;
        int _clipRectMaxX = 0;
        int _clipRectMaxY = 0;

	};

}
