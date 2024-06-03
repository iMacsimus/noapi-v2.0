#include "tex_shader.h"

TexturedShader::InputData 
TexturedShader::vertex_fetch(uint32_t i) const {
  return InputData { positions[indices[i]], tex_coords[indices[i]] };
}

TexturedShader::VariablesData 
TexturedShader::vertex_shader(InputData input) const {
  VariablesData out;
  out.vPos = projection * view * input.aPos;
  out.vTexCoords = input.aTexCoords;
  out.vPosLightView = projection * light_view * input.aPos;
  return out;
}

float4 TexturedShader::pixel_shader(VariablesData in) const { 
  //color from texture
  float4 color = tex_sampler->sample(in.vTexCoords);
  //shadow
  float bias = 0.001f;
  float cur_depth = 1.0f / in.vPosLightView.w;
  float4 coords = in.vPosLightView / in.vPosLightView.w;
  coords = coords*0.5f + 0.5f;
  float closest_depth = sm_sampler->sample(float2(coords.x, coords.y)).x;
  float brightness = cur_depth+bias > closest_depth ? 1.0f : 0.5f;
  color *= brightness;
  color.w = 1.0f;
  return color;
}

TexturedShader::VariablesData
TexturedShader::interpolate(
    const VariablesData data[3],  
    const LiteMath::float3 &barycentric,
    float interpolated_1_w) const {
  VariablesData res;
  res.vTexCoords = (data[0].vTexCoords / data[0].vPos.w * barycentric[0] + 
          data[1].vTexCoords / data[1].vPos.w * barycentric[1] + 
          data[2].vTexCoords / data[2].vPos.w * barycentric[2]) / interpolated_1_w;
  res.vPosLightView = (data[0].vPosLightView / data[0].vPos.w * barycentric[0] + 
          data[1].vPosLightView / data[1].vPos.w * barycentric[1] + 
          data[2].vPosLightView / data[2].vPos.w * barycentric[2]) / interpolated_1_w;
  return res;
}