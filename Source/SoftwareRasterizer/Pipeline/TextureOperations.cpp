#include "TextureOperations.h" 
#include "..\..\Lib\Algorithm.h"
#include <algorithm>// clamp, fill
#include <cstdint>
#include <cassert>

namespace SoftwareRasterizer
{
    struct R8G8B8A8
    {
        uint8_t r;
        uint8_t g;
        uint8_t b;
        uint8_t a;
    };

    struct B8G8R8A8// = DIB BI_RGB
    {
        uint8_t b;
        uint8_t g;
        uint8_t r;
        uint8_t a;
    };

    struct D24S8
    {
        uint32_t depth : 24;
        uint32_t stencil : 8;
    };

    void TextureOperations::validate(const Texture2D* texture)
    {
        assert(nullptr != texture->addr);
        assert(0 <= texture->width);
        assert(0 <= texture->height);
        assert(texture->width <= texture->widthBytes);
    }

    void TextureOperations::fillColor(Texture2D* texture, const Vector4& color)
    {
        uintptr_t addr = (uintptr_t)(texture->addr);
        size_t width = texture->width;
        size_t height = texture->height;
        size_t widthBytes = texture->widthBytes;// TODO: rename
        size_t byteCount = 4;// TODO: rename

        B8G8R8A8 texel;
        texel.b = Lib::DenormalizeByte(color.z);
        texel.g = Lib::DenormalizeByte(color.y);
        texel.r = Lib::DenormalizeByte(color.x);
        texel.a = Lib::DenormalizeByte(color.w);

        // １行目
        {
            uintptr_t src = (uintptr_t)&texel;
            uintptr_t dst = addr;
            for (int x = 0; x < width; x++)
            {
                std::memcpy((void*)dst, (const void*)src, byteCount);
                dst += byteCount;
            }
        }

        // 残りの行は１行目をコピー
        {
            uintptr_t src = addr;
            uintptr_t dst = addr + widthBytes;
            for (int y = 1; y < height; y++)
            {
                std::memcpy((void*)dst, (const void*)src, widthBytes);
                dst += widthBytes;
            }
        }
    }
    
    void TextureOperations::fillDepth(Texture2D* texture, float depth)
    {
        uintptr_t addr = (uintptr_t)(texture->addr);
        size_t width = texture->width;
        size_t height = texture->height;
        size_t widthBytes = texture->widthBytes;// TODO: rename
        size_t byteCount = 4;// TODO: rename

        float t = std::clamp(depth, 0.0f, 1.0f);
        uint32_t d24 = (uint32_t)(0xffffff * t);

        D24S8 texel;
        texel.depth = d24;
        texel.stencil = 0;

        // １行目
        {
            uintptr_t src = (uintptr_t)&texel;
            uintptr_t dst = addr;
            for (int x = 0; x < width; x++)
            {
                std::memcpy((void*)dst, (const void*)src, byteCount);
                dst += byteCount;
            }
        }

        // 残りの行は１行目をコピー
        {
            uintptr_t src = addr;
            uintptr_t dst = addr + widthBytes;
            for (int y = 1; y < height; y++)
            {
                std::memcpy((void*)dst, (const void*)src, widthBytes);
                dst += widthBytes;
            }
        }
    }

    Vector4 TextureOperations::fetchTexelColor(const Texture2D* texture, int tx, int ty)
    {
        uintptr_t addr = (uintptr_t)(texture->addr);
        size_t width = texture->width;
        size_t height = texture->height;
        size_t widthBytes = texture->widthBytes;
        size_t byteCount = 4;// TODO: rename

        if (tx < 0 || width <= tx || ty < 0 || height <= ty)
        {
            return Vector4::kZero;// TODO: wrap mode
        }

        size_t offset = (widthBytes * ty) + (byteCount * tx);
        uintptr_t src = addr + offset;

        R8G8B8A8 texel = *(const R8G8B8A8*)src;

        Vector4 color(
            Lib::NormalizeByte(texel.r),
            Lib::NormalizeByte(texel.g),
            Lib::NormalizeByte(texel.b),
            Lib::NormalizeByte(texel.a)
        );

        return color;
    }

    float TextureOperations::fetchTexelDepth(const Texture2D* texture, int tx, int ty)
    {
        uintptr_t addr = (uintptr_t)(texture->addr);
        size_t width = texture->width;
        size_t height = texture->height;
        size_t widthBytes = texture->widthBytes;
        size_t byteCount = 4;// TODO: rename

        if (tx < 0 || width <= tx || ty < 0 || height <= ty)
        {
            return 0.0f;// TODO: wrap mode
        }

        size_t offset = (widthBytes * ty) + (byteCount * tx);
        uintptr_t src = addr + offset;

        D24S8 texel = *(const D24S8*)src;
        float depth = ((float)texel.depth) / ((float)0xffffff);
        return depth;

    }

    void TextureOperations::storeTexelColor(Texture2D* texture, int tx, int ty, const Vector4& color)
    {
        // x, y はウィンドウ座標
        // DIBは左下が(0,0)なので上下反転は不要

        uintptr_t addr = (uintptr_t)(texture->addr);
        size_t width = texture->width;
        size_t height = texture->height;
        size_t widthBytes = texture->widthBytes;
        size_t byteCount = 4;// TODO: rename

        if (tx < 0 || width <= tx || ty < 0 || height <= ty)
        {
            return;
        }

        size_t offset = (widthBytes * ty) + (byteCount * tx);
        uintptr_t dst = addr + offset;

        B8G8R8A8 texel;
        texel.b = Lib::DenormalizeByte(color.z);
        texel.g = Lib::DenormalizeByte(color.y);
        texel.r = Lib::DenormalizeByte(color.x);
        texel.a = Lib::DenormalizeByte(color.w);

        *(B8G8R8A8*)dst = texel;
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

        float t = std::clamp(depth, 0.0f, 1.0f);
        uint32_t d24 = (uint32_t)(0xffffff * t);

        D24S8 texel;
        texel.depth = d24;
        texel.stencil = 0;

        *(D24S8*)dst = texel;
    }

}
