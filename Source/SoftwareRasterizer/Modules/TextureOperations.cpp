#include "TextureOperations.h" 
#include "NormalizedConverter.h"
#include <cstdint>
#include <cstring>//memcpy
#include <cassert>//assert

namespace SoftwareRasterizer
{
    struct ColorR8G8B8A8
    {
        uint8_t r;
        uint8_t g;
        uint8_t b;
        uint8_t a;
    };

    struct ColorB8G8R8A8// = DIB BI_RGB
    {
        uint8_t b;
        uint8_t g;
        uint8_t r;
        uint8_t a;
    };

    struct DepthStencilD24S8
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

    void TextureOperations::FillTextureColor(Texture2D* texture, const Vector4& color)
    {
        uintptr_t addr = (uintptr_t)(texture->addr);
        size_t width = texture->width;
        size_t height = texture->height;
        size_t widthBytes = texture->widthBytes;// TODO: rename
        size_t byteCount = 4;// TODO: rename

        ColorB8G8R8A8 texel;
        texel.b = NormalizedConverter::DenormalizeU8(color.z);
        texel.g = NormalizedConverter::DenormalizeU8(color.y);
        texel.r = NormalizedConverter::DenormalizeU8(color.x);
        texel.a = NormalizedConverter::DenormalizeU8(color.w);

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
    
    void TextureOperations::FillTextureDepth(Texture2D* texture, float depth)
    {
        uintptr_t addr = (uintptr_t)(texture->addr);
        size_t width = texture->width;
        size_t height = texture->height;
        size_t widthBytes = texture->widthBytes;// TODO: rename
        size_t byteCount = 4;// TODO: rename

        uint32_t d24 = NormalizedConverter::DenormalizeU24(depth);

        DepthStencilD24S8 texel;
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

    Vector4 TextureOperations::FetchTexelColor(const Texture2D* texture, const IntVector2& texelCoord)
    {
        uintptr_t addr = (uintptr_t)(texture->addr);
        size_t width = texture->width;
        size_t height = texture->height;
        size_t widthBytes = texture->widthBytes;
        size_t texelBytes = 4;

        if (texelCoord.x < 0 || width <= texelCoord.x ||
            texelCoord.y < 0 || height <= texelCoord.y)
        {
            return Vector4::kZero;
        }

        size_t offset = (widthBytes * texelCoord.y) + (texelBytes * texelCoord.x);
        uintptr_t src = addr + offset;

        ColorR8G8B8A8 texel = *(const ColorR8G8B8A8*)src;

        Vector4 color(
            NormalizedConverter::NormalizeU8(texel.r),
            NormalizedConverter::NormalizeU8(texel.g),
            NormalizedConverter::NormalizeU8(texel.b),
            NormalizedConverter::NormalizeU8(texel.a)
        );

        return color;
    }

    float TextureOperations::FetchTexelDepth(const Texture2D* texture, const IntVector2& texelCoord)
    {
        uintptr_t addr = (uintptr_t)(texture->addr);
        size_t width = texture->width;
        size_t height = texture->height;
        size_t widthBytes = texture->widthBytes;
        size_t texelBytes = 4;

        if (texelCoord.x < 0 || width <= texelCoord.x ||
            texelCoord.y < 0 || height <= texelCoord.y)
        {
            return 0.0f;
        }

        size_t offset = (widthBytes * texelCoord.y) + (texelBytes * texelCoord.x);
        uintptr_t src = addr + offset;

        DepthStencilD24S8 texel = *(const DepthStencilD24S8*)src;
        float depth = NormalizedConverter::NormalizeU24(texel.depth);
        return depth;

    }

    void TextureOperations::StoreTexelColor(Texture2D* texture, const IntVector2& texelCoord, const Vector4& color)
    {
        // x, y はウィンドウ座標
        // DIBは左下が(0,0)なので上下反転は不要

        uintptr_t addr = (uintptr_t)(texture->addr);
        size_t width = texture->width;
        size_t height = texture->height;
        size_t widthBytes = texture->widthBytes;
        size_t texelBytes = 4;

        if (texelCoord.x < 0 || width <= texelCoord.x ||
            texelCoord.y < 0 || height <= texelCoord.y)
        {
            return;
        }

        size_t offset = (widthBytes * texelCoord.y) + (texelBytes * texelCoord.x);
        uintptr_t dst = addr + offset;

        ColorB8G8R8A8 texel;
        texel.b = NormalizedConverter::DenormalizeU8(color.z);
        texel.g = NormalizedConverter::DenormalizeU8(color.y);
        texel.r = NormalizedConverter::DenormalizeU8(color.x);
        texel.a = NormalizedConverter::DenormalizeU8(color.w);

        *(ColorB8G8R8A8*)dst = texel;
    }

    void TextureOperations::StoreTexelDepth(Texture2D* texture, const IntVector2& texelCoord, float depth)
    {
        uintptr_t addr = (uintptr_t)(texture->addr);
        size_t width = texture->width;
        size_t height = texture->height;
        size_t widthBytes = texture->widthBytes;
        size_t byteCount = sizeof(uint32_t);// TODO: rename

        if (texelCoord.x < 0 || width <= texelCoord.x ||
            texelCoord.y < 0 || height <= texelCoord.y)
        {
            return;
        }

        size_t offset = (widthBytes * texelCoord.y) + (sizeof(float) * texelCoord.x);
        uintptr_t dst = addr + offset;

        DepthStencilD24S8 texel;
        texel.depth = NormalizedConverter::DenormalizeU24(depth);
        texel.stencil = 0;

        *(DepthStencilD24S8*)dst = texel;
    }

}
