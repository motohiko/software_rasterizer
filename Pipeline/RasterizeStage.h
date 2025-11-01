#pragma once

#include "RasterizeStageState.h"
#include "..\Types.h"
#include "..\Vector.h"

namespace SoftwareRasterizer
{
    class RasterizeStage
	{

    private:

        const RasterizeStageState* _rasterizeStageState;

    public:

        RasterizeStage(const RasterizeStageState* state);

        void validateState();

        void transformToNdcVertex(const ShadedVertex* vertex, NdcVertex* ndcVertex) const;
        void transformRasterVertex(const ShadedVertex* clippedPrimitiveVertices, const NdcVertex* ndcVertex, RasterVertex* rasterizationPoint) const;

        void getLineFragment(int x, int y, const RasterVertex* p0, const RasterVertex* p1, Fragment* fragment);
        void getTriangleFragment(int x, int y, const RasterVertex* p0, const RasterVertex* p1, const RasterVertex* p2, Fragment* fragment);

        // –Êã‚Ì‚ ‚é“_‚ªOŠpŒ`‚Ì“à•”‚É‚ ‚é‚©‚Ç‚¤‚©‚ğ”»’è
         //
         // Juan Pineda 1988 A Parallel Algorithm for Polygon Rasterization.
         //
        static float edgeFunction(const Vector2& a, const Vector2& b, const Vector2& c)
        {
            Vector2 ab = b - a;
            Vector2 ac = c - a;
            return ab.cross(ac);
        }

    };
}
