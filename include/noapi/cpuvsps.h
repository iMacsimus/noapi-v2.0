#pragma once
#include <vector>
#include <memory>
#include <iostream>

#include <noapi/IShader.h>
#include <noapi/vsps_clipping.h>

#include <LiteMath.h>
#include <Image2d.h>

namespace noapi
{
    struct BoundingBox2d 
    {  
        int32_t xmax; 
        int32_t xmin; 
        int32_t ymax; 
        int32_t ymin;
    };

    inline LiteMath::float4 to_screen_space(const LiteMath::float4 &pos, const BoundingBox2d &viewport)
    {
        LiteMath::float4 res(pos.x, pos.y, 0, 0);
        //to NDS
        res /= pos.w;
        //from DNS to screen space
        res = (res+1) * LiteMath::float4((float)(viewport.xmax-viewport.xmin), (float)(viewport.ymax-viewport.ymin), 0.0f, 0.0f) / 2.0f + 
                LiteMath::float4((float)viewport.xmin, (float)viewport.ymin, 1.0f/pos.w, 0.0f);
        return res;
    }

    inline BoundingBox2d
    bounding_box_2d(LiteMath::float4 p[3], const BoundingBox2d &viewport)
    {
        return 
        {
            LiteMath::clamp((int32_t)std::max(p[0].x, std::max(p[1].x, p[2].x)), viewport.xmin, viewport.xmax-1), //xmax
            LiteMath::clamp((int32_t)std::min(p[0].x, std::min(p[1].x, p[2].x)), viewport.xmin, viewport.xmax-1), //xmin
            LiteMath::clamp((int32_t)std::max(p[0].y, std::max(p[1].y, p[2].y)), viewport.ymin, viewport.ymax-1), //ymax
            LiteMath::clamp((int32_t)std::min(p[0].y, std::min(p[1].y, p[2].y)), viewport.ymin, viewport.ymax-1) //ymin
        };
    }

    inline float
    edge_function(const LiteMath::float4& a, const LiteMath::float4& b, const LiteMath::float4& p)
    {
        return (p.y-a.y) * (b.x - a.x) - (p.x-a.x) * (b.y-a.y);
    }

    inline bool
    inside_of_triangle(LiteMath::float3 &barycentric)
    {
        return barycentric[0] >= 0 && barycentric[1] >= 0 && barycentric[2] >= 0;
    }

    inline bool 
    z_test( //change z_buffer side effect
            uint32_t x, uint32_t y, 
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
        std::shared_ptr<Shader> m_pShaderObj;
    public:
        ShaderWrapper(std::shared_ptr<Shader> a_pShader) : m_pShaderObj(a_pShader) {}
        using VariablesData = typename Shader::VariablesData;
        struct TriangleSet
        {
            VariablesData variables[3];
        };
    public:
        BoundingBox2d viewport;
        std::vector<TriangleSet> triangles;
        std::vector<TriangleSet> addtitional;
        bool clipping_enabled = false;
        CullingMode culling_mode = DO_NOT_CULL;
    public:
        void set_viewport(int32_t xstart, int32_t ystart, int32_t xend, int32_t yend) override
        {
            viewport = { xend, xstart, yend, ystart };
        }
        void set_culling(CullingMode mode) override
        {
            culling_mode = mode;
        }
        void set_clipping(bool enable) override
        {
            clipping_enabled = enable;
        }
        void draw_triangles(size_t elements_count, Framebuffer fb) override
        {
            triangles.reserve(std::max(triangles.capacity(), elements_count/3));
            addtitional.reserve(std::max(addtitional.capacity(), elements_count/3*2));
            triangles.clear();
            addtitional.clear();
            
            auto *color_buffer = fb.color_buf;
            auto *z_buffer = fb.zbuf;

            for (uint32_t tindex = 0; tindex < elements_count / 3; ++tindex) {
                triangles.push_back({});
                auto &cur = triangles.back();

                cur.variables[0] = m_pShaderObj->vertex_shader(m_pShaderObj->vertex_fetch(3*tindex+0));
                cur.variables[1] = m_pShaderObj->vertex_shader(m_pShaderObj->vertex_fetch(3*tindex+1));
                cur.variables[2] = m_pShaderObj->vertex_shader(m_pShaderObj->vertex_fetch(3*tindex+2));
            }

            if (clipping_enabled) {
                for (auto &tr : triangles) {
                    auto res = vs_to_ts(clip<Shader>(tr.variables, m_pShaderObj.get()));
                    addtitional.insert(addtitional.end(), res.begin(), res.end());
                }
                triangles = addtitional;
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
            float I[3] =
            {
                sspos[1].y - sspos[2].y,
                sspos[2].y - sspos[0].y,
                sspos[0].y - sspos[1].y
            };
            float J[3] =
            {
                sspos[2].x - sspos[1].x,
                sspos[0].x - sspos[2].x,
                sspos[1].x - sspos[0].x
            };
            float K[3] =
            {
                sspos[1].x*sspos[2].y - sspos[1].y*sspos[2].x,
                sspos[2].x*sspos[0].y - sspos[2].y*sspos[0].x,
                sspos[0].x*sspos[1].y - sspos[0].y*sspos[1].x
            };
            float Cy1 = I[0]*float(bb.xmin+0.5f)+J[0]*float(bb.ymin+0.5f)+K[0];
            float Cy2 = I[1]*float(bb.xmin+0.5f)+J[1]*float(bb.ymin+0.5f)+K[1];
            float Cy3 = I[2]*float(bb.xmin+0.5f)+J[2]*float(bb.ymin+0.5f)+K[2];
            float tr_square = I[2]*sspos[2].x+J[2]*sspos[2].y+K[2];
            
            //CULL
            if ((int)culling_mode == (tr_square >= 0)) return;

            I[0] /= tr_square; I[1] /= tr_square; I[2] /= tr_square;
            J[0] /= tr_square; J[1] /= tr_square; J[2] /= tr_square;
            Cy1 /= tr_square; Cy2 /= tr_square; Cy3 /= tr_square;
            for (uint32_t y = (uint32_t)bb.ymin; y <= (uint32_t)bb.ymax; ++y) {
                float Cx1 = Cy1, Cx2 = Cy2, Cx3 = Cy3;
                for (uint32_t x = (uint32_t)bb.xmin; x <= (uint32_t)bb.xmax; ++x) {
                    LiteMath::float3 barycentric = { Cx1, Cx2, Cx3 };
                    float interpolated_1_w = sspos[0].z * barycentric[0] + sspos[1].z * barycentric[1] + sspos[2].z * barycentric[2];
                    if (inside_of_triangle(barycentric)
                            && z_test(x, y, interpolated_1_w, fb.zbuf) 
                            && fb.color_buf) {
                        uint inversed_y = fb.color_buf->height() - y - 1;
                        VariablesData interpolated = m_pShaderObj->interpolate(vd, barycentric, interpolated_1_w);
                        LiteMath::float4 res       = m_pShaderObj->pixel_shader(interpolated) * 255.0f;
                        (*fb.color_buf)[LiteMath::uint2{x, inversed_y}] = LiteMath::uchar4
                        { 
                            (LiteMath::uchar)res.x, 
                            (LiteMath::uchar)res.y, 
                            (LiteMath::uchar)res.z, 
                            (LiteMath::uchar)res.w 
                        }.u32;
                    }
                    Cx1 += I[0]; Cx2 += I[1]; Cx3 += I[2];
                }
                Cy1 += J[0]; Cy2 += J[1]; Cy3 += J[2];
            }
        }
        std::vector<TriangleSet> vs_to_ts(const std::vector<VariablesData> &vs)
        {
            switch (vs.size())
            {
            case 0: return {};
            case 3: return { { vs[0], vs[1], vs[2] } };
            case 4: return { { vs[0], vs[1], vs[2] }, { vs[2], vs[3], vs[0] } };
            }
            return {};
        }
    };

    template<typename Shader>
    std::shared_ptr<IShader> make_cpu_vsps_shader(const char *impl_path, std::shared_ptr<Shader> a_pShader)
    {
        return std::make_shared< ShaderWrapper<Shader> >(a_pShader);
    };
}