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
        virtual void set_viewport(uint32_t xstart, uint32_t ystart, uint32_t xend, uint32_t yend) {}
        virtual void set_uniform(void *uniforms_struct_ptr) = 0;
    protected:
        std::vector<void *> ptrs;
        uint32_t *indices;
    };
}