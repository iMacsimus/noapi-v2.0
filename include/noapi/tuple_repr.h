#pragma once

#include <tuple>
#include <noapi/integer_sequence.h>

namespace noapi 
{
    template<class S, typename... Attributes>
    struct tuple_repr
    {
        std::tuple<Attributes...> container;
        tuple_repr operator+(const tuple_repr& src) 
        {
            return sum(src, make_index_sequence<std::tuple_size<decltype(container)>::value>());
        }
        tuple_repr operator*(float scalar)
        {
            return mult(scalar, make_index_sequence<std::tuple_size<decltype(container)>::value>());
        }
        operator S() { return to_struct(make_index_sequence<std::tuple_size<decltype(container)>::value>()); }
    private:
        template<size_t... indices>
        tuple_repr sum(const tuple_repr& src, index_sequence<indices...>) 
        {
            auto res= *this;
            int dummy[] = { (std::get<indices>(res.container)=std::get<indices>(res.container)+std::get<indices>(src.container), 0)... };
            return res;
        }
        template<size_t... indices>
        tuple_repr mult(float scalar, 
                index_sequence<indices...>) 
        {
            auto res= *this;
            int dummy[] = { (std::get<indices>(res.container)=std::get<indices>(res.container)*scalar, 0)... };
            return res;
        }
        template<size_t... indices>
        S to_struct(index_sequence<indices...>)
        {
            return S { std::get<indices>(container)... };
        }
    };

    template<class S, typename... Attributes>
    tuple_repr<S, Attributes...> tie(const Attributes&... attributes)
    {
        return tuple_repr<S, Attributes...> { std::tie(attributes...) };
    }
}
