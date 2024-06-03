#ifndef NOAPI_INCLUDE_VSPS_CLIPPING_H
# define NOAPI_INCLUDE_VSPS_CLIPPING_H

#include <vector>
#include <iostream>

#include <LiteMath.h>

#include "Lerper.h"

namespace noapi 
{

template<typename Shader>
std::vector<typename Shader::VariablesData> 
clip(const typename Shader::VariablesData data[3], const Shader* a_pShader)
{
  using VariablesData = typename Shader::VariablesData;
  bool is_inside[] = {
    data[0].vPos.z >= -data[0].vPos.w,
    data[1].vPos.z >= -data[1].vPos.w,
    data[2].vPos.z >= -data[2].vPos.w
  };
  size_t count = is_inside[0] + is_inside[1] + is_inside[2];
  switch (count)
  {
    case 0:
      return {};
    case 1: {
      size_t index = is_inside[1]*1 + is_inside[2]*2;
      size_t other[2] = { (index+1)%3, (index+2)%3 };
      float v1_ratio = (data[index].vPos.w + data[index].vPos.z)
                     / ((data[index].vPos.w + data[index].vPos.z) 
                       -(data[other[0]].vPos.w + data[other[0]].vPos.z));
      auto new_v1 = Lerper<Shader>::lerp(data[index], data[other[0]], 
                                         v1_ratio, a_pShader);
      float v2_ratio = (data[index].vPos.w + data[index].vPos.z)
                     / ((data[index].vPos.w + data[index].vPos.z) 
                       -(data[other[1]].vPos.w + data[other[1]].vPos.z));
      auto new_v2 = Lerper<Shader>::lerp(data[index], data[other[1]], 
                                         v2_ratio, a_pShader);
      return { data[index], new_v1, new_v2 };
    }
    case 2: {
      size_t index = (!is_inside[1])*1 + (!is_inside[2])*2;
      size_t other[2] = { (index+1)%3, (index+2)%3 };
      float v1_ratio = (data[other[1]].vPos.w + data[other[1]].vPos.z)
                     / ((data[other[1]].vPos.w + data[other[1]].vPos.z) 
                       -(data[index].vPos.w + data[index].vPos.z));
      auto new_v1 = Lerper<Shader>::lerp(data[other[1]], data[index], 
                                         v1_ratio, a_pShader);
      float v2_ratio = (data[other[0]].vPos.w + data[other[0]].vPos.z)
                     / ((data[other[0]].vPos.w + data[other[0]].vPos.z) 
                       -(data[index].vPos.w + data[index].vPos.z));
      auto new_v2 = Lerper<Shader>::lerp(data[other[0]], data[index], 
                                         v2_ratio, a_pShader);
      return { data[other[0]], data[other[1]], new_v1, new_v2 };
    }
    case 3: default:
      return { data[0], data[1], data[2] };
  }
  return {};
}

} // namespace noapi

#endif