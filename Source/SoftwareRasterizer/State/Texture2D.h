#pragma once

#include "..\Core\Types.h"

namespace SoftwareRasterizer
{

    struct Texture2D
    {
        const void* addr;
        int width;
        int height;
        int widthBytes;

        //internalformat = 4
        //format = GL_RGBA
    };

}
