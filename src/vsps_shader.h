#pragma once
#include <memory>
#include <cstring>

#include <LiteMath.h>

#include <common_structures.h>

template<
    typename InputDataClass,
    typename VariablesDataClass,
    typename UniformsClass,
    template<typename> typename Executor, 
    template<typename> typename Allocator
> struct VSPS_Shader
{
    VSPS_Shader() : uniforms(exec.get_uniform_reference()) {}

    using InputData = InputDataClass;
    using Uniforms = UniformsClass;
    using VariablesData = VariablesDataClass;

    Executor<VSPS_Shader> exec;

    Allocator<InputData> allocator;
    Allocator<uint32_t> indices_allocator;
    
    InputData *input_data = nullptr;
    size_t elements_count = 0;
    bool to_clear_data = true;

    uint32_t *indices = nullptr;
    size_t indices_count = 0;
    bool to_clear_indices = true;

    Framebuffer fb;
    uint32_t viewport[4] = { 0, 0, 0, 0 };

    Uniforms &uniforms;

    template<typename... PtrTypes>
    InputData* make_data(size_t elements_count, PtrTypes... ptrs)
    {
        InputData *arr = allocator.allocate(elements_count);
        for (int i = 0; i < elements_count; ++i) {
            allocator.construct(arr+i, ptrs[i]...);
        }
        return arr;
    }

    void bind_data(InputData *input_data, size_t elements_count, bool to_clear_data = true)
    {
        if (to_clear_data) {
            clear_data(this->input_data, this->elements_count);
        }

        this->to_clear_data = to_clear_data;
        this->elements_count = elements_count;
        this->input_data = input_data;
    }

    template<typename... PtrTypes>
    void make_and_bind_data(size_t elements_count, PtrTypes... ptrs)
    {
        if (to_clear_data) {
            clear_data(this->input_data, this->elements_count);
        }

        input_data = make_data(elements_count, ptrs...);
        to_clear_data = true;
        this->elements_count = elements_count;
    }

    void clear_data(InputData *data, size_t count)
    {
        allocator.deallocate(data, count);
    }

    void set_viewport(uint32_t xstart, uint32_t ystart, uint32_t xend, uint32_t yend)
    {
        viewport[0] = xstart;
        viewport[1] = ystart;
        viewport[2] = xend;
        viewport[3] = yend;
    }

    uint32_t *make_index_array(size_t elements_count, uint32_t *indices = nullptr)
    {
        uint32_t *res = indices_allocator.allocate(elements_count);
        if (indices) {
            memcpy(res, indices, elements_count * sizeof(uint32_t));
        }
        return res;
    }

    void bind_index_array(size_t elements_count, uint32_t *indices = nullptr, bool to_clear_index_array=true)
    {
        if (to_clear_index_array) { clear_index_arr(this->indices, indices_count); }
        this->indices = indices;
        this->indices_count = elements_count;
        to_clear_index_array = to_clear_index_array;
    }

    void make_and_bind_index_arr(size_t elements_count, uint32_t *indices = nullptr)
    {
        if (to_clear_indices) { clear_index_arr(this->indices, indices_count); }
        this->indices = make_index_array(elements_count, indices);
        this->indices_count = elements_count;
        this->to_clear_indices = true;
    }

    void clear_index_arr(uint32_t *indices, size_t count) { indices_allocator.deallocate(indices, count); }

    void bind_framebuffer(Framebuffer fb) { this->fb = fb; }

    virtual VariablesData vertex_shader(const InputData *input, uint32_t index, LiteMath::float4 &vertexPosition) = 0;
    virtual LiteMath::float4 pixel_shader(VariablesData input) { return LiteMath::float4(0.0f); }
    virtual VariablesData interpolate(
            const VariablesData data[3], 
            const LiteMath::float4 points[3], 
            const LiteMath::float3 &barycentric,
            float interpolated_1_w) = 0;

    void draw()
    {
        exec.run(*this);
    }

    ~VSPS_Shader()
    {
        if (to_clear_data) clear_data(input_data, elements_count);
        if (to_clear_indices) clear_index_arr(indices, indices_count);
    }
};