#pragma once

#include "..\State\RenderTarget.h"
#include "..\State\DepthState.h"
#include "..\Types.h"

namespace SoftwareRasterizer
{
    class OutputMergerStage
    {

    private:

        const RenderTarget* _renderTarget = nullptr;
        const DepthState* _depthState = nullptr;

        class RenderingContext* _renderingContext = nullptr;

    public:

        OutputMergerStage();

        void input(const RenderTarget* renderTarget) { _renderTarget = renderTarget; }
        void input(const DepthState* depthState) { _depthState = depthState; }

        void ouput(class RenderingContext* renderingContext) { _renderingContext = renderingContext; }

        void execute(int x, int y, const Vector4& color, float depth);

    private:

        bool depthTest(float storedDepth, float depth);

        float readDepth(int x, int y) const;
        void writePixel(int x, int y, const Vector4& color, float depth);

    };
}
