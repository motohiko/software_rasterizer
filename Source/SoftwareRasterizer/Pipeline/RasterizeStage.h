#pragma once

#include "..\State\RasterizerState.h"
#include "..\State\Viewport.h"
#include "..\Types.h"
#include "..\..\Lib\\Vector.h"

namespace SoftwareRasterizer
{
    struct RasterPrimitive
    {
        PrimitiveType primitiveType;
        ShadedVertex vertices[3];
        int vertexNum;
    };

    class RasterizeStage
	{

    private:

        const RasterizerState* _rasterizerState = nullptr;
        const Viewport* _viewport = nullptr;

        class RenderingContext* _renderingContext = nullptr;

        int _windowWidth = 0;
        int _windowHeight = 0;

        int _clipRectMinX = 0;
        int _clipRectMinY = 0;
        int _clipRectMaxX = 0;
        int _clipRectMaxY = 0;

        float _sarea2 = 0.0f;// singed area 2x

    public:

        static void validateState(const Viewport* state);

        RasterizeStage();

        void setWindowSize(int width, int height);

        void input(const RasterizerState* rasterizerState) { _rasterizerState = rasterizerState; }
        void input(const Viewport* viewport) { _viewport = viewport; }

        void ouput(class RenderingContext* renderingContext) { _renderingContext = renderingContext; }

        void prepareRasterize();

        void rasterizePrimitive(RasterPrimitive& rasterPrimitive);

    private:

        static void transformToNdcVertex(const ShadedVertex* vertex, NdcVertex* ndcVertex)
        {
            ndcVertex->ndcPosition = vertex->clipPosition.getXYZ() / vertex->clipPosition.w;
        }

        Vector2 transformWindow(const NdcVertex* ndcVertex) const;
        float mapDepthRange(const NdcVertex* ndcVertex) const;

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
