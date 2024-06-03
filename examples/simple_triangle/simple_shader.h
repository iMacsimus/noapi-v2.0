#ifndef EXAMPLES_SIMPLE_TRIANGLE_SIMPLE_SHADER_H
# define EXAMPLES_SIMPLE_TRIANGLE_SIMPLE_SHADER_H

#include <memory>

#include <LiteMath.h>

using namespace LiteMath;

struct SimpleShader
{
 public:
  SimpleShader(
      const float3 *positions, 
      const float3 *colors, 
      const uint32_t *indices)
      : positions(positions), colors(colors),
        indices(indices) {}
 public:
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
 public:
  InputData vertex_fetch(uint32_t i) const;
  VariablesData vertex_shader(InputData input) const;
  float4 pixel_shader(VariablesData in) const;
  VariablesData interpolate(
      const VariablesData data[3],  
      const LiteMath::float3 &barycentric,
      float interpolated_1_w) const;
 protected:
  const float3 *positions;
  const float3 *colors;
  const uint32_t *indices;
};

#endif