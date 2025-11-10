
// ウィンドウ座標（ビューポート変換後）
//
//       +y                
//         |
//         |
//         +---- +x
//   (0, 0)
//
//
// Windows GUI の座標系
//
//   (0, 0)
//         +---- +x
//         |
//         |
//       +y                
//
//
// DIBのメモリを直接参照したとき座標系
//
//       +y                
//         |
//         |
//         +---- +x
//   (0, 0)
//

#include "FrameBuffer.h"
#include "..\Lib\Algorithm.h"
#include <cassert>
#include <algorithm>// fill

namespace SoftwareRasterizer
{
    void FrameBuffer::setFrameSize(int width, int height)
    {
        _freameWidth = width;
        _freameHeight = height;
    }

    void FrameBuffer::setColorBuffer(void* addr, size_t widthBytes)
    {
        _colorBuffer = addr;
        _colorBufferWidthBytes = widthBytes;
    }

    void FrameBuffer::setDepthBuffer(void* addr, size_t widthBytes)
    {
        _depthBuffer = addr;
        _depthBufferWidthBytes = widthBytes;
    }

    void FrameBuffer::setClearColor(float r, float g, float b, float a)
    {
        _clearColor = (Lib::DenormalizeByte(a) << 24) | (Lib::DenormalizeByte(r) << 16) | (Lib::DenormalizeByte(g) << 8) | Lib::DenormalizeByte(b);
    }

    void FrameBuffer::setClearDepth(float depth)
    {
        _clearDepth = depth;
    }

    void FrameBuffer::clearBuffer()
	{
        assert(0 <= _freameWidth);
        assert(0 <= _freameHeight);
        assert(nullptr != _colorBuffer);
        assert(_freameWidth <= _colorBufferWidthBytes);
        assert(nullptr != _depthBuffer);
        assert(_freameWidth <= _depthBufferWidthBytes);

        uint32_t* firstColor = (uint32_t*)_colorBuffer;
        uint32_t* lastColor = ((uint32_t*)_colorBuffer) + (_freameHeight * _freameWidth);
        std::fill(firstColor, lastColor, _clearColor);

        float* firstDepth = (float*)_depthBuffer;
        float* lastDepth = ((float*)_depthBuffer) + (_freameHeight * _freameWidth);
        std::fill(firstDepth, lastDepth, _clearDepth);
    }

    float FrameBuffer::readDepth(int x, int y) const
    {
        // x, y はウィンドウ相対座標

        if (0 <= x && x < _freameWidth || 0 <= y || y < _freameHeight)
        {
            size_t depthOffset = (_depthBufferWidthBytes * y) + (sizeof(float) * x);
            const float* depthSrc = (float*)(((uintptr_t)_depthBuffer) + depthOffset);
            return *depthSrc;
        }
        else
        {
            return _clearDepth;
        }
    }
    
    void FrameBuffer::writePixel(int x, int y, const Vector4& color, float depth)
    {
        // x, y はウィンドウ座標
        
        if (0 <= x && x < _freameWidth || 0 <= y || y < _freameHeight)
        {
            // DIBも左下が(0,0)なので上下反転は不要
            size_t colorOffset = (_colorBufferWidthBytes * y) + (sizeof(uint32_t) * x);
            uint32_t* colorDst = (uint32_t*)(((uintptr_t)_colorBuffer) + colorOffset);
            uint32_t r = Lib::DenormalizeByte(color.x);
            uint32_t g = Lib::DenormalizeByte(color.y);
            uint32_t b = Lib::DenormalizeByte(color.z);

            //r += (*colorDst >> 16) & 0xff;
            //g += (*colorDst >> 8) & 0xff;
            //b += (*colorDst >> 0) & 0xff;
            //r = std::min(r, 0xffu);
            //g = std::min(g, 0xffu);
            //b = std::min(b, 0xffu);

            *colorDst = (r << 16) | (g << 8) | (b);// BI_RGB

            size_t depthOffset = (_depthBufferWidthBytes * y) + (sizeof(float) * x);
            float* depthDst = (float*)(((uintptr_t)_depthBuffer) + (_depthBufferWidthBytes * y) + (sizeof(float) * x));
            *depthDst = depth;
        }
    }
}
