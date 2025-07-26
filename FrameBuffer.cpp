#include "FrameBuffer.h"
#include "Algorithm.h"
#include <cassert>
#include <memory>// memset
#include <algorithm>// fill

namespace MyApp
{
    void FrameBuffer::setFrameSize(int width, int height)
    {
        _freameWidth = width;
        _freameHeight = height;
    }

    void FrameBuffer::setRenderColorBuffer(void* addr, size_t widthBytes)
    {
        assert(_freameWidth <= widthBytes);
        colorFrameBuffer.addr = addr;
        colorFrameBuffer.widthBytes = widthBytes;
    }

    void FrameBuffer::setRenderDepthBuffer(void* addr, size_t widthBytes)
    {
        assert(_freameWidth <= widthBytes);
        depthFrameBuffer.addr = addr;
        depthFrameBuffer.widthBytes = widthBytes;
    }

    void FrameBuffer::clearRenderBuffer()
	{
        uint32_t clearColor = 0;// glClearColor(r, g, b, a) (Default all 0)
        float clearDepth = 1.0f; // glClearDepth(depth) (Default 1)

        // glClear()
        if (0 == clearColor)
        {
            size_t size = colorFrameBuffer.widthBytes * _freameHeight;
            std::memset(colorFrameBuffer.addr, 0, size);
        }
        else
        {
            // TODO
            uint32_t* firstColor = (uint32_t*)colorFrameBuffer.addr;
            uint32_t* lastColor = ((uint32_t*)colorFrameBuffer.addr) + (_freameHeight * _freameWidth);
            std::fill(firstColor, lastColor, clearColor);
        }


        {
            float* firstDepth = (float*)depthFrameBuffer.addr;
            float* lastDepth = ((float*)depthFrameBuffer.addr) + (_freameHeight * _freameWidth);
            std::fill(firstDepth, lastDepth, clearDepth);
        }
    }

    float FrameBuffer::readDepth(int x, int y) const
    {
        size_t depthOffset = (depthFrameBuffer.widthBytes * y) + (sizeof(float) * x);
        assert(0 <= depthOffset && depthOffset < (depthFrameBuffer.widthBytes * _freameHeight));
        if (0 <= depthOffset && depthOffset < (depthFrameBuffer.widthBytes * _freameHeight))
        {
            const float* depthSrc = (float*)(((uintptr_t)(depthFrameBuffer.addr)) + depthOffset);
            return *depthSrc;
        }

        float clearDepth = 1.0f; // glClearDepth(depth) (Default 1)
        return clearDepth;
    }

    void FrameBuffer::writeColorAndDepth(int x, int y, const Vector4& color, float depth)
    {
        // x, y はウィンドウ座標系、DIBも左下が(0,0)なので上下反転は不要
        size_t colorOffset = (colorFrameBuffer.widthBytes * y) + (sizeof(uint32_t) * x);
        assert(0 <= colorOffset && colorOffset < (colorFrameBuffer.widthBytes * _freameHeight));
        if (0 <= colorOffset && colorOffset < (colorFrameBuffer.widthBytes * _freameHeight))
        {
            uint32_t* colorDst = (uint32_t*)(((uintptr_t)(colorFrameBuffer.addr)) + colorOffset);
            uint32_t r = denormalizeByte(color.x);
            uint32_t g = denormalizeByte(color.y);
            uint32_t b = denormalizeByte(color.z);
            *colorDst = (r << 16) | (g << 8) | (b);
        }

        size_t depthOffset = (depthFrameBuffer.widthBytes * y) + (sizeof(float) * x);
        assert(0 <= depthOffset && depthOffset < (depthFrameBuffer.widthBytes * _freameHeight));
        if (0 <= depthOffset && depthOffset < (depthFrameBuffer.widthBytes * _freameHeight))
        {
            float* depthDst = (float*)(((uintptr_t)(depthFrameBuffer.addr)) + (depthFrameBuffer.widthBytes * y) + (sizeof(float) * x));
            *depthDst = depth;
        }
    }
}
