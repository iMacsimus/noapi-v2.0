#pragma once 
#include <tuple>
#include <noapi/attribute_checker.h>
#include <noapi/choose.h>
#include <LiteMath.h>

#define auto_interpolate(...) \
template<size_t index> auto get() -> decltype(noapi::choose<index>(__VA_ARGS__)) { return noapi::choose<index>(__VA_ARGS__); } \
template<size_t index> auto get() const -> decltype(noapi::choose<index>(__VA_ARGS__)) { return noapi::choose<index>(__VA_ARGS__); } \
static const size_t interp_count = std::tuple_size<decltype(std::tie(__VA_ARGS__))>::value;

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

template<class Shader> 
struct Interpolator
{
    using VariablesData = typename Shader::VariablesData;
    static VariablesData
    interpolate(const VariablesData data[3],  
            const LiteMath::float3 &barycentric,
            float interpolated_1_w)
    {
        static_assert(HasMethodInterpolate<Shader>::value || HasMethodGet<VariablesData>::value, 
                "Shader class hasn't method interpolate(). Implement this static method or use auto_interpolate");
        return helper<Shader>(data, barycentric, interpolated_1_w);
    }
    template<class T, typename std::enable_if<HasMethodInterpolate<T>::value, int>::type = 0> 
    static VariablesData
    helper(const VariablesData data[3],  
            const LiteMath::float3 &barycentric,
            float interpolated_1_w)
    {
        return Shader::interpolate(data, barycentric, interpolated_1_w);
    }
    template<class T, typename std::enable_if<
            !(HasMethodInterpolate<T>::value)
            && (HasMethodGet<VariablesData>::value), int>::type = 0> 
    static VariablesData
    helper(const VariablesData data[3],  
            const LiteMath::float3 &barycentric,
            float interpolated_1_w)
    {
        return auto_interpolation<T>(noapi::make_index_sequence<VariablesData::interp_count>(), data, barycentric, interpolated_1_w);
    }
    template<class T, size_t... indices>
    static VariablesData
    auto_interpolation(noapi::index_sequence<indices...>,
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