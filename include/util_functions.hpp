// SPDX-FileCopyrightText: 2024 Francesco Cavaliere <francescocava95@gmail.com>
// SPDX-License-Identifier: MIT

#ifndef CAV_INCLUDE_UTIL_FUNCTIONS_HPP
#define CAV_INCLUDE_UTIL_FUNCTIONS_HPP

#include <cstring>
#include <utility>

namespace cav {

#define CAV_REQUIRES(...)      typename std::enable_if<(__VA_ARGS__)>::type
#define CAV_REQUIRES_T(T, ...) typename std::enable_if<(__VA_ARGS__), T>::type

template <typename T>
using no_cvr = typename std::remove_cv<typename std::remove_reference<T>::type>::type;

template <typename C>
struct container_metadata {
    using iterator   = decltype(std::declval<C>().begin());
    using value_type = no_cvr<decltype(*std::declval<iterator>())>;
    using size_type  = decltype(std::declval<C>().size());
};

template <typename T, size_t N>
struct container_metadata<T[N]> {
    using iterator   = T*;
    using value_type = T;
    using size_type  = size_t;
};

template <typename C>
using container_iterator_t = typename container_metadata<C>::iterator;
template <typename C>
using container_value_type_t = typename container_metadata<C>::value_type;
template <typename C>
using container_size_type_t = typename container_metadata<C>::size_type;

template <typename T2, typename T1>
static T2 bit_cast(T1 t1) {
    static_assert(sizeof(T1) == sizeof(T2), "Types must match sizes");
    T2 t2;
    std::memcpy(std::addressof(t2), std::addressof(t1), sizeof(T1));  // pre c++20 type-punning
    return t2;
}

template <typename T, typename LT, typename UT>
[[nodiscard]] constexpr T clamp(T const& v, LT const& lb, UT const& ub) noexcept {
    return (lb <= v) ? static_cast<T>(lb) : (v >= ub) ? static_cast<T>(ub) : v;
}

template <typename T>
[[nodiscard]] constexpr T abs(T val) noexcept {
    return val < T{} ? -val : val;
}

//// Multi-arg max
template <typename T>
[[nodiscard]] constexpr T&& max(T&& v) noexcept {
    return static_cast<T&&>(v);
}

template <typename T1, typename T2, typename... Ts>
[[nodiscard]] constexpr T1 max(T1 const& v1, T2 const& v2, Ts const&... tail) noexcept {
    return (v1 >= max(v2, tail...) ? v1 : static_cast<T1>(max(v2, tail...)));
}

template <typename... Ts>
[[nodiscard]] constexpr bool max(bool b1, bool b2, Ts... tail) noexcept {
    return b1 || max(b2, tail...);
}

//// Multi-arg min
template <typename T>
[[nodiscard]] constexpr T&& min(T&& v) noexcept {
    return static_cast<T&&>(v);
}

template <typename T1, typename T2, typename... Ts>
[[nodiscard]] constexpr T1 min(T1 const& v1, T2 const& v2, Ts const&... tail) noexcept {
    return v1 <= min(v2, tail...) ? v1 : static_cast<T1>(min(v2, tail...));
}

template <typename... Ts>
[[nodiscard]] constexpr bool min(bool b1, bool b2, Ts... tail) noexcept {
    return b1 && min(b2, tail...);
}

// Condition test operations
template <typename T, typename O>
bool any(T const& container, O&& op) {
    for (auto const& e : container)
        if (op(e))
            return true;
    return false;
}

template <typename T, typename O>
bool all(T const& container, O&& op) {
    for (auto const& e : container)
        if (!op(e))
            return false;
    return true;
}

template <typename T>
size_t size(T const& container) {
    return container.size();
}

template <typename T, size_t N>
size_t size(T const (& /*unused*/)[N]) {
    return N;
}

struct IdentityFtor {
    template <typename T>
    T&& operator()(T&& t) const noexcept {
        return std::forward<T>(t);
    }
};

}  // namespace cav

#endif /* CAV_INCLUDE_UTIL_FUNCTIONS_HPP */
