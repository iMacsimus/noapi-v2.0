#pragma once
#include <type_traits>

#define attribute_checker(NAME, ExpressionToCheck) \
template<class T> \
struct NAME \
{\
    template<typename Type> \
    static constexpr auto f(int) -> decltype(ExpressionToCheck, int()); \
    template<typename...> \
    static constexpr char f(...); \
    const static bool value = std::is_same<int, decltype(f<T>(0))>::value; \
}
