#include <iostream>
#include <chrono>

#include <Image2d.h>

#include <noapi/cpuvsps.h>
#include <Mesh.h>

using namespace noapi;
using namespace LiteMath;
using namespace LiteImage;

struct TexturedShader
{
    struct InputData
    {
        float4 aPos;
        float2 aTexCoords;
    };

    struct Uniforms
    {
        float4x4 view;
        float4x4 projection;
        std::shared_ptr<ICombinedImageSampler> sampler;
    };

    struct VariablesData
    {
        float2 vTexCoords;
        float4 vPos;
    };

    static
    InputData vertex_fetch(uint32_t i, float4 *positions, float2 *tex_coords)
    {
        return InputData { positions[i], tex_coords[i] };
    }

    static
    VariablesData vertex_shader(InputData input, const Uniforms &uniforms)
    {
        VariablesData out;
        out.vPos = uniforms.projection * uniforms.view * input.aPos;
        out.vTexCoords = input.aTexCoords;
        return out;
    }

    static
    float4 pixel_shader(VariablesData in, const Uniforms &uniforms) 
    { 
        float4 color = uniforms.sampler->sample(in.vTexCoords);
        color.w = 1.0f;
        return color;
    }

    static
    VariablesData interpolate(
            const VariablesData data[3],  
            const LiteMath::float3 &barycentric,
            float interpolated_1_w)
    {
        VariablesData res;
        res.vTexCoords = (data[0].vTexCoords / data[0].vPos.w * barycentric[0] + 
                data[1].vTexCoords / data[1].vPos.w * barycentric[1] + 
                data[2].vTexCoords / data[2].vPos.w * barycentric[2]) / interpolated_1_w;
        return res;
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
    Image2D<uint32_t> color_buffer(w, h);
    Image2D<float> z_buf(w, h);
    Framebuffer fb = { &color_buffer, &z_buf };

    auto img = std::make_shared<Image2D<uint32_t>>(LoadImage<uint32_t>((rsc_path+"texture1.bmp").data()));
    Sampler img_sampler;

    Scene scn = load_scene_from_obj(rsc_path+"cube.obj");
    Mesh& cube = scn.meshes["Cube"];
    size_t indices_count = cube.indices.size();

    auto program = make_cpu_vsps_shader<TexturedShader>(argv[0]);
    program->set_vertex_arrays(cube.vertices.data(), cube.tex_coords.data());
    program->set_index_array(cube.indices.data());
    program->set_viewport(0, 0, w, h);
    program->set_culling(CULL_BACK);

    TexturedShader::Uniforms uniforms;
    uniforms.view = lookAt(float3(5.0f, 2.0f, 5.0f), float3(0.0f), float3(0.0f, 1.0f, 0.0f));
    uniforms.projection = perspectiveMatrix(45, (float)w/h, 0.1f, 100.0f);
    uniforms.sampler = MakeCombinedTexture2D(img, img_sampler);
    program->set_uniform(&uniforms);

    float sum = 0;
    int count = 100;
    for (int i = 0; i < count; ++i) {
        z_buf.clear(0);
        color_buffer.clear(0);
        auto b = std::chrono::high_resolution_clock::now();
        program->draw_triangles(indices_count, fb);
        auto e = std::chrono::high_resolution_clock::now();
        sum += std::chrono::duration_cast<std::chrono::microseconds>(e-b).count() / 1000.0f;
    }
    std::cout << sum/count << "ms" << std::endl;

    SaveImage((project_path+"cube.bmp").data(), color_buffer);
    return 0;
}