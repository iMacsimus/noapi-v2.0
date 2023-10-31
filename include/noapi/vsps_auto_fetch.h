#pragma once

#include <tuple>
#include <type_traits>
#include <cinttypes>

#include <noapi/attribute_checker.h>
#include <noapi/integer_sequence.h>

template<typename... Ts>
struct arglist{};

template<typename T, typename... Args>
arglist<Args...> get_args(T(*)(uint32_t, Args*...));

#define auto_fetch(...) \
using typelist = arglist<__VA_ARGS__>;

attribute_checker(HasTypedefTypelist, typename Type::typelist());
attribute_checker(HasMethodFetch, &(Type::vertex_fetch));

template<class Shader, typename ArgList> 
struct NonAutoFetcher;
template<class Shader, typename... Args>
struct NonAutoFetcher<Shader, arglist<Args...>>
{
    static typename Shader::InputData vertex_fetch(uint32_t i, Args*... ptrs)
    {
        return Shader::vertex_fetch(i, ptrs...);
    }
};

template<class Shader, typename ArgList> 
struct AutoFetcher;
template<class Shader, typename... Args>
struct AutoFetcher<Shader, arglist<Args...>>
{
    static typename Shader::InputData vertex_fetch(uint32_t i, Args*... ptrs)
    {
        return typename Shader::InputData { ptrs[i]... };
    }
};

template<class Shader, bool = HasMethodFetch<Shader>::value, bool = HasTypedefTypelist<typename Shader::InputData>::value>
struct Fetcher;
template<class Shader, bool has_typelist>
struct Fetcher<Shader, true, has_typelist> : NonAutoFetcher<Shader, decltype(get_args(Shader::vertex_fetch))>
{
};
template<class Shader>
struct Fetcher<Shader, false, true> : AutoFetcher<Shader, typename Shader::InputData::typelist>
{
};



