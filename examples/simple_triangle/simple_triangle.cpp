#include <noapi/cpuvsps.h>

#include <Image2d.h>

#include <iostream>
#include <string>
#include <chrono>

#include "simple_shader.h"
using namespace noapi;
using namespace LiteImage;

int main(int argc, char **argv) 
{
    std::string cur_path = argv[0];
    std::string folder_path = cur_path.substr(0, cur_path.find_last_of('/')+1) + "../";

    float3 positions[] = 
    {
        { -0.5f, -0.5f, 0.0f },
        { 0.5f, -0.5f, 0.0f },
        { 0.0f,  0.5f, 0.0f }
    };
    float3 colors[] = 
    {
        { 1.0f, 0.0f, 0.0f },
        { 0.0f, 1.0f, 0.0f },
        { 0.0f, 0.0f, 1.0f }
    };
    uint32_t indices[] = { 0, 1, 2 };

    int w = 1920;
    int h = 1080;
    Image2D<uint32_t> image(w, h);
    Image2D<float> zbuf(w, h);

    auto shader = std::make_shared<SimpleShader>(positions, colors, indices);
    auto program = make_cpu_vsps_shader<SimpleShader>(argv[0], shader);
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