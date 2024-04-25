// SPDX-FileCopyrightText: 2024 Francesco Cavaliere <francescocava95@gmail.com>
// SPDX-License-Identifier: MIT

#ifndef CAV_INCLUDE_SORT_UTILS_HPP
#define CAV_INCLUDE_SORT_UTILS_HPP

#include <algorithm>
#include <cstdint>

#include "util_functions.hpp"

namespace cav {
/////////////////////////// KEYS CONVERSION ///////////////////////////

static uint8_t to_uint(uint8_t k) noexcept {
    return k;
}

static uint16_t to_uint(uint16_t k) noexcept {
    return k;
}

static uint32_t to_uint(uint32_t k) noexcept {
    return k;
}

static uint64_t to_uint(uint64_t k) noexcept {
    return k;
}

static uint8_t to_uint(int8_t k) noexcept {
    return static_cast<uint8_t>(k) + static_cast<uint8_t>(1U << 7U);
}

static uint16_t to_uint(int16_t k) noexcept {
    return static_cast<uint16_t>(k) + static_cast<uint16_t>(1U << 15U);
}

static uint32_t to_uint(int32_t k) noexcept {
    return static_cast<uint32_t>(k) + (1U << 31U);
}

static uint64_t to_uint(int64_t k) noexcept {
    return static_cast<uint64_t>(k) + (1ULL << 63U);
}

static uint32_t to_uint(float f) noexcept {
    auto unsgn     = bit_cast<uint32_t>(f);
    auto sign_mask = static_cast<uint32_t>(-static_cast<int32_t>(unsgn >> 31U));
    return unsgn ^ (sign_mask | (1U << 31U));
}

static uint64_t to_uint(double d) noexcept {
    auto unsgn     = bit_cast<uint64_t>(d);
    auto sign_mask = static_cast<uint64_t>(-static_cast<int64_t>(unsgn >> 63U));
    return unsgn ^ (sign_mask | (1ULL << 63U));
}

namespace sort {
    ///////// SHORTHAND TEMPLATE ALIASES FOR SORTED TYPES METADATA //////////
    template <typename C>
    using value_t = container_value_type_t<C>;

    template <typename C, typename K>
    using key_t = no_cvr<decltype(std::declval<K>()(std::declval<value_t<C>>()))>;

    template <typename C, typename K>
    using ukey_t = no_cvr<decltype(to_uint(std::declval<key_t<C, K>>()))>;

    template <typename K>
    struct CompWrap {
        K key;

        template <typename T>
        bool operator()(T const& v1, T const& v2) {
            return key(v1) < key(v2);
        }
    };

    template <typename K>
    CompWrap<K> make_comp_wrap(K key) noexcept {
        return CompWrap<K>{key};
    }

}  // namespace sort

template <size_t N, typename T>
static uint8_t nth_byte(T k) noexcept {
    return static_cast<uint8_t>(k >> 8U * N);
}

template <typename T>
static uint8_t nth_byte(T k, uint8_t n) noexcept {
    return static_cast<uint8_t>(k >> 8U * n);
}

/// @brief Moves src to dest. Assumes that dest refers to uninitialized memory.
template <typename D, typename S>
static auto move_uninit(D& dest, S& src) -> CAV_REQUIRES(!std::is_trivially_copyable<D>::value) {
    ::new (std::addressof(dest)) D(std::move(src));
    src.~S();
}

/// @brief Moves src to dest, optimized for trivially copiable types.
template <typename D, typename S>
static auto move_uninit(D& dest, S& src) -> CAV_REQUIRES(std::is_trivially_copyable<D>::value) {
    static_assert(sizeof(D) == sizeof(S), "Size mismatch");
    std::memcpy(std::addressof(dest), std::addressof(src), sizeof(D));
}

/// @brief Moves src span to dest span. Assumes that dest refers to uninitialized memory.
template <typename D, typename S>
static auto move_uninit_span(D&& dest, S&& src)
    -> CAV_REQUIRES(!std::is_trivially_copyable<sort::value_t<S>>::value) {
    for (size_t i = 0; i < size(src); ++i)
        move_uninit(dest[i], src[i]);
}

/// @brief Moves src span to dest span, optimized for trivially copiable types.
template <typename D, typename S>
static auto move_uninit_span(D&& dest, S&& src)
    -> CAV_REQUIRES(std::is_trivially_copyable<sort::value_t<S>>::value) {
    static_assert(std::is_same<sort::value_t<D>, sort::value_t<S>>::value, "Values mismatch");
    std::memcpy(std::addressof(dest[0]),
                std::addressof(src[0]),
                sizeof(sort::value_t<S>) * size(src));
}

/////////////////////////////// INSERTION SORT ///////////////////////////////////////
template <typename C, typename K = IdentityFtor>
static void insertion_sort(C& cont, K key = {}) {
    if (size(cont) < 2)
        return;

    auto cbeg = std::begin(cont);
    for (auto it = cbeg + 1; it != std::end(cont); ++it) {
        auto ikey = key(*it);
        if (ikey < (key(*cbeg))) {
            auto val = std::move(*it);
            std::move_backward(cbeg, it, it + 1);
            *cbeg = std::move(val);
        } else {
            auto val  = std::move(*it);
            auto next = it;
            while (ikey < key(*(next - 1))) {
                *next = std::move(*(next - 1));
                --next;
            }
            *next = std::move(val);
        }
    }

    // reference implementation
    // std::__insertion_sort(std::begin(cont),
    //                       std::end(cont),
    //                       __gnu_cxx::__ops::__iter_comp_iter(make_comp_wrap(key)));
}


}  // namespace cav

#endif /* CAV_INCLUDE_SORT_UTILS_HPP */
