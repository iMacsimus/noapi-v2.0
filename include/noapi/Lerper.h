#ifndef NOAPI_INCLUDE_LERPER_H
# define NOAPI_INCLUDE_LERPER_H

#include <tuple>
#include <iostream>

#include <LiteMath.h>

namespace noapi {

template<class Shader>
struct Lerper
{
 public:
  using VariablesData = typename Shader::VariablesData;
 public:
  static VariablesData lerp(
      const VariablesData &v1, 
      const VariablesData &v2, 
      float ratio, 
      const Shader* a_pShader) {
    VariablesData v3;
    v3.vPos = LiteMath::float4(1.0f);
    VariablesData arr[3] = { v1, v2, v3 };
    LiteMath::float3 barycentric{v1.vPos.w * (1.0f - ratio), 
                                 v2.vPos.w * ratio, 0.0f};
    auto result = a_pShader->interpolate(arr, barycentric, 1.0f);
    result.vPos = v1.vPos*(1.0f - ratio) + v2.vPos*ratio;
    return result;
  }
};

} // namespace noapi

#endif