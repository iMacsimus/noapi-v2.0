#pragma once
#include <vector>

#include <noapi/common_structures.h>

namespace noapi
{
    class IShader
    {
    public:
        template<typename... Args>
        void set_vertex_arrays(Args*... ptrs)
        {
            this->ptrs = { (void*)ptrs... };
        }
        void set_index_array(uint32_t *indices)
        {
            this->indices = indices;
        }
        virtual void draw_triangles(size_t triangles_count, Framebuffer fb) = 0;
        virtual void set_viewport(int32_t xstart, int32_t ystart, int32_t xend, int32_t yend) = 0;
        virtual void set_culling(CullingMode mode) = 0;
        virtual void set_uniform(void *uniforms_struct_ptr) = 0;
        virtual void set_clipping(bool enable) = 0;
    public:
        virtual ~IShader() {}
    protected:
        std::vector<void *> ptrs;
        uint32_t *indices;
    };
}