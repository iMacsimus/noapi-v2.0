#include "LiteMath.h"
#include "Image2d.h"

#include <memory> // for std::shared_ptr

using namespace LiteMath;
using namespace LiteImage;

struct TexturedShader
{
    TexturedShader(const float4* a_positions, const float2* a_tex_coords, const uint* a_indices) : positions(a_positions), tex_coords(a_tex_coords), indices(a_indices) {}

    struct InputData
    {
        float4 aPos;
        float2 aTexCoords;
    };

    struct VariablesData
    {
        float2 vTexCoords;
        float4 vPos;
    };

    InputData vertex_fetch(uint32_t i) const;

    VariablesData vertex_shader(InputData input) const;
    float4 pixel_shader(VariablesData in) const;

    VariablesData interpolate(const VariablesData data[3], const LiteMath::float3 &barycentric, float interpolated_1_w) const;

protected:
    const float4* positions  = nullptr;
    const float2* tex_coords = nullptr; 
    const uint*   indices    = nullptr; 
    
public:
    float4x4 view;
    float4x4 projection;
    std::shared_ptr<ICombinedImageSampler> sampler;
};
