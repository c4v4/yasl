// Copyright (c) 2024 Francesco Cavaliere
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program. If not, see <https://www.gnu.org/licenses/>.

#ifndef CAV_INCLUDE_SPAN_HPP
#define CAV_INCLUDE_SPAN_HPP

#include <cassert>
#include <iterator>

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

}  // namespace cav

#endif /* CAV_INCLUDE_SPAN_HPP */
