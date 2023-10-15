#include <cpuvsps.h>
#include <vsps_shader.h>

#include <iostream>
#include <string>
#include <chrono>

using namespace LiteMath;
using namespace LiteImage;

struct InputData
{
    InputData(const float3 &aPos, const float3 &aCol) : aPos(aPos), aCol(aCol) {}
    float3 aPos;
    float3 aCol;
};

struct VariablesData
{
    float3 vCol;
};

struct Uniforms
{
};

struct SimpleShader : VSPS_Shader 
<
    InputData,
    VariablesData,
    Uniforms,
    CPU_VSPS_Executor,
    std::allocator
>
{
    VariablesData vertex_shader(const InputData *input, uint32_t index, LiteMath::float4 &vertexPosition)
    {
        vertexPosition = to_float4(input[index].aPos, 1.0f);
        return VariablesData{ input[index].aCol };
    }

    LiteMath::float4 pixel_shader(VariablesData in) 
    { 
        return to_float4(in.vCol, 1.0f);
    }

    VariablesData interpolate(
            const VariablesData data[3], 
            const LiteMath::float4 p[3], 
            const LiteMath::float3 &barycentric,
            float interpolated_1_w)
    {
        VariablesData res;
        res.vCol = (data[0].vCol * p[1].z * barycentric[0] + 
                data[1].vCol * p[2].z * barycentric[1] + 
                data[2].vCol * p[2].z * barycentric[2]) / interpolated_1_w;
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

    SimpleShader program;
    program.make_and_bind_data(3, (float3*)positions, (float3*)colors);
    program.make_and_bind_index_arr(3, indices);
    program.bind_framebuffer(Framebuffer{&image, &zbuf});
    program.set_viewport(0, 0, 1920, 1080);

    float sum = 0;
    int count = 100;
    for (int i = 0; i < count; ++i) {
        auto b = std::chrono::high_resolution_clock::now();
        program.draw();
        auto e = std::chrono::high_resolution_clock::now();
        sum += std::chrono::duration_cast<std::chrono::microseconds>(e-b).count() / 1000.0f;
    }

    std::cout << sum/count << "ms" << std::endl;

    SaveImage((folder_path+"triangle_result.bmp").c_str(), image);
    return 0;
}