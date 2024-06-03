#ifndef NOAPI_INCLUDE_COMMON_STRUCTURES_H
# define NOAPI_INCLUDE_COMMON_STRUCTURES_H

#include <Image2d.h>

namespace noapi
{

struct Framebuffer
{
  Framebuffer(
      LiteImage::Image2D<uint32_t> *color = nullptr, 
      LiteImage::Image2D<float> *z = nullptr): color_buf(color), zbuf(z) {}
  LiteImage::Image2D<uint32_t> *color_buf = nullptr;
  LiteImage::Image2D<float> *zbuf = nullptr;
};

enum CullingMode
{
  CULL_BACK = 0,
  CULL_FRONT = 1,
  DO_NOT_CULL = 2
};

enum { OFF, ON };

} // namespace noapi

#endif