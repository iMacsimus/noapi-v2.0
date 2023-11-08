#pragma once 
#include <tuple>
#include <noapi/attribute_checker.h>
#include <noapi/choose.h>
#include <iostream>
#include <LiteMath.h>

#define auto_interpolate(...) \
template<size_t index> auto get() -> decltype(noapi::choose<index>(__VA_ARGS__)) { return noapi::choose<index>(__VA_ARGS__); } \
template<size_t index> auto get() const -> decltype(noapi::choose<index>(__VA_ARGS__)) { return noapi::choose<index>(__VA_ARGS__); } \
static const size_t interp_count = std::tuple_size<decltype(std::tie(__VA_ARGS__))>::value;

namespace noapi {
    attribute_checker(HasMethodInterpolate, &(Type::interpolate));
    attribute_checker(HasMethodGet, Type::interp_count);
    attribute_checker(HasAttribDefaultIndex, Type::default_index);

    template<typename T, bool = HasAttribDefaultIndex<T>::value>
    struct DefaultIndex
    {
        static const size_t value = T::default_index;
    };

    template<typename T>
    struct DefaultIndex<T, false>
    {
        static const size_t value = 0;
    };

    template<typename Shader, typename IndexSequence>
    struct AutoInterpolator;
    template<typename Shader, size_t... indices>
    struct AutoInterpolator<Shader, noapi::index_sequence<indices...>>
    {
        using VariablesData = typename Shader::VariablesData;

        static VariablesData
        interpolate(
                const VariablesData data[3],  
                const LiteMath::float3 &barycentric,
                float interpolated_1_w)
        {
            VariablesData res = data[DefaultIndex<VariablesData>::value];
            int dummy[] = 
            { 
                (res.template get<indices>() = (data[0].template get<indices>() * (barycentric[0]/data[0].vPos.w)
                        + data[1].template get<indices>() * (barycentric[1]/data[1].vPos.w)
                        + data[2].template get<indices>() * (barycentric[2]/data[2].vPos.w)) * (1.0 / interpolated_1_w), 0)...
            };
            return res;
        }
    };

    template<typename Shader>
    struct NonAutoInterpolator
    {
        using VariablesData = typename Shader::VariablesData;
        
        static VariablesData
        interpolate(
                const VariablesData data[3],  
                const LiteMath::float3 &barycentric,
                float interpolated_1_w)
        {
            return Shader::interpolate(data, barycentric, interpolated_1_w);
        }
    };

    template<typename Shader, bool = HasMethodInterpolate<Shader>::value, bool = HasMethodGet<typename Shader::VariablesData>::value>
    struct Interpolator;
    template<typename Shader, bool has_meth_get>
    struct Interpolator<Shader, true, has_meth_get> : 
            NonAutoInterpolator<Shader> {};
    template<typename Shader>
    struct Interpolator<Shader, false, true> : 
            AutoInterpolator<Shader, noapi::make_index_sequence<Shader::VariablesData::interp_count>> {};

    template<class Shader>
    struct Lerper
    {
        using VariablesData = typename Shader::VariablesData;
        static VariablesData lerp(const VariablesData &v1, const VariablesData &v2, float ratio)
        {
            
            VariablesData arr[3] = { v1, v2, VariablesData{ .vPos = LiteMath::float4(1.0f, 1.0f, 1.0f, 1.0f) } };
            LiteMath::float3 barycentric = { v1.vPos.w * (1.0f - ratio), v2.vPos.w * ratio, 0.0f };
            auto result = Interpolator<Shader>::interpolate(arr, barycentric, 1.0f);
            result.vPos = v1.vPos * (1.0f - ratio) + v2.vPos * ratio;
            return result;
        }
    };
} // namespace noapi