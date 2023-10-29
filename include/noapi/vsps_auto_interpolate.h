#pragma once 
#include <noapi/tuple_repr.h>
#include<noapi/attribute_checker.h>
#include <LiteMath.h>

#define auto_interpolate(...) \
using repr_t = decltype(noapi::tie<VariablesData>(__VA_ARGS__)); \
operator repr_t() const { return noapi::tie<VariablesData>(__VA_ARGS__); };

attribute_checker(HasAttributeInterpolate, &(Type::interpolate));
attribute_checker(HasTypedefReprT, typename Type::repr_t());

template<class Shader> 
struct Interpolator
{
    using VariablesData = typename Shader::VariablesData;
    static VariablesData
    interpolate(const VariablesData data[3],  
            const LiteMath::float3 &barycentric,
            float interpolated_1_w)
    {
        static_assert(HasAttributeInterpolate<Shader>::value || HasTypedefReprT<VariablesData>::value, 
                "Shader class hasn't method interpolate(). Implement this static method or use auto_interpolate");
        return helper<Shader>(data, barycentric, interpolated_1_w);
    }
    template<class T, typename std::enable_if<HasAttributeInterpolate<T>::value, int>::type = 0> 
    static VariablesData
    helper(const VariablesData data[3],  
            const LiteMath::float3 &barycentric,
            float interpolated_1_w)
    {
        return Shader::interpolate(data, barycentric, interpolated_1_w);
    }
    template<class T, typename std::enable_if<
            !(HasAttributeInterpolate<T>::value)
            && (HasTypedefReprT<VariablesData>::value), int>::type = 0> 
    static VariablesData
    helper(const VariablesData data[3],  
            const LiteMath::float3 &barycentric,
            float interpolated_1_w)
    {
        typename VariablesData::repr_t tmp[3] = { data[0], data[1], data[2] };
        return (tmp[0] * (barycentric[0]/data[0].vPos.w) 
                + tmp[1] * (barycentric[1]/data[1].vPos.w)
                + tmp[2] * (barycentric[2]/data[2].vPos.w)) * (1.0f / interpolated_1_w);
    }
};

