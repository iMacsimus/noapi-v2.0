#include "LiteMath.h"
#include "Image2d.h"

#include <memory> // for std::shared_ptr

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

    static InputData vertex_fetch(uint32_t i, float4 *positions, float2 *tex_coords);

    static VariablesData vertex_shader(InputData input, const Uniforms &uniforms);
    static float4 pixel_shader(VariablesData in, const Uniforms &uniforms);

    static VariablesData interpolate(const VariablesData data[3], const LiteMath::float3 &barycentric, float interpolated_1_w);
};
