#pragma once

#include "..\Core\Types.h"

namespace SoftwareRasterizer
{

    struct Texture2D
    {
        const void* addr = nullptr;
        int width = 0;
        int height = 0;
        int widthBytes = 0;

        //internalformat = 4
        //format = GL_RGBA
    };

}
