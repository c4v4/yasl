// SPDX-FileCopyrightText: 2024 Francesco Cavaliere <francescocava95@gmail.com>
// SPDX-License-Identifier: MIT

#ifndef CAV_INCLUDE_SPAN_HPP
#define CAV_INCLUDE_SPAN_HPP

#include <cassert>
#include <iterator>

#include "utils.hpp"

namespace cav {

/// @brief A simple non-owning span
template <typename ItT>
struct Span {
    using iterator        = ItT;
    using size_type       = std::size_t;
    using value_type      = typename std::iterator_traits<ItT>::value_type;
    using reference       = typename std::iterator_traits<ItT>::reference;
    using pointer         = typename std::iterator_traits<ItT>::pointer;
    using difference_type = typename std::iterator_traits<ItT>::difference_type;

    iterator start;
    iterator finish;

    [[nodiscard]] size_type size() const {
        return finish - start;
    }

    [[nodiscard]] bool empty() const {
        return start == finish;
    }

    [[nodiscard]] iterator begin() const {
        return start;
    }

    [[nodiscard]] iterator end() const {
        return finish;
    }

    [[nodiscard]] value_type& operator[](size_type i) const {
        assert(finish > start);
        assert(i < static_cast<size_t>(finish - start));
        return start[i];
    }
};

template <typename ItT>
[[nodiscard]] inline Span<ItT> make_span() {
    return {{}, {}};
}

template <typename ItT>
[[nodiscard]] inline Span<ItT> make_span(ItT beg, ItT end) {
    return {beg, end};
}

template <typename ItT>
[[nodiscard]] inline Span<ItT> make_span(ItT beg, size_t sz) {
    return {beg, beg + sz};
}

template <typename ConT>
[[nodiscard]] inline auto make_span(ConT&& cont, size_t beg, size_t end)
    -> Span<container_iterator_t<ConT>> {
    return {std::begin(cont) + beg, std::begin(cont) + end};
}

}  // namespace cav

#endif /* CAV_INCLUDE_SPAN_HPP */
