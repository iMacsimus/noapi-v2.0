#pragma once 

#pragma once 

#include <LiteMath.h>
#include <Image2d.h>
#include <memory>

using namespace LiteMath;
using namespace LiteImage;

struct ShadowMapShader
{
public:
    ShadowMapShader(const float4 *positions, const uint32_t *indices)
        : 
            positions(positions), 
            indices(indices)
    {
    }
public:
    struct InputData
    {
        float4 aPos;
    };
    struct VariablesData
    {
        float4 vPos;
    };
public:
    InputData vertex_fetch(uint32_t i) const;
    VariablesData vertex_shader(InputData input) const;
    float4 pixel_shader(VariablesData in) const;
    VariablesData interpolate(
            const VariablesData data[3],  
            const LiteMath::float3 &barycentric,
            float interpolated_1_w) const;
public: //uniforms
    float4x4 view;
    float4x4 projection;
protected:
    const float4* positions;
    const uint32_t* indices;
};