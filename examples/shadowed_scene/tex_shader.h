#ifndef EXAMPLES_SHADOWED_SCENE_TEX_SHADER_H
# define EXAMPLES_SHADOWED_SCENE_TEX_SHADER_H

#include <memory>

#include <LiteMath.h>
#include <Image2d.h>

using namespace LiteMath;
using namespace LiteImage;

struct TexturedShader
{
 public:
  TexturedShader(
      const float4 *positions, 
      const float2 *tex_coords, 
      const uint32_t *indices)
      : positions(positions), tex_coords(tex_coords), 
        indices(indices) {}
 public:
  struct InputData
  {
    float4 aPos;
    float2 aTexCoords;
  };
  struct VariablesData
  {
    float2 vTexCoords;
    float4 vPosLightView;
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
  float4x4 light_view;
  float4x4 projection;
  std::shared_ptr<ICombinedImageSampler> tex_sampler;
  std::shared_ptr<ICombinedImageSampler> sm_sampler;
 protected:
  const float4* positions;
  const float2* tex_coords;
  const uint32_t* indices;
};

#endif