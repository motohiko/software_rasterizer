#pragma once

#include "..\State\RenderTarget.h"
#include "..\State\DepthState.h"
#include "..\Types.h"

namespace SoftwareRasterizer
{
    class RenderingContext;

    class OutputMergerStage
    {

    private:

        RenderingContext* _renderingContext;
        const RenderTarget* _renderTarget;
        const DepthState* _depthState;

    public:

        OutputMergerStage(RenderingContext* renderingContext);

        void execute(int x, int y, const Vector4& color, float depth);

    private:

        bool depthTest(float storedDepth, float depth);

        float readDepth(int x, int y) const;
        void writePixel(int x, int y, const Vector4& color, float depth);

    };
}
