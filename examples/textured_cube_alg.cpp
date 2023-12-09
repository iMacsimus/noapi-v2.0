#include <iostream>
#include <chrono>

#include "textured_cube_alg.h"

using namespace LiteMath;
using namespace LiteImage;

TexturedShader::InputData TexturedShader::vertex_fetch(uint32_t i, float4 *positions, float2 *tex_coords)
{
  return InputData { positions[i], tex_coords[i] };
}

TexturedShader::VariablesData TexturedShader::vertex_shader(InputData input, const Uniforms &uniforms)
{
  VariablesData out;
  out.vPos = uniforms.projection * uniforms.view * input.aPos;
  out.vTexCoords = input.aTexCoords;
  return out;
}

float4 TexturedShader::pixel_shader(VariablesData in, const Uniforms &uniforms) 
{ 
  float4 color = uniforms.sampler->sample(in.vTexCoords);
  color.w = 1.0f;
  return color;
}

TexturedShader::VariablesData TexturedShader::interpolate(const VariablesData data[3], const LiteMath::float3 &barycentric, float interpolated_1_w)
{
  TexturedShader::VariablesData res;
  res.vTexCoords = (data[0].vTexCoords / data[0].vPos.w * barycentric[0] + 
                    data[1].vTexCoords / data[1].vPos.w * barycentric[1] + 
                    data[2].vTexCoords / data[2].vPos.w * barycentric[2]) / interpolated_1_w;
  return res;
}
