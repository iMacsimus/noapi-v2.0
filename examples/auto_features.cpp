#include <iostream>
#include <chrono>

#include <Image2d.h>

#include <noapi/cpuvsps.h>
#include <Mesh.h>

using namespace LiteMath;
using namespace LiteImage;
using namespace noapi;

struct TexturedShader
{
    struct InputData
    {
        float4 aPos; float2 aTexCoords;
        auto_fetch(float4, float2)
    };
    struct Uniforms
    {
        float4x4 view;
        float4x4 light_view;
        float4x4 projection;
        std::shared_ptr<ICombinedImageSampler> tex_sampler;
        std::shared_ptr<ICombinedImageSampler> sm_sampler;
    };
    struct VariablesData
    {
        float2 vTexCoords; float4 vPosLightView; float4 vPos;
        auto_interpolate(vTexCoords, vPosLightView)
    };
    static
    VariablesData vertex_shader(InputData input, const Uniforms &uniforms)
    {
        VariablesData out;
        out.vPos = uniforms.projection * uniforms.view * input.aPos;
        out.vTexCoords = input.aTexCoords;
        out.vPosLightView = uniforms.projection * uniforms.light_view * input.aPos;
        return out;
    }
    static
    float4 pixel_shader(VariablesData in, const Uniforms &uniforms) 
    { 
        //color from texture
        float4 color = uniforms.tex_sampler->sample(in.vTexCoords);
        //shadow
        float bias = 0.001f;
        float cur_depth = 1.0f / in.vPosLightView.w;
        float4 coords = in.vPosLightView / in.vPosLightView.w;
        coords = coords*0.5f + 0.5f;
        float closest_depth = uniforms.sm_sampler->sample(float2(coords.x, coords.y)).x;
        float brightness = cur_depth+bias > closest_depth ? 1.0f : 0.5f;
        color *= brightness;
        color.w = 1.0f;
        return color;
    }
};

struct ShadowMapShader
{
    struct InputData
    {
        float4 aPos;
        auto_fetch(float4)
    };
    struct Uniforms { float4x4 view; float4x4 projection; };
    struct VariablesData
    {
        float4 vPos;
        auto_interpolate()
    };
    static 
    VariablesData vertex_shader(InputData input, const Uniforms &uniforms)
    {
        return VariablesData { uniforms.projection * uniforms.view * input.aPos };
    }
    static 
    float4 pixel_shader(VariablesData, const Uniforms&) // will not be called
    {
        return float4();
    }
};

int main(int argc, char **argv) 
{
    std::string cur_path = argv[0];
    std::string folder_path = cur_path.substr(0, cur_path.find_last_of('/')+1) + "../examples/";
    std::string project_path = folder_path + "../";
    std::string rsc_path = folder_path + "resources/";

    int w = 1920;
    int h = 1080;
    int shadowmap_resolution = 256;

    Image2D<uint32_t> color_buffer(w, h);
    Image2D<float> z_buf(w, h);

    Scene scn = load_scene_from_obj(rsc_path+"scene.obj");
    Mesh& cubes = scn.meshes["Cubes"];
    Mesh& floor = scn.meshes["Floor"];

    auto concrete_texture = std::make_shared<Image2D<uint>>(LoadImage<uint>((rsc_path+"concrete.bmp").data()));
    auto box_texture = std::make_shared<Image2D<uint>>(LoadImage<uint>((rsc_path+"box.bmp").data()));
    auto shadow_map = std::make_shared<Image2D<float>>(shadowmap_resolution, shadowmap_resolution);

    float4x4 view = lookAt(float3(5.76f, 5.76f, 5.0f), float3(0.0f), float3(0.0f, 1.0f, 0.0f));
    float4x4 light_view = lookAt(float3(0.0f, 10.4f, -15.5f), float3(0.0f), float3(0.0f, 1.0f, 0.0f));
    float4x4 projection = perspectiveMatrix(45, (float)w/h, 0.1f, 100.0f);

    TexturedShader::Uniforms floor_uniforms = 
    {
        view,
        light_view,
        projection,
        MakeCombinedTexture2D(concrete_texture, Sampler()),
        MakeCombinedTexture2D(shadow_map, Sampler()),
    };

    TexturedShader::Uniforms cubes_uniforms = 
    {
        view,
        light_view,
        projection,
        MakeCombinedTexture2D(box_texture, Sampler()),
        MakeCombinedTexture2D(shadow_map, Sampler()),
    };

    ShadowMapShader::Uniforms sm_uniforms = 
    {
        light_view,
        projection
    };

    auto tex_shader = make_cpu_vsps_shader<TexturedShader>(argv[0]);
    tex_shader->set_viewport(0, 0, w, h);
    tex_shader->set_culling(CULL_BACK);
    tex_shader->set_clipping(ON);
    auto sm_shader = make_cpu_vsps_shader<ShadowMapShader>(argv[0]);
    sm_shader->set_viewport(0, 0, shadowmap_resolution, shadowmap_resolution);
    sm_shader->set_clipping(ON);

    float sum = 0;
    int count = 1;
    for (int i = 0; i < count; ++i) {
        color_buffer.clear(uchar4{ 50, 50, 50, 255 }.u32);
        z_buf.clear(0);
        shadow_map->clear(0);
        auto b = std::chrono::high_resolution_clock::now();
        //draw to shadow_map
        sm_shader->set_vertex_arrays(cubes.vertices.data());
        sm_shader->set_index_array(cubes.indices.data());
        sm_shader->set_uniform(&sm_uniforms);
        sm_shader->draw_triangles(cubes.indices.size(), Framebuffer{ nullptr, shadow_map.get() });
        //draw to framebuffer
        tex_shader->set_vertex_arrays(cubes.vertices.data(), cubes.tex_coords.data());
        tex_shader->set_index_array(cubes.indices.data());
        tex_shader->set_uniform(&cubes_uniforms);
        tex_shader->draw_triangles(cubes.indices.size(), Framebuffer{ &color_buffer, &z_buf });
        tex_shader->set_vertex_arrays(floor.vertices.data(), floor.tex_coords.data());
        tex_shader->set_index_array(floor.indices.data());
        tex_shader->set_uniform(&floor_uniforms);
        tex_shader->draw_triangles(floor.indices.size(), Framebuffer{ &color_buffer, &z_buf });
        auto e = std::chrono::high_resolution_clock::now();
        sum += std::chrono::duration_cast<std::chrono::microseconds>(e-b).count() / 1000.0f;
    }
    std::cout << sum/count << "ms" << std::endl;

    SaveImage((project_path+"scene.bmp").data(), color_buffer);
    SaveImage((project_path+"sm.bmp").data(), *shadow_map.get());
    return 0;
}