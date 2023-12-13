#include "simple_shader.h"

SimpleShader::InputData
SimpleShader::vertex_fetch(uint32_t i) const
{
    return InputData { positions[indices[i]], colors[indices[i]] };
}

SimpleShader::VariablesData 
SimpleShader::vertex_shader(InputData input) const
{
    return VariablesData{ input.aCol, to_float4(input.aPos, 1.0f) };
}


float4 
SimpleShader::pixel_shader(VariablesData in) const
{ 
    return to_float4(in.vCol, 1.0f);
}

SimpleShader::VariablesData 
SimpleShader::interpolate(
        const VariablesData data[3],  
        const LiteMath::float3 &barycentric,
        float interpolated_1_w) const
{
    VariablesData res;
    res.vCol = (data[0].vCol / data[0].vPos.w * barycentric[0] + 
            data[1].vCol / data[1].vPos.w * barycentric[1] + 
            data[2].vCol / data[2].vPos.w * barycentric[2]) / interpolated_1_w;
    return res;
}