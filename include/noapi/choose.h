#pragma once
#include <noapi/integer_sequence.h>

namespace noapi 
{
    template<typename T> struct choose_helper;
    template<size_t... indices>
    struct choose_helper<index_sequence<indices...>>
    {
        template<typename T> static T& 
        choose(decltype(indices, (const void*)nullptr)..., T* ptr, ...) { return *ptr; }
    };

    template<size_t index, typename... Ts>
    inline
    auto choose(const Ts&... args) -> decltype(choose_helper<make_index_sequence<index>>::choose(&args...))
    {
        return choose_helper<make_index_sequence<index>>::choose(&args...);
    }

    template<size_t index, typename... Ts>
    inline
    auto choose(Ts&... args) -> decltype(choose_helper<make_index_sequence<index>>::choose(&args...))
    {
        return choose_helper<make_index_sequence<index>>::choose(&args...);
    }
} // namespace noapi