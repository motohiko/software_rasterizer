#pragma once

#include "RasterizeStageState.h"
#include "..\Types.h"
#include "..\Lib\\Vector.h"

namespace SoftwareRasterizer
{
    class IFragmentOutput
    {
    public:
        virtual void outputFragment(const Fragment* fragment) = 0;
    };

    struct RasterPrimitive
    {
        PrimitiveType primitiveType;
        ShadedVertex vertices[3];
        int vertexNum;
    };

    class RasterizeStage
	{

    private:

        const RasterizeStageState* _rasterizeStageState;

        int _frameWidth;
        int _frameHeight;

        IFragmentOutput* _outputFragment;

    public:

        static void validateState(const RasterizeStageState* state);

        static void transformToNdcVertex(const ShadedVertex* vertex, NdcVertex* ndcVertex)
        {
            ndcVertex->ndcPosition = vertex->clipSpacePosition.getXYZ() / vertex->clipSpacePosition.w;
        }

        RasterizeStage(const RasterizeStageState* state);

        void setFrameSize(int width, int height);
        void setFragmentOutput(IFragmentOutput* output);

        void rasterizePrimitive(RasterPrimitive& rasterPrimitive);

    private:

        void transformRasterVertex(const ShadedVertex* clippedPrimitiveVertices, const NdcVertex* ndcVertex, RasterVertex* rasterizationPoint) const;

        static float edgeFunction(const Vector2& a, const Vector2& b, const Vector2& c)
        {
            Vector2 ab = b - a;
            Vector2 ac = c - a;
            return ab.cross(ac);
        }

        void rasterizeLine(const RasterVertex* p0, const RasterVertex* p1);
        void rasterizeTriangle(const RasterVertex* rasterizationPoint0, const RasterVertex* rasterizationPoint1, const RasterVertex* rasterizationPopint2);

        void getLineFragment(int x, int y, const RasterVertex* p0, const RasterVertex* p1, Fragment* fragment);
        bool getTriangleFragment(int x, int y, const RasterVertex* p0, const RasterVertex* p1, const RasterVertex* p2, Fragment* fragment);
    };
}
