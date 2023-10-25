#pragma once
#include <vector>
#include <memory>

#include <noapi/integer_sequence.h>

#include <LiteMath.h>
#include <Image2d.h>

#include <noapi/IShader.h>

struct BoundingBox2d 
{  
    uint32_t xmax; 
    uint32_t xmin; 
    uint32_t ymax; 
    uint32_t ymin;
};

inline static LiteMath::float4 to_screen_space(const LiteMath::float4 &pos, const BoundingBox2d &viewport)
{
    LiteMath::float4 res(pos.x, pos.y, 0, 0);
    //to NDS
    res /= pos.w;
    //from DNS to screen space
    res = (res+1) * LiteMath::float4(viewport.xmax-viewport.xmin, viewport.ymax-viewport.ymin, 0, 0) / 2 + 
            LiteMath::float4(viewport.xmin, viewport.ymin, 1.0f/pos.w, 0);
    return res;
}

inline static BoundingBox2d
bounding_box_2d(LiteMath::float4 p[3], const BoundingBox2d &viewport)
{
    return 
    {
        LiteMath::clamp((uint)std::max(p[0].x, std::max(p[1].x, p[2].x)), viewport.xmin, viewport.xmax-1), //xmax
        LiteMath::clamp((uint)std::min(p[0].x, std::min(p[1].x, p[2].x)), viewport.xmin, viewport.xmax-1), //xmin
        LiteMath::clamp((uint)std::max(p[0].y, std::max(p[1].y, p[2].y)), viewport.ymin, viewport.ymax-1), //ymax
        LiteMath::clamp((uint)std::min(p[0].y, std::min(p[1].y, p[2].y)), viewport.ymin, viewport.ymax-1) //ymin
    };
}

inline static float
edge_function(const LiteMath::float4& a, const LiteMath::float4& b, const LiteMath::float4& p)
{
    return (p.x-a.x) * (b.y-a.y) - (p.y-a.y) * (b.x - a.x);
}

inline static bool
inside_of_triangle(LiteMath::float3 &barycentric)
{
    return barycentric[0] >= 0 && barycentric[1] >= 0 && barycentric[2] >= 0;
}

inline static bool 
z_test( //change z_buffer side effect
        uint x, uint y, 
        float cur_z,
        LiteImage::Image2D<float> *z_buf)
{
    if (!z_buf) return true;
    if (cur_z > (*z_buf)[LiteMath::uint2{x, y}]) {
        (*z_buf)[LiteMath::uint2{x, y}] = cur_z; //side effect here
        return true;
    }
    return false;
}

template<typename Shader>
struct ShaderWrapper : IShader
{
public:
    using VariablesData = typename Shader::VariablesData;
    using Uniforms = typename Shader::Uniforms;
    using InputData = typename Shader::InputData; 
    struct TriangleSet
    {
        VariablesData variables[3];
    };
public:
    Uniforms uniforms;
    BoundingBox2d viewport;
    std::vector<TriangleSet> triangles;
public:
    template<typename... Args>
    static constexpr size_t 
    ptrs_count(InputData(*)(uint32_t, Args*...))
    {
        return sizeof...(Args);
    }
    template<typename... Args, size_t... indices>
    InputData fetch_helper(uint32_t i, InputData(*)(uint32_t, Args*...), noapi::index_sequence<indices...>)
    {
        return Shader::vertex_fetch(i, (Args*)ptrs[indices]...);
    }
    InputData fetch(uint32_t i)
    {
        return fetch_helper(i, Shader::vertex_fetch, noapi::make_index_sequence<ptrs_count(Shader::vertex_fetch)>());
    }
public:
    void set_viewport(uint32_t xstart, uint32_t ystart, uint32_t xend, uint32_t yend) override
    {
        viewport = { xend, xstart, yend, ystart };
    }
    void set_uniform(void *uniform_struct_ptr) override 
    {
        uniforms = *(Uniforms*)uniform_struct_ptr;
    }
    void draw_triangles(size_t elements_count, Framebuffer fb) override
    {
        triangles.reserve(std::max(triangles.capacity(), elements_count));
        triangles.clear();
        
        auto *color_buffer = fb.color_buf;
        auto *z_buffer = fb.zbuf;
        uint width = (color_buffer) ? color_buffer->width() : z_buffer->width();
        uint height = (color_buffer) ? color_buffer->height() : z_buffer->height();

        for (uint32_t tindex = 0; tindex < elements_count / 3; ++tindex) {
            triangles.push_back({});
            auto &cur = triangles.back();
            cur.variables[0] = Shader::vertex_shader(fetch(indices[3*tindex+0]), uniforms);
            cur.variables[1] = Shader::vertex_shader(fetch(indices[3*tindex+1]), uniforms);
            cur.variables[2] = Shader::vertex_shader(fetch(indices[3*tindex+2]), uniforms);
        }
        for (auto &tr : triangles) {
            LiteMath::float4 sspos[3] = 
            {
                to_screen_space(tr.variables[0].vPos, viewport),
                to_screen_space(tr.variables[1].vPos, viewport),
                to_screen_space(tr.variables[2].vPos, viewport),
            };
            rasterize(tr.variables, sspos, fb, bounding_box_2d(sspos, viewport));
        }
    }
public:
    void 
    rasterize(
        VariablesData vd[3],
        const LiteMath::float4 sspos[3], 
        Framebuffer &fb,
        const BoundingBox2d &bb)
    {
        float tr_square = edge_function(sspos[0], sspos[1], sspos[2]);
        for (uint x = bb.xmin; x <= bb.xmax; ++x) {
            for (uint y = bb.ymin; y <= bb.ymax; ++y) {
                LiteMath::float4 point = { x+0.5f, y+0.5f, 0.0f, 0.0f };
                LiteMath::float3 barycentric = 
                {
                    edge_function(sspos[1], sspos[2], point),
                    edge_function(sspos[2], sspos[0], point),
                    edge_function(sspos[0], sspos[1], point),
                };
                barycentric /= tr_square;
                float interpolated_1_w = sspos[0].z * barycentric[0] + sspos[1].z * barycentric[1] + sspos[2].z * barycentric[2];

                if (inside_of_triangle(barycentric)
                        && z_test(x, y, interpolated_1_w, fb.zbuf) 
                        && fb.color_buf) {
                    uint inversed_y = fb.color_buf->height() - y;
                    VariablesData interpolated = Shader::interpolate(vd, barycentric, interpolated_1_w);
                    LiteMath::float4 res = Shader::pixel_shader(interpolated, uniforms) * 255.0f;
                    (*fb.color_buf)[LiteMath::uint2{x, inversed_y}] = LiteMath::uchar4
                    { 
                        (LiteMath::uchar)res.x, 
                        (LiteMath::uchar)res.y, 
                        (LiteMath::uchar)res.z, 
                        (LiteMath::uchar)res.w 
                    }.u32;
                }
            }
        }
    }
};

template<typename Shader>
std::shared_ptr<IShader> make_cpu_vsps_shader(const char *impl_path)
{
    Shader(); // to define static methods
    return std::make_shared<ShaderWrapper<Shader>>();
};