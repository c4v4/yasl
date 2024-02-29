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

#ifndef CFT_INCLUDE_RADIX_STUFF_HPP
#define CFT_INCLUDE_RADIX_STUFF_HPP

#include <algorithm>
#include <cstdint>
#include <cstring>

#include "../include/sorting_networks.hpp"
#include "Span.hpp"
#include "util_functions.hpp"

namespace cft {

/// @brief A class to sort and find the nth element of a container, keeping a
/// buffer for performance and working with key instead of compartors.
template <typename SzT = uint32_t, typename AlcT = std::allocator<char>>
struct Sorter : AlcT {
    using size_type  = SzT;
    using alloc_type = AlcT;

    char*  cache_buff = nullptr;
    size_t buff_size  = 0;

    Sorter() = default;

    Sorter(char* buff, size_t sz, AlcT const& alc = {})
        : AlcT(alc)
        , cache_buff(buff)
        , buff_size(sz) {
    }

    ~Sorter() {
        alloc_type::deallocate(cache_buff, buff_size);
    }

    template <typename C>
    using value_t = container_value_type_t<C>;

    template <typename C, typename K>
    using key_t = no_cvr<decltype(std::declval<K>()(std::declval<value_t<C>>()))>;

private:
    template <typename D, typename S>
    auto _move_uninit(D& dest, S&& src) -> CFT_REQUIRES(!std::is_trivially_copyable<D>::value) {
        new (std::addressof(dest)) D(std::forward<S>(src));
        src.~S();
    }

    template <typename D, typename S>
    auto _move_uninit(D& dest, S&& src) -> CFT_REQUIRES(std::is_trivially_copyable<D>::value) {
        static_assert(sizeof(D) == sizeof(S), "Size mismatch");
        std::memcpy(std::addressof(dest), std::addressof(src), sizeof(D));
    }

    /// @brief Provide a working buffer maintained between calls to avoid
    /// reallocations
    template <typename T>
    Span<T*> _get_span(size_t sz) {
        size_t char_sz = sz * sizeof(T);
        if (char_sz > buff_size) {
            alloc_type::deallocate(cache_buff, buff_size);
            cache_buff = alloc_type::allocate(char_sz);
            buff_size  = char_sz;
        }
        return make_span(reinterpret_cast<T*>(cache_buff), sz);
    }

    /////////////////////////// KEYS CONVERSION ///////////////////////////

    static uint8_t _to_uint(uint8_t k) noexcept {
        return k;
    }

    static uint16_t _to_uint(uint16_t k) noexcept {
        return k;
    }

    static uint32_t _to_uint(uint32_t k) noexcept {
        return k;
    }

    static uint64_t _to_uint(uint64_t k) noexcept {
        return k;
    }

    static uint8_t _to_uint(int8_t k) noexcept {
        return static_cast<uint8_t>(k) + static_cast<uint8_t>(1U << 7U);
    }

    static uint16_t _to_uint(int16_t k) noexcept {
        return static_cast<uint16_t>(k) + static_cast<uint16_t>(1U << 15U);
    }

    static uint32_t _to_uint(int32_t k) noexcept {
        return static_cast<uint32_t>(k) + (1U << 31U);
    }

    static uint64_t _to_uint(int64_t k) noexcept {
        return static_cast<uint64_t>(k) + (1ULL << 63U);
    }

    static uint32_t _to_uint(float f) noexcept {
        auto unsgn     = bit_cast<uint32_t>(f);
        auto sign_mask = static_cast<uint32_t>(-static_cast<int32_t>(unsgn >> 31U));
        return unsgn ^ (sign_mask | (1U << 31U));
    }

    static uint64_t _to_uint(double d) noexcept {
        auto unsgn     = bit_cast<uint64_t>(d);
        auto sign_mask = static_cast<uint64_t>(-static_cast<int64_t>(unsgn >> 63U));
        return unsgn ^ (sign_mask | (1ULL << 63U));
    }

    ////////////////////////////////////////////////////////////////////////////
    //////////////////////////////// RADIX SORT ////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////

    template <typename C1, typename C2, typename K>
    void _byte_sort(C1& cont1, C2& cont2, K&& key, size_t b, uint32_t (&counters)[2][256]) {
        bool     active_c = b & 1U;
        uint32_t tot      = 0;
        for (size_t i = 0; i < 256; ++i) {
            uint32_t old_count     = counters[active_c][i];
            counters[active_c][i]  = tot;
            counters[!active_c][i] = 0;
            tot += old_count;
        }

        for (auto& elem : cont1) {
            auto k = _to_uint(key(elem)) >> (b * 8U);
            assert(counters[active_c][static_cast<uint8_t>(k)] < size(cont2));
            _move_uninit(cont2[counters[active_c][static_cast<uint8_t>(k)]], elem);
            ++counters[active_c][static_cast<uint8_t>(k)];
            ++counters[!active_c][static_cast<uint8_t>(k >> 8U)];
        }
    }

public:
    template <typename C, typename K = IdentityFtor>
    void radix_sort(C& container, K&& key = {}) {
        constexpr size_t n_bytes = sizeof(key_t<C, K>);

        uint32_t counters[2][256] = {};  // counters are computed later to reduce stack usage
        for (auto const& elem : container)
            ++counters[0][static_cast<uint8_t>(_to_uint(key(elem)))];

        auto val_buff = _get_span<value_t<C>>(size(container));
        for (uint32_t b = 0; b < n_bytes; ++b) {
            _byte_sort(container, val_buff, key, b, counters);
            if (++b == n_bytes) {
                std::move(std::begin(val_buff), std::end(val_buff), std::begin(container));
                return;
            }
            _byte_sort(val_buff, container, key, b, counters);
        }
    }

    ////////////////////////////////////////////////////////////////////////////
    //////////////////////////////// SMALL SORT ////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////

    /////////////////////// SORTING NETWORKS SORT //////////////////////////////
private:
    template <typename C1, typename C2, typename C3, typename K>
    void _merge(C1 half1, C2 half2, C3 buffer, K&& key) {
#ifndef NDEBUG
        assert(size(half1) + size(half2) == size(buffer));
        assert(std::is_sorted(
            std::begin(half1),
            half1.end(),
            [&](value_t<C1> const& a, value_t<C1> const& b) { return key(a) < key(b); }));
        assert(std::is_sorted(
            std::begin(half2),
            half2.end(),
            [&](value_t<C1> const& a, value_t<C1> const& b) { return key(a) < key(b); }));
#endif

        size_t i = 0, j = 0, k = 0;
        if (size(half1) > 0 && size(half2) > 0) {
            auto ki = key(half1[i]), kj = key(half2[j]);
            for (;;) {  // ugly, but avoid redundant invocations to key()
                if (ki < kj) {
                    _move_uninit(buffer[k++], half1[i++]);
                    if (i == size(half1))
                        break;
                    ki = key(half1[i]);
                } else {
                    _move_uninit(buffer[k++], half2[j++]);
                    if (j == size(half2))
                        break;
                    kj = key(half2[j]);
                }
            }
        }
        while (i < size(half1))
            _move_uninit(buffer[k++], half1[i++]);
        while (j < size(half2))
            _move_uninit(buffer[k++], half2[j++]);

        assert(std::is_sorted(
            std::begin(buffer),
            buffer.end(),
            [&](value_t<C1> const& a, value_t<C1> const& b) { return key(a) < key(b); }));
    }

#ifndef NDEBUG
#define NET_SIZE 8U
#else
#define NET_SIZE 32U
#endif

    template <typename C, typename K = IdentityFtor>
    void _net_dispatch(C& container, K&& key = {}) {
        switch (size(container)) {
        case 0:
        case 1:
            return;
#if NET_SIZE > 1
        case 2:
            return netsort::s2(container, key);
#if NET_SIZE > 2
        case 3:
            return netsort::s3(container, key);
#if NET_SIZE > 3
        case 4:
            return netsort::s4(container, key);
#if NET_SIZE > 4
        case 5:
            return netsort::s5(container, key);
#if NET_SIZE > 5
        case 6:
            return netsort::s6(container, key);
#if NET_SIZE > 6
        case 7:
            return netsort::s7(container, key);
#if NET_SIZE > 7
        case 8:
            return netsort::s8(container, key);
#if NET_SIZE > 8
        case 9:
            return netsort::s9(container, key);
#if NET_SIZE > 9
        case 10:
            return netsort::s10(container, key);
#if NET_SIZE > 10
        case 11:
            return netsort::s11(container, key);
#if NET_SIZE > 11
        case 12:
            return netsort::s12(container, key);
#if NET_SIZE > 12
        case 13:
            return netsort::s13(container, key);
#if NET_SIZE > 13
        case 14:
            return netsort::s14(container, key);
#if NET_SIZE > 14
        case 15:
            return netsort::s15(container, key);
#if NET_SIZE > 15
        case 16:
            return netsort::s16(container, key);
#if NET_SIZE > 16
        case 17:
            return netsort::s17(container, key);
#if NET_SIZE > 17
        case 18:
            return netsort::s18(container, key);
#if NET_SIZE > 18
        case 19:
            return netsort::s19(container, key);
#if NET_SIZE > 19
        case 20:
            return netsort::s20(container, key);
#if NET_SIZE > 20
        case 21:
            return netsort::s21(container, key);
#if NET_SIZE > 21
        case 22:
            return netsort::s22(container, key);
#if NET_SIZE > 22
        case 23:
            return netsort::s23(container, key);
#if NET_SIZE > 23
        case 24:
            return netsort::s24(container, key);
#if NET_SIZE > 24
        case 25:
            return netsort::s25(container, key);
#if NET_SIZE > 25
        case 26:
            return netsort::s26(container, key);
#if NET_SIZE > 26
        case 27:
            return netsort::s27(container, key);
#if NET_SIZE > 27
        case 28:
            return netsort::s28(container, key);
#if NET_SIZE > 28
        case 29:
            return netsort::s29(container, key);
#if NET_SIZE > 29
        case 30:
            return netsort::s30(container, key);
#if NET_SIZE > 30
        case 31:
            return netsort::s31(container, key);
#if NET_SIZE > 31
        default:
            assert(size(container) == 32);
            return netsort::s32(container, key);
#endif
#endif
#endif
#endif
#endif
#endif
#endif
#endif
#endif
#endif
#endif
#endif
#endif
#endif
#endif
#endif
#endif
#endif
#endif
#endif
#endif
#endif
#endif
#endif
#endif
#endif
#endif
#endif
#endif
#endif
#endif
        }
    }

    template <typename C1, typename C2, typename K>
    size_t _chunks_merge(C1& cont1, C2& cont2, size_t curr_size, size_t old_residual, K&& key) {
        size_t csize = size(cont1);
        size_t i     = 0;
        for (; i < csize - curr_size; i += (2 * curr_size)) {
            size_t residual = min(curr_size, csize - i - curr_size);
            auto   piece1   = make_span(std::begin(cont1) + i, curr_size);
            auto   piece2   = make_span(std::begin(cont1) + i + curr_size, residual);
            auto   out      = make_span(std::begin(cont2) + i, size(piece1) + size(piece2));
            _merge(piece1, piece2, out, key);
        }

        if (i < csize) {  // manage residual
            auto piece1 = make_span(std::begin(cont1) + i, csize - i - old_residual);
            auto piece2 = make_span(std::begin(cont1) + i + size(piece1), old_residual);
            auto out    = make_span(std::begin(cont2) + i, size(piece1) + size(piece2));
            _merge(piece1, piece2, out, key);
            return csize - i;
        }

        return 0;
    }

public:
    template <typename C, typename K = IdentityFtor>
    void net_sort(C& container, K&& key = {}) {

        size_t csize = size(container);
        if (csize <= NET_SIZE)
            return _net_dispatch(container, key);

        for (size_t i = 0; i < csize; i += NET_SIZE) {
            size_t residual = min(NET_SIZE, csize - i);
            auto   chunk    = make_span(std::begin(container) + i, residual);
            _net_dispatch(chunk, key);
        }

        auto   buffer       = _get_span<value_t<C>>(csize);
        size_t curr_size    = NET_SIZE;
        size_t old_residual = 0;
        while (curr_size < csize) {

            old_residual = _chunks_merge(container, buffer, curr_size, old_residual, key);
            curr_size *= 2;
            if (curr_size >= csize) {
                for (size_t j = 0; j < csize; ++j)
                    _move_uninit(container[j], buffer[j]);
                return;
            }

            old_residual = _chunks_merge(buffer, container, curr_size, old_residual, key);
            curr_size *= 2;
        }
    }

    /////////////////////////////// INSERTION SORT
    //////////////////////////////////////////
    template <typename C, typename K = IdentityFtor>
    void insertion_sort(C& container, K&& key = {}) {

        auto cbeg = std::begin(container);
        auto buff = _get_span<container_value_type_t<C>>(size(container));
        auto b    = std::begin(buff);
        for (auto i = cbeg; i < std::end(container) - 1; ++i) {
            auto mink = key(*i);
            auto min  = i;
            for (auto j = i + 1; j < std::end(container); ++j) {
                min  = key(*j) < mink ? j : min;
                mink = key(*j) < mink ? key(*j) : mink;
            }
            _move_uninit(*b, *min);
            ++b;
        }
        std::move(std::begin(buff), std::end(buff), std::begin(container));
    }

    //////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////// NTH ELEMENT
    ///////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////

    /////////////////////////// DUTCH FLAG NTH ELEMENT
    //////////////////////////////////////
private:
    template <typename C, typename V, typename K>
    void _three_partition(C& container, V mid, size_t mid_start, K&& key) {
        auto   key_buff = _get_span<value_t<C>>(size(container));
        size_t front = 0, back = size(container) - 1;
        for (auto& elem : container) {
            auto   k   = _to_uint(key(elem));
            size_t idx = k < mid ? front++ : k > mid ? back-- : mid_start++;
            _move_uninit(key_buff[idx], elem);
        }
        std::move(std::begin(key_buff), std::end(key_buff), std::begin(container));
    }

public:
    template <typename C, typename K = IdentityFtor>
    void dutch_nth_elem(C& container, size_t nth, K&& key = {}) {
        using ukey_type           = no_cvr<decltype(_to_uint(std::declval<key_t<C, K>>()))>;
        constexpr size_t n_bytes  = sizeof(key_t<C, K>);
        size_t           nth_copy = nth;

        uint32_t counters[256] = {};
        auto     key_buff      = _get_span<ukey_type>(size(container));
        for (size_t i = 0; i < size(container); ++i) {
            key_buff[i] = _to_uint(key(container[i]));
            ++counters[static_cast<uint8_t>(key_buff[i] >> (n_bytes - 1U) * 8U)];
        }

        for (uint32_t b = 0; b < n_bytes; ++b) {

            uint8_t median      = 0;
            size_t  bucket_size = 0;
            for (size_t i = 0; i < 256; ++i) {
                if (nth_copy < counters[i]) {
                    bucket_size = counters[i];
                    median      = i;
                    break;
                }
                nth_copy -= counters[i];
            }
            for (auto& c : counters)
                c = 0;

            size_t   count      = 0;
            uint32_t shift      = (n_bytes - b - 1U) * 8;
            uint32_t next_shift = (max(2U, n_bytes - b) - 2U) * 8;
            for (size_t j = 0; count < bucket_size; ++j) {
                uint8_t k       = key_buff[j] >> shift;
                uint8_t next_k  = key_buff[j] >> next_shift;
                key_buff[count] = key_buff[j];
                counters[next_k] += (k == median);
                count += (k == median);
            }
            if (count == 1)
                break;
        }

        _three_partition(container, key_buff[0], nth - nth_copy, key);
    }

    ////////////////////////////// RADIX NTH ELEMENT
    ////////////////////////////////////////
private:
    template <typename C1, typename C2, size_t Nm>
    void _unwind_moves(C1& cont1, C2& cont2, size_t i, size_t (&beg)[Nm], size_t (&end)[Nm]) {
        for (; i < Nm; ++i) {
            for (size_t j = beg[i]; j < end[i]; ++j)
                _move_uninit(cont1[j], cont2[j]);
            if (++i >= Nm)
                return;
            for (size_t j = beg[i]; j < end[i]; ++j)
                _move_uninit(cont2[j], cont1[j]);
        }
    }

    template <typename C1, typename C2, typename K, size_t Nm>
    void _byte_nth_elem(C1&    cont1,
                        C2&    cont2,
                        size_t nth,
                        K&&    key,
                        size_t b,
                        uint32_t (&counters)[2][256],
                        size_t (&beg)[Nm],
                        size_t (&end)[Nm]) {

        constexpr size_t n_bytes = sizeof(key_t<C1, K>);

        bool     active_c = (n_bytes - 1 - b) & 1U;
        uint32_t tot      = beg[b + 1];
        size_t   median   = 0;
        for (size_t i = 0; i < 256; ++i) {
            if (tot <= nth)
                median = i;
            uint32_t old_count     = counters[active_c][i];
            counters[active_c][i]  = tot;
            counters[!active_c][i] = 0;
            tot += old_count;
        }
        beg[b] = counters[active_c][median];

        uint32_t shift      = b * 8;
        uint32_t next_shift = (max(1U, b) - 1U) * 8;
        for (size_t j = beg[b + 1]; j < end[b + 1]; ++j) {
            auto&   elem   = cont1[j];
            uint8_t k      = _to_uint(key(elem)) >> shift;
            uint8_t next_k = _to_uint(key(elem)) >> next_shift;
            assert(beg[b + 1] <= counters[active_c][k] && counters[active_c][k] < end[b + 1]);
            _move_uninit(cont2[counters[active_c][k]], elem);
            ++counters[active_c][k];
            counters[!active_c][next_k] += (k == median);
        }
        end[b] = counters[active_c][median];
    }

public:
    template <typename C, typename K = IdentityFtor>
    void radix_nth_elem(C& container, size_t nth, K&& key = {}) {
        constexpr size_t n_bytes = sizeof(key_t<C, K>);

        uint32_t counters[2][256] = {};
        for (auto const& elem : container)
            ++counters[0][static_cast<uint8_t>(_to_uint(key(elem)) >> (n_bytes - 1U) * 8U)];

        auto   val_buff         = _get_span<value_t<C>>(size(container));
        size_t beg[n_bytes + 1] = {}, end[n_bytes + 1] = {};
        end[n_bytes] = size(container);

        for (size_t b = n_bytes - 1;;) {
            _byte_nth_elem(container, val_buff, nth, key, b, counters, beg, end);
            if (end[b] - beg[b] == 1 || b == 0)
                return _unwind_moves(container, val_buff, b + 1, beg, end);
            --b;
            _byte_nth_elem(val_buff, container, nth, key, b, counters, beg, end);
            if (end[b] - beg[b] == 1 || b == 0)
                return _unwind_moves(val_buff, container, b + 1, beg, end);
            --b;
        }
    }

    //////////////////////////////////////////////////////////////////////////////////////
    //////////////////////// ADAPTIVE SORT AND NTH ELEMENT
    //////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////

    template <typename C, typename K = IdentityFtor>
    auto sort(C& container, K&& key = {})
        -> CFT_REQUIRES(std::is_same<value_t<C>, key_t<C, K>>::value) {

        // Best effort to detect identity key
        if (key(container[0]) < container[0] || key(container[0]) > container[0])
            if (size(container) < sizeof(key_t<C, K>) * 12)
                insertion_sort(container, key);
            else
                radix_sort(container, key);
        else if (size(container) < sizeof(key_t<C, K>) * 24)
            net_sort(container, key);
        else
            radix_sort(container, key);

        assert(std::is_sorted(
            std::begin(container),
            std::end(container),
            [&](value_t<C> const& a, value_t<C> const& b) { return key(a) < key(b); }));
    }

    template <typename C, typename K = IdentityFtor>
    auto sort(C& container, K&& key = {})
        -> CFT_REQUIRES(!std::is_same<value_t<C>, key_t<C, K>>::value) {

        if (size(container) < sizeof(key_t<C, K>) * 12)
            insertion_sort(container, key);
        else
            radix_sort(container, key);

        assert(std::is_sorted(
            std::begin(container),
            std::end(container),
            [&](value_t<C> const& a, value_t<C> const& b) { return key(a) < key(b); }));
    }

    template <typename C, typename K = IdentityFtor>
    void nth_element(C& container, size_t nth, K&& key = {}) {
        if (size(container) < 48U)
            sort(container, key);
        else
            radix_nth_elem(container, nth, key);  // Faster for smaller types

#ifndef NDEBUG
        for (size_t i = 0; i < nth; ++i)
            assert(_to_uint(key(container[i])) <= _to_uint(key(container[nth])));
        for (size_t i = nth; i < size(container); ++i)
            assert(_to_uint(key(container[i])) >= _to_uint(key(container[nth])));
#endif
    }
};

template <typename SzT = uint32_t, typename AlcT = std::allocator<char>>
inline Sorter<SzT, AlcT> make_sorter() {
    return Sorter<SzT, AlcT>{};
}

}  // namespace cft

#endif /* CFT_INCLUDE_RADIX_STUFF_HPP */
