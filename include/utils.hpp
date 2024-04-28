// SPDX-FileCopyrightText: 2024 Francesco Cavaliere <francescocava95@gmail.com>
// SPDX-License-Identifier: MIT

#ifndef CAV_INCLUDE_UTIL_FUNCTIONS_HPP
#define CAV_INCLUDE_UTIL_FUNCTIONS_HPP

#include <cstring>
#include <iterator>
#include <utility>

namespace cav {

#define CAV_REQUIRES(...)      typename std::enable_if<(__VA_ARGS__)>::type
#define CAV_REQUIRES_T(T, ...) typename std::enable_if<(__VA_ARGS__), T>::type

template <typename T>
using no_cvr = typename std::remove_cv<typename std::remove_reference<T>::type>::type;

template <typename T>
size_t size(T const& container) {
    return container.size();
}

template <typename T, size_t N>
size_t size(T const (& /*unused*/)[N]) {
    return N;
}

template <typename C>
struct container_metadata {
    using iterator   = decltype(std::begin(std::declval<C>()));
    using value_type = no_cvr<decltype(*std::declval<iterator>())>;
    using size_type  = decltype(::cav::size(std::declval<C>()));
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
T clamp(T const& v, LT const& lb, UT const& ub) {
    return (v < lb) ? static_cast<T>(lb) : (v > ub) ? static_cast<T>(ub) : v;
}

template <typename T>
T abs(T val) {
    return val < T{} ? -val : val;
}

/// Multi-arg max. NOTE: to avoid ambiguity, return type is always the first argument type
template <typename T1, typename T2>
constexpr T1 max(T1 v1, T2 v2) {
    return v1 > static_cast<T1>(v2) ? v1 : static_cast<T1>(v2);
}

template <typename T1, typename T2, typename... Ts>
T1 max(T1 v1, T2 v2, Ts... tail) {
    T1 mtail = max<T1>(v2, tail...);
    return (v1 >= mtail ? v1 : mtail);
}

/// Multi-arg min. NOTE: to avoid ambiguity, return type is always the first argument type
template <typename T1, typename T2>
constexpr T1 min(T1 v1, T2 v2) {
    return v1 < static_cast<T1>(v2) ? v1 : static_cast<T1>(v2);
}

template <typename T1, typename T2, typename... Ts>
T1 min(T1 v1, T2 v2, Ts... tail) {
    T1 mtail = min<T1>(v2, tail...);
    return v1 <= mtail ? v1 : mtail;
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

struct IdentityFtor {
    template <typename T>
    T&& operator()(T&& t) const noexcept {
        return std::forward<T>(t);
    }
};

template <typename C, typename K = IdentityFtor>
bool is_sorted(C const& container, K key = IdentityFtor{}) {
    if (cav::size(container) <= 1)
        return true;
    auto prev_key = key(*std::begin(container));
    for (auto const& elem : container) {
        auto elem_key = key(elem);
        if (elem_key < prev_key)
            return false;
        prev_key = elem_key;
    }
    return true;
}

template <typename C, typename K = IdentityFtor>
bool is_nth_elem(C const& container, size_t nth, K key = IdentityFtor{}) {
    if (cav::size(container) <= 1)
        return true;
    for (size_t i = 0; i < nth; ++i)
        if (key(container[i]) > key(container[nth]))
            return false;
    for (size_t i = nth; i < cav::size(container); ++i)
        if (key(container[i]) < key(container[nth]))
            return false;
    return true;
}

#ifdef NDEBUG
#define assert_sorted(...)   static_cast<void>(0)
#define assert_nth_elem(...) static_cast<void>(0)

#else
template <typename C, typename K = IdentityFtor>
void assert_sorted(C const& container, K key = IdentityFtor{}) {
    if (cav::size(container) <= 1)
        return;
    auto prev_key = key(*std::begin(container));
    for (auto const& elem : container) {
        auto elem_key = key(elem);
        assert(elem_key >= prev_key);
        prev_key = elem_key;
    }
}

template <typename C, typename K = IdentityFtor>
void assert_nth_elem(C const& container, size_t nth, K key = IdentityFtor{}) {
    if (cav::size(container) <= 1)
        return;
    for (size_t i = 0; i < nth; ++i)
        assert(key(container[i]) <= key(container[nth]));
    for (size_t i = nth; i < cav::size(container); ++i)
        assert(key(container[i]) >= key(container[nth]));
}
#endif


}  // namespace cav

#endif /* CAV_INCLUDE_UTIL_FUNCTIONS_HPP */
