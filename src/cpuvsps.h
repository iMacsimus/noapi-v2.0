#pragma once
#include <vector>

#include <LiteMath.h>
#include <Image2d.h>

#include <common_structures.h>

template<typename Shader>
struct CPU_VSPS_Executor
{
    using VariablesData = typename Shader::VariablesData;
    using Uniforms = typename Shader::Uniforms;
    
    template<typename T>
    using allocator = std::allocator<T>;

    Uniforms uniforms;
    Uniforms& get_uniform_reference() { return uniforms; }

    struct TriangleSet
    {
        VariablesData variables[3];
        LiteMath::float4 p[3];
    };

    std::vector<TriangleSet> triangles;

    void run(Shader &shader)
    {   
        triangles.reserve(shader.indices_count);
        triangles.clear();
        auto *color_buffer = shader.fb.color_buf;
        auto *z_buffer = shader.fb.zbuf;
        uint width = (color_buffer) ? color_buffer->width() : z_buffer->width();
        uint height = (color_buffer) ? color_buffer->height() : z_buffer->height();

        for (uint32_t tindex = 0; tindex < shader.indices_count / 3; ++tindex) {
            triangles.push_back(TriangleSet());
            auto &cur = triangles.back();
            cur.variables[0] = shader.vertex_shader(shader.input_data, shader.indices[3*tindex+0], cur.p[0]);
            cur.variables[1] = shader.vertex_shader(shader.input_data, shader.indices[3*tindex+1], cur.p[1]);
            cur.variables[2] = shader.vertex_shader(shader.input_data, shader.indices[3*tindex+2], cur.p[2]);
        }

        for (auto &tr : triangles) {
            to_screen_space(tr.p[0], shader.viewport);
            to_screen_space(tr.p[1], shader.viewport);
            to_screen_space(tr.p[2], shader.viewport);

            BoundingBox2d box = bounding_box_2d(tr.p, shader.viewport);
            rasterize(tr, shader, shader.fb, box.xmax, box.xmin, box.ymax, box.ymin);
        }
    }
private:
    union BoundingBox2d 
    {  
        struct { uint32_t xmax; uint32_t xmin; uint32_t ymax; uint32_t ymin;  };
        uint32_t buf[4];
    };
    inline static void to_screen_space(LiteMath::float4 &pos, uint32_t viewport[4])
    {
        //to NDS
        float w = pos.w;
        pos /= w;
        //from DNS to screen space
        pos = (pos+1) * LiteMath::float4(viewport[2]-viewport[0], viewport[3]-viewport[1], 0, 0) / 2 + 
                LiteMath::float4(viewport[0], viewport[1], 0, 0);
        pos.z = 1.0f/w;
    }
    inline static BoundingBox2d
    bounding_box_2d(LiteMath::float4 p[3], uint viewport[4])
    {
        return 
        {
            LiteMath::clamp((uint)std::max(p[0].x, std::max(p[1].x, p[2].x)), viewport[0], viewport[2]-1), //xmax
            LiteMath::clamp((uint)std::min(p[0].x, std::min(p[1].x, p[2].x)), viewport[0], viewport[2]-1), //xmin
            LiteMath::clamp((uint)std::max(p[0].y, std::max(p[1].y, p[2].y)), viewport[1], viewport[3]-1), //ymax
            LiteMath::clamp((uint)std::min(p[0].y, std::min(p[1].y, p[2].y)), viewport[1], viewport[3]-1) //ymin
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
        if (1.0f / cur_z < 1.0f / (*z_buf)[LiteMath::uint2{x, y}]) {
            (*z_buf)[LiteMath::uint2{x, y}] = cur_z; //side effect here
            return true;
        }
        return false;
    }
    static void 
    rasterize(
        TriangleSet &tr, 
        Shader &shader,
        Framebuffer &fb,
        uint xmax, uint xmin,
        uint ymax, uint ymin)
    {
        float tr_square = edge_function(tr.p[0], tr.p[1], tr.p[2]);
        for (uint x = xmin; x <= xmax; ++x) {
            for (uint y = ymin; y <= ymax; ++y) {
                LiteMath::float4 point = { x+0.5f, y+0.5f, 0.0f, 0.0f };
                LiteMath::float3 barycentric = 
                {
                    edge_function(tr.p[1], tr.p[2], point),
                    edge_function(tr.p[2], tr.p[0], point),
                    edge_function(tr.p[0], tr.p[1], point),
                };
                barycentric /= tr_square;
                float interpolated_1_w = tr.p[0].z * barycentric[0] + tr.p[1].z * barycentric[1] + tr.p[2].z * barycentric[2];

                if (inside_of_triangle(barycentric) && 
                        z_test(x, y, interpolated_1_w, fb.zbuf)) {
                    VariablesData interpolated = shader.interpolate(tr.variables, tr.p, barycentric, interpolated_1_w);
                    LiteMath::float4 res = shader.pixel_shader(interpolated) * 255.0f;
                    if (fb.color_buf) {
                        uint inversed_y = fb.color_buf->height() - y;
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
    }
};