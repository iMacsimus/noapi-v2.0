#include <iostream>
#include <chrono>

#include <Image2d.h>

#include <noapi/cpuvsps.h>
#include <Mesh.h>

#include "sm_shader.h"
#include "tex_shader.h"
using namespace noapi;

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

    float4x4 view = lookAt(float3(15.76f, 5.76f, -12.0f), float3(0.0f), float3(0.0f, 1.0f, 0.0f));
    float4x4 light_view = lookAt(float3(0.0f, 10.4f, -15.5f), float3(0.0f), float3(0.0f, 1.0f, 0.0f));
    float4x4 projection = perspectiveMatrix(45, (float)w/h, 0.1f, 100.0f);

    auto tex_floor_shader = std::make_shared<TexturedShader>(floor.vertices.data(), floor.tex_coords.data(), floor.indices.data());
    tex_floor_shader->view = view;
    tex_floor_shader->light_view = light_view;
    tex_floor_shader->projection = projection;
    tex_floor_shader->tex_sampler = MakeCombinedTexture2D(concrete_texture, Sampler());
    tex_floor_shader->sm_sampler = MakeCombinedTexture2D(shadow_map, Sampler());

    auto tex_cubes_shader = std::make_shared<TexturedShader>(cubes.vertices.data(), cubes.tex_coords.data(), cubes.indices.data());
    tex_cubes_shader->view = view;
    tex_cubes_shader->projection = projection;
    tex_cubes_shader->light_view = light_view;
    tex_cubes_shader->tex_sampler = MakeCombinedTexture2D(box_texture, Sampler());
    tex_cubes_shader->sm_sampler = MakeCombinedTexture2D(shadow_map, Sampler());

    auto sm_cubes_shader = std::make_shared<ShadowMapShader>(cubes.vertices.data(), cubes.indices.data());
    sm_cubes_shader->view = light_view;
    sm_cubes_shader->projection = projection;

    auto tex_floor_rasterizer = make_cpu_vsps_shader<TexturedShader>(argv[0], tex_floor_shader);
    tex_floor_rasterizer->set_viewport(0, 0, w, h);
    tex_floor_rasterizer->set_culling(CULL_BACK);

    auto tex_cubes_rasterizer = make_cpu_vsps_shader<TexturedShader>(argv[0], tex_cubes_shader);
    tex_cubes_rasterizer->set_viewport(0, 0, w, h);
    tex_cubes_rasterizer->set_culling(CULL_BACK);

    auto sm_cubes_rasterizer = make_cpu_vsps_shader<ShadowMapShader>(argv[0], sm_cubes_shader);
    sm_cubes_rasterizer->set_viewport(0, 0, shadowmap_resolution, shadowmap_resolution);
    sm_cubes_rasterizer->set_culling(CULL_BACK);

    float sum = 0;
    int count = 100;
    for (int i = 0; i < count; ++i) {
        color_buffer.clear(uchar4{ 50, 50, 50, 255 }.u32);
        z_buf.clear(0);
        shadow_map->clear(0);
        auto b = std::chrono::high_resolution_clock::now();
        //draw to shadow_map
        sm_cubes_rasterizer->draw_triangles(cubes.indices.size(), Framebuffer{ nullptr, shadow_map.get() });
        //draw to framebuffer
        tex_cubes_rasterizer->draw_triangles(cubes.indices.size(), Framebuffer{ &color_buffer, &z_buf });
        tex_floor_rasterizer->draw_triangles(floor.indices.size(), Framebuffer{ &color_buffer, &z_buf });
        auto e = std::chrono::high_resolution_clock::now();
        sum += std::chrono::duration_cast<std::chrono::microseconds>(e-b).count() / 1000.0f;
    }
    std::cout << sum/count << "ms" << std::endl;

    SaveImage((project_path+"scene.bmp").data(), color_buffer);
    SaveImage((project_path+"sm.bmp").data(), *shadow_map.get());
    return 0;
}