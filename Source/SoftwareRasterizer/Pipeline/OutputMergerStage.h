#pragma once

#include "..\State\DepthState.h"
#include "..\State\DepthRange.h"
#include "..\State\RenderTarget.h"
#include "..\Types.h"

namespace SoftwareRasterizer
{
    class OutputMergerStage
    {

    private:

        const DepthState* _depthState = nullptr;
        const DepthRange* _depthRange = nullptr;

        RenderTarget* _renderTarget = nullptr;

    public:

        OutputMergerStage();

        void input(const DepthState* depthState) { _depthState = depthState; }
        void input(const DepthRange* depthRange) { _depthRange = depthRange; }

        void ouput(RenderTarget* renderTarget) { _renderTarget = renderTarget; }

        void execute(int x, int y, const Vector4& color, float depth);

    private:

        float fetchTexelDepth(int x, int y) const;

        bool depthTest(float storedDepth, float depth);

        void storeTexelColor(int x, int y, const Vector4& color);
        void storeTexelDepth(int x, int y, float depth);
        
    };
}
