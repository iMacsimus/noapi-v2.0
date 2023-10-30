#pragma once
#include <cstddef>

namespace noapi
{
	template <std::size_t ...>
    struct index_sequence
    { };

    template <std::size_t N, std::size_t ... Next>
    struct indexSequenceHelper : public indexSequenceHelper<N-1U, N-1U, Next...>
    { };

    template <std::size_t ... Next>
    struct indexSequenceHelper<0U, Next ... >
    { using type = index_sequence<Next ... >; };

    template <std::size_t N>
    using make_index_sequence = typename indexSequenceHelper<N>::type;
}
