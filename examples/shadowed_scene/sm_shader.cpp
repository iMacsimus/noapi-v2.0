#include "sm_shader.h"

ShadowMapShader::InputData 
ShadowMapShader::vertex_fetch(uint32_t i) const
{
    return InputData { positions[indices[i]] };
}

ShadowMapShader::VariablesData 
ShadowMapShader::vertex_shader(InputData input) const
{
    VariablesData out;
    out.vPos = projection * view * input.aPos;
    return VariablesData{ projection * view * input.aPos };
}

float4 ShadowMapShader::pixel_shader(VariablesData in) const
{
    return float4();
}

ShadowMapShader::VariablesData
ShadowMapShader::interpolate(
        const VariablesData data[3],  
        const LiteMath::float3 &barycentric,
        float interpolated_1_w) const
{
    return ShadowMapShader::VariablesData{};
}