#include "TextureOperations.h" 
#include "..\..\Lib\Algorithm.h"
#include <algorithm>// clamp, fill
#include <cstdint>
#include <cassert>

namespace SoftwareRasterizer
{
    void TextureOperations::validate(const Texture2D* texture)
    {
        assert(nullptr != texture->addr);
        assert(0 <= texture->width);
        assert(0 <= texture->height);
        assert(texture->width <= texture->widthBytes);
    }

    void TextureOperations::fillColor(Texture2D* texture, const Vector4& clearColor)
    {
        uintptr_t addr = (uintptr_t)(texture->addr);
        size_t width = texture->width;
        size_t height = texture->height;
        size_t widthBytes = texture->widthBytes;
        size_t byteCount = 4;// TODO: rename

        float r = clearColor.x;
        float g = clearColor.y;
        float b = clearColor.z;
        float a = clearColor.w;

        uint32_t color = (Lib::DenormalizeByte(a) << 24) | (Lib::DenormalizeByte(r) << 16) | (Lib::DenormalizeByte(g) << 8) | Lib::DenormalizeByte(b);// BI_RGB

        // １行目
        uint32_t* first = (uint32_t*)texture->addr;
        uint32_t* last = ((uint32_t*)texture->addr) + texture->width;
        std::fill(first, last, color);

        // 残りの行は１行目をコピー
        for (int y = 1; y < height; y++)
        {
            uintptr_t src = addr;
            uintptr_t dst = addr + (widthBytes * y);
            std::memcpy((void*)dst, (const void*)src, widthBytes);
        }
    }
    
    void TextureOperations::fillDepth(Texture2D* texture, float clearDepth)
    {
        uintptr_t addr = (uintptr_t)(texture->addr);
        size_t width = texture->width;
        size_t height = texture->height;
        size_t widthBytes = texture->widthBytes;
        size_t byteCount = 4;// TODO: rename

        union {
            struct
            {
                uint32_t depth : 24;
                uint32_t stencil : 8;
            };
            uint32_t bytes;
        } tmp;

        float t = std::clamp(clearDepth, 0.0f, 1.0f);
        tmp.depth = (uint32_t)(0xffffff * t);
        tmp.stencil = 0;

        //tmp.depth = _clearDepth;

        // １行目
        for (int x = 0; x < width; x++)
        {
            uintptr_t src = (uintptr_t)(&tmp.bytes);
            uintptr_t dst = addr + (byteCount * x);
            std::memcpy((void*)dst, (const void*)src, byteCount);
        }

        // 残りの行は１行目をコピー
        for (int y = 1; y < height; y++)
        {
            uintptr_t src = addr;
            uintptr_t dst = addr + (widthBytes * y);
            std::memcpy((void*)dst, (const void*)src, widthBytes);
        }
    }

    Vector4 TextureOperations::fetchTexelColor(const Texture2D* texture, int tx, int ty)
    {
        uintptr_t addr = (uintptr_t)(texture->addr);
        size_t width = texture->width;
        size_t height = texture->height;
        size_t widthBytes = texture->widthBytes;
        size_t byteCount = sizeof(uint32_t);// TODO: rename

        if (tx < 0 || width <= tx || ty < 0 || height <= ty)
        {
            return Vector4::kZero;// TODO: wrap mode
        }

        struct RGBA
        {
            uint8_t r;
            uint8_t g;
            uint8_t b;
            uint8_t a;
        };

        const RGBA* texel = ((const RGBA*)addr) + ty * width + tx;
        Vector4 color(
            Lib::NormalizeByte(texel->r),
            Lib::NormalizeByte(texel->g),
            Lib::NormalizeByte(texel->b),
            Lib::NormalizeByte(texel->a)
        );
        return color;
    }

    float TextureOperations::fetchTexelDepth(const Texture2D* texture, int tx, int ty)
    {
        uintptr_t addr = (uintptr_t)(texture->addr);
        size_t width = texture->width;
        size_t height = texture->height;
        size_t widthBytes = texture->widthBytes;
        size_t byteCount = sizeof(uint32_t);// TODO: rename

        if (tx < 0 || width <= tx || ty < 0 || height <= ty)
        {
            return 0.0f;// TODO: wrap mode
        }

        size_t offset = (widthBytes * ty) + (byteCount * tx);
        uintptr_t src = addr + offset;

        union {
            struct
            {
                uint32_t depth : 24;
                uint32_t stencil : 8;
            };
            uint32_t uint32;
        } tmp;

        tmp.uint32 = *(uint32_t*)src;

        return ((float)tmp.depth) / ((float)0xffffff);

    }

    void TextureOperations::storeTexelColor(Texture2D* texture, int tx, int ty, const Vector4& color)
    {
        // x, y はウィンドウ座標

        // DIBは左下が(0,0)なので上下反転は不要

        uintptr_t addr = (uintptr_t)(texture->addr);
        size_t width = texture->width;
        size_t height = texture->height;
        size_t widthBytes = texture->widthBytes;
        size_t byteCount = sizeof(uint32_t);// TODO: rename

        if (tx < 0 || width <= tx || ty < 0 || height <= ty)
        {
            return;
        }

        size_t offset = (widthBytes * ty) + (byteCount * tx);
        uintptr_t dst = addr + offset;

        uint32_t r = Lib::DenormalizeByte(color.x);
        uint32_t g = Lib::DenormalizeByte(color.y);
        uint32_t b = Lib::DenormalizeByte(color.z);

        // color blend - add
        //r += (*dst >> 16) & 0xff;
        //g += (*dst >> 8) & 0xff;
        //b += (*dst >> 0) & 0xff;
        //r = std::min(r, 0xffu);
        //g = std::min(g, 0xffu);
        //b = std::min(b, 0xffu);

        uint32_t bytes = (r << 16) | (g << 8) | (b);// BI_RGB

        *(uint32_t*)dst = bytes;
    }

    void TextureOperations::storeTexelDepth(Texture2D* texture, int tx, int ty, float depth)
    {
        uintptr_t addr = (uintptr_t)(texture->addr);
        size_t width = texture->width;
        size_t height = texture->height;
        size_t widthBytes = texture->widthBytes;
        size_t byteCount = sizeof(uint32_t);// TODO: rename

        if (tx < 0 || width <= tx || ty < 0 || height <= ty)
        {
            return;
        }

        size_t offset = (widthBytes * ty) + (sizeof(float) * tx);
        uintptr_t dst = addr + offset;

        union {
            struct
            {
                uint32_t depth : 24;
                uint32_t stencil : 8;
            };
            uint32_t uint32;
        } tmp;


        tmp.depth = 0xffffff * std::clamp(depth, 0.0f, 1.0f);
        tmp.stencil = 0;

        *(uint32_t*)dst = tmp.uint32;
    }

}
