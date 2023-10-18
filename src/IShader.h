#pragma once
#include <vector>

#include <common_structures.h>

class IShader
{
public:
    template<typename... Args>
    void set_vertex_arrays(Args*... ptrs)
    {
        this->ptrs = { (void*)ptrs... };
    }
    void set_index_array(uint32_t *indices, size_t elements_count)
    {
        this->indices = indices;
        this->elements_count = elements_count;
    }
    virtual void draw(Framebuffer fb) = 0;
    virtual void set_viewport(uint32_t xstart, uint32_t ystart, uint32_t xend, uint32_t yend) {}
    virtual void set_uniform(void *uniforms_struct_ptr) = 0;
protected:
    std::vector<void *> ptrs;
    uint32_t *indices;
    size_t elements_count;
};