#include <noapi/cpuvsps.h>

#include <iostream>
#include <string>
#include <chrono>

using namespace LiteMath;
using namespace LiteImage;
using namespace noapi;

struct SimpleShader
{
    struct InputData
    {
        float3 aPos;
        float3 aCol;
    };

    struct VariablesData
    {
        float3 vCol;
        float4 vPos;
    };

    struct Uniforms
    {
    };

    static
    InputData vertex_fetch(uint32_t i, float3 *positions, float3 *colors)
    {
        return InputData { positions[i], colors[i] };
    }

    static
    VariablesData vertex_shader(InputData input, const Uniforms &uniforms)
    {
        return VariablesData{ input.aCol, to_float4(input.aPos, 1.0f) };
    }

    static
    float4 pixel_shader(VariablesData in, const Uniforms &uniforms) 
    { 
        return to_float4(in.vCol, 1.0f);
    }

    static
    VariablesData interpolate(
            const VariablesData data[3],  
            const LiteMath::float3 &barycentric,
            float interpolated_1_w)
    {
        VariablesData res;
        res.vCol = (data[0].vCol / data[0].vPos.w * barycentric[0] + 
                data[1].vCol / data[1].vPos.w * barycentric[1] + 
                data[2].vCol / data[2].vPos.w * barycentric[2]) / interpolated_1_w;
        return res;
    }
};

int main(int argc, char **argv) 
{
    std::string cur_path = argv[0];
    std::string folder_path = cur_path.substr(0, cur_path.find_last_of('/')+1) + "../";

    float positions[] = 
    {
        -0.5f, -0.5f, 0.0f,
         0.5f, -0.5f, 0.0f,
         0.0f,  0.5f, 0.0f
    };
    float colors[] = 
    {
        1.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 1.0f
    };
    uint32_t indices[] = { 0, 1, 2 };

    int w = 1920;
    int h = 1080;
    Image2D<uint32_t> image(w, h);
    Image2D<float> zbuf(w, h);

    auto program = make_cpu_vsps_shader<SimpleShader>(argv[0]);
    program->set_vertex_arrays(positions, colors);
    program->set_index_array(indices);
    program->set_viewport(0, 0, w, h);

    float sum = 0;
    int count = 100;
    for (int i = 0; i < count; ++i) {
        image.clear(0);
        zbuf.clear(0);
        auto b = std::chrono::high_resolution_clock::now();
        program->draw_triangles(3, Framebuffer{ &image, &zbuf });
        auto e = std::chrono::high_resolution_clock::now();
        sum += std::chrono::duration_cast<std::chrono::microseconds>(e-b).count() / 1000.0f;
    }
    std::cout << sum/count << "ms" << std::endl;

    SaveImage((folder_path+"triangle_result.bmp").c_str(), image);
    return 0;
}