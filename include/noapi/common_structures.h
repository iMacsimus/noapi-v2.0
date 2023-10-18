#pragma once

#include <Image2d.h>

struct Framebuffer
{
    Framebuffer(LiteImage::Image2D<uint32_t> *color = nullptr, LiteImage::Image2D<float> *z = nullptr)
        : 
            color_buf(color),
            zbuf(z)
    {
    }
    LiteImage::Image2D<uint32_t> *color_buf = nullptr;
    LiteImage::Image2D<float> *zbuf = nullptr;
};