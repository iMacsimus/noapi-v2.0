#pragma once
#include <vector>

#include <noapi/common_structures.h>

namespace noapi
{

struct IShader
{
  virtual ~IShader() {}
  virtual void draw_triangles(size_t triangles_count, 
                              Framebuffer fb) = 0;
  virtual void set_viewport(int32_t xstart, int32_t ystart, 
                            int32_t xend, int32_t yend) = 0;
  virtual void set_culling(CullingMode mode) = 0;
  virtual void set_clipping(bool enable) = 0;
};

} // namespace noapi