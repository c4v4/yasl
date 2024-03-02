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

#ifndef CAV_INCLUDE_RADIX_STUFF_HPP
#define CAV_INCLUDE_RADIX_STUFF_HPP

#include <sys/types.h>

#include <algorithm>
#include <cstring>
#include <memory>

#include "Span.hpp"
#include "sorting_networks.hpp"
#include "util_functions.hpp"

#ifndef NDEBUG
#include "limits.hpp"
#endif

namespace cav {

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

/// @brief A class to sort and find the nth element of a container, keeping a buffer for performance
/// and working with key instead of compartors.
template <typename SzT = uint32_t, typename AlcT = std::allocator<char>>
struct Sorter {
    static_assert(std::is_integral<SzT>::value, "SzT must be an integral type");

    using size_type  = SzT;
    using alloc_type = AlcT;

    ///////// SHORTHAND TEMPLATE ALIASES FOR SORTED TYPES METADATA //////////
    template <typename C>
    using value_t = container_value_type_t<C>;

    template <typename C, typename K>
    using key_t = no_cvr<decltype(std::declval<K>()(std::declval<value_t<C>>()))>;

    template <typename C, typename K>
    using ukey_t = no_cvr<decltype(_to_uint(std::declval<key_t<C, K>>()))>;

    ///////// Internal buffer to speed up sorting operations + EBO for the allocator /////////
    struct SorterData : alloc_type {
        SorterData()                                 = default;
        SorterData(SorterData const&)                = delete;
        SorterData(SorterData&&) noexcept            = default;
        SorterData& operator=(SorterData const&)     = delete;
        SorterData& operator=(SorterData&&) noexcept = default;

        SorterData(alloc_type const& alc)
            : alloc_type(alc) {
        }

        ~SorterData() {
            std::allocator_traits<alloc_type>::deallocate(*this, cache_buff, buff_size);
        }

        char* get_sized_buff(size_type char_sz) {
            if (char_sz > buff_size) {
                std::allocator_traits<alloc_type>::deallocate(*this, cache_buff, buff_size);
                cache_buff = std::allocator_traits<alloc_type>::allocate(*this, char_sz);
                buff_size  = char_sz;
            }
            return cache_buff;
        }

        char* cache_buff = nullptr;
        SzT   buff_size  = 0;
    } data;

private:
    /// @brief Provide a working buffer maintained between calls to avoid reallocations
    template <typename T>
    Span<T*> _get_span(size_type sz) {
        return make_span(reinterpret_cast<T*>(data.get_sized_buff(sz * sizeof(T))), sz);
    }

    /// @brief Moves src to dest. Assumes that dest refers to uninitialized memory.
    template <typename D, typename S>
    static auto _move_uninit(D& dest, S& src)
        -> CAV_REQUIRES(!std::is_trivially_copyable<D>::value) {
        ::new (std::addressof(dest)) D(std::move(src));
        src.~S();
    }

    /// @brief Moves src to dest, optimized for trivially copiable types.
    template <typename D, typename S>
    static auto _move_uninit(D& dest, S& src)
        -> CAV_REQUIRES(std::is_trivially_copyable<D>::value) {
        static_assert(sizeof(D) == sizeof(S), "Size mismatch");
        std::memcpy(std::addressof(dest), std::addressof(src), sizeof(D));
    }

    /// @brief Moves src span to dest span. Assumes that dest refers to uninitialized memory.
    template <typename D, typename S>
    static auto _move_uninit_span(D& dest, S& src)
        -> CAV_REQUIRES(!std::is_trivially_copyable<value_t<S>>::value) {
        for (size_type i = 0; i < size(src); ++i)
            _move_uninit(dest[i], src[i]);
    }

    /// @brief Moves src span to dest span, optimized for trivially copiable types.
    template <typename D, typename S>
    static auto _move_uninit_span(D& dest, S& src)
        -> CAV_REQUIRES(std::is_trivially_copyable<value_t<S>>::value) {
        static_assert(std::is_same<value_t<D>, value_t<S>>::value, "Values mismatch");
        std::memcpy(std::addressof(dest[0]),
                    std::addressof(src[0]),
                    sizeof(value_t<S>) * size(src));
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

    template <size_t N, typename T>
    static uint8_t _nth_byte(T k) noexcept {
        return static_cast<uint8_t>(k >> 8U * N);
    }

    template <typename T>
    static uint8_t _nth_byte(T k, uint8_t n) noexcept {
        return static_cast<uint8_t>(k >> 8U * n);
    }

    ////////////////////////////////////////////////////////////////////////////
    //////////////////////////////// RADIX SORT ////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////

    template <typename C1, typename C2, typename K, size_t Nb>
    static size_type _byte_sort(C1&       cont1,
                                C2&       cont2,
                                K         key,
                                size_type b,
                                size_type (&counters)[256],
                                size_type (&nnz)[Nb]) {

        for (auto& elem : cont1) {
            auto k = _nth_byte(_to_uint(key(elem)), b);
            assert(counters[k] < size(cont2));
            _move_uninit(cont2[counters[k]], elem);
            ++counters[k];
        }

        for (;;)
            if (++b == Nb || nnz[b] > 1)
                return b;
    }

    template <typename C1, typename C2, typename K = IdentityFtor>
    static void _radix_sort_impl(C1& container, C2& buff, K key = {}) {
        constexpr uint8_t n_bytes = sizeof(key_t<C1, K>);

        size_type counters[n_bytes][256] = {};
        for (auto const& elem : container)
            for (uint8_t b = 0; b < n_bytes; ++b)
                ++counters[b][_nth_byte(_to_uint(key(elem)), b)];

        size_type accum[n_bytes] = {};
        size_type nnz[n_bytes]   = {};
        for (size_type i = 0; i < 256; ++i)
            for (uint8_t b = 0; b < n_bytes; ++b) {
                size_type old_count = counters[b][i];
                counters[b][i]      = accum[b];
                accum[b] += old_count;
                nnz[b] += old_count > 0;
            }

        size_type b = 0;
        for (; b < n_bytes;) {
            b = _byte_sort(container, buff, key, b, counters[b], nnz);
            if (b == n_bytes)
                return _move_uninit_span(container, buff);
            b = _byte_sort(buff, container, key, b, counters[b], nnz);
        }
    }

    struct BegEnd {
        size_type beg;
        size_type end;
    };

    template <typename C1, typename C2, typename K = IdentityFtor>
    static BegEnd _byte_sort2(C1& cont, C2& buff, K key, uint8_t b, size_type (&counters)[256]) {
        if (size(cont) < sizeof(key_t<C1, K>) * 12) {
            insertion_sort(cont, key);
            return {0, 0};
        }

        size_type beg = 0, end = 0;
        for (auto& elem : cont)
            ++counters[_nth_byte(_to_uint(key(elem)), b)];

        for (size_type accum = 0; accum < size(cont); ++end) {
            size_type old_count = counters[end];
            counters[end]       = accum;
            beg                 = accum == 0 ? end : beg;
            accum += old_count;
            assert(end <= 256 && beg <= end);
        }

        for (auto& elem : cont) {
            uint8_t k = _nth_byte(_to_uint(key(elem)), b);
            _move_uninit(buff[counters[k]], elem);
            ++counters[k];
        }
        assert(counters[beg] > 0 && counters[end - 1] == size(cont));
        assert(std::is_sorted(
            std::begin(buff),
            std::end(buff),
            make_comp_wrap([&](value_t<C1> const& c) { return _nth_byte(_to_uint(key(c)), b); })));
        return {beg, end};
    }

#define CHECK_CONT(cont, key) \
    assert(std::is_sorted(std::begin(cont), std::end(cont), make_comp_wrap(key)))

    template <typename C1, typename C2, typename K = IdentityFtor>
    static void _radix_sort_impl2(C1&     cont,
                                  C2&     buff,
                                  K       key = {},
                                  uint8_t b   = sizeof(key_t<C1, K>) - 1) {
        constexpr uint8_t n_bytes = sizeof(key_t<C1, K>);

        size_type counts[256] = {};
        auto      srng        = _byte_sort2(cont, buff, key, b, counts);
        if (srng.end == 0)
            return CHECK_CONT(cont, key);
        assert(srng.beg < srng.end);

        if (b == 0) {
            _move_uninit_span(cont, buff);
            return CHECK_CONT(cont, key);
        }

        size_type sub_beg = 0;
        for (size_type s = srng.beg; s < srng.end; sub_beg = counts[s++]) {
            size_type sub_size = counts[s] - sub_beg;
            if (sub_size == 0)
                continue;
            size_type sub_counts[256] = {};
            auto      sub_buff        = make_span(std::begin(buff) + sub_beg, sub_size);
            auto      sub_cont        = make_span(std::begin(cont) + sub_beg, sub_size);
            auto      ssrng           = _byte_sort2(sub_buff, sub_cont, key, b - 1, sub_counts);
            if (ssrng.end == 0) {
                _move_uninit_span(sub_cont, sub_buff);
                continue;
            }
            assert(ssrng.beg < ssrng.end);

            if (b - 1 == 0)
                continue;

            size_type sub_sub_beg = 0;
            for (size_type ss = ssrng.beg; ss < ssrng.end; sub_sub_beg = sub_counts[ss++]) {
                size_type sub_sub_size = sub_counts[ss] - sub_sub_beg;
                auto sub_sub_cont = make_span(std::begin(sub_cont) + sub_sub_beg, sub_sub_size);
                if (sub_sub_size <= 4) {
                    net_dispatch(sub_sub_cont, key);
                    continue;
                }
                auto sub_sub_buff = make_span(std::begin(sub_buff) + sub_sub_beg, sub_sub_size);
                _radix_sort_impl2(sub_sub_cont, sub_sub_buff, key, b - 2);
                assert(std::is_sorted(std::begin(sub_sub_cont),
                                      std::end(sub_sub_cont),
                                      make_comp_wrap(key)));
            }
        }

        CHECK_CONT(cont, key);
    }

    ////////////////////////////////////////////////////////////////////////////
    //////////////////////////////// SMALL SORT ////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////

    /////////////////////// SORTING NETWORKS SORT //////////////////////////////
    template <typename C1, typename C2, typename C3, typename K>
    static void _merge(C1 half1, C2 half2, C3 buffer, K key) {
        assert(std::is_sorted(std::begin(half1), std::end(half1), make_comp_wrap(key)));
        assert(std::is_sorted(std::begin(half2), std::end(half2), make_comp_wrap(key)));

        size_type i = 0, j = 0, k = 0;
        if (size(half1) > 0 && size(half2) > 0) {
            auto ki = key(half1[i]), kj = key(half2[j]);
            for (;;) {  // ugly, but avoids redundant invocations to key()
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

        assert(std::is_sorted(std::begin(buffer), std::end(buffer), make_comp_wrap(key)));
    }

    /// @brief Merges adjacent chunks pairs of size `curr_size` from `cont1` into `cont2`. The
    /// number of elements that could not be merged at the end is returned. NOTE: an assumption is
    /// made that `curr_size` is doubled between invocation of this function. In this way, the
    /// residual of the previous call can never exceed the new residual (i.e., the residual elements
    /// always grow until they can be merged).
    template <typename C1, typename C2, typename K>
    static size_type _chunks_merge(C1&       cont1,
                                   C2&       cont2,
                                   size_type curr_size,
                                   size_type old_residual,
                                   K         key) {
        size_type csize = size(cont1);
        size_type i     = 0;
        for (; i < csize - curr_size; i += (2 * curr_size)) {
            size_type residual = min(curr_size, csize - i - curr_size);
            auto      piece1   = make_span(std::begin(cont1) + i, curr_size);
            auto      piece2   = make_span(std::begin(cont1) + i + curr_size, residual);
            auto      out      = make_span(std::begin(cont2) + i, size(piece1) + size(piece2));
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

    template <typename C1, typename C2, typename K>
    static void _net_sort_impl(C1& container, C2& buffer, K key = {}) {
        size_type csize = size(container);

        for (size_type i = 0; i < csize; i += CAV_MAX_NET_SIZE) {
            size_type residual = min(CAV_MAX_NET_SIZE, csize - i);
            auto      chunk    = make_span(std::begin(container) + i, residual);
            net_dispatch(chunk, key);
        }

        size_type curr_size    = CAV_MAX_NET_SIZE;
        size_type old_residual = 0;
        while (curr_size < csize) {

            old_residual = _chunks_merge(container, buffer, curr_size, old_residual, key);
            curr_size *= 2;
            if (curr_size >= csize) {
                _move_uninit_span(container, buffer);
                return;
            }

            old_residual = _chunks_merge(buffer, container, curr_size, old_residual, key);
            curr_size *= 2;
        }
    }

    //////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////// NTH ELEMENT ////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////

    /////////////////////////// DUTCH FLAG NTH ELEMENT ///////////////////////////////////
private:
    template <typename C, typename V, typename K>
    void _three_partition(C& container, V mid, size_type mid_start, K key) {
        auto      buffer = _get_span<value_t<C>>(size(container));
        size_type front = 0, back = size(container) - 1;
        for (auto& elem : container) {
            auto      k   = _to_uint(key(elem));
            size_type idx = k < mid ? front++ : k > mid ? back-- : mid_start++;
            _move_uninit(buffer[idx], elem);
        }
        _move_uninit_span(container, buffer);
    }

public:
    template <typename C, typename K = IdentityFtor>
    void dutch_nth_elem(C& container, size_type nth, K key = {}) {
        constexpr size_type n_bytes  = sizeof(key_t<C, K>);
        size_type           nth_copy = nth;

        size_type counters[256] = {};
        auto      key_buff      = _get_span<ukey_t<C, K>>(size(container));
        for (size_type i = 0; i < size(container); ++i) {
            key_buff[i] = _to_uint(key(container[i]));
            ++counters[_nth_byte<n_bytes - 1U>(key_buff[i])];
        }

        for (size_type b = 0; b < n_bytes; ++b) {
            uint8_t   median      = 0;
            size_type bucket_size = 0;
            for (size_type i = 0; i < 256; ++i) {
                if (nth_copy < counters[i]) {
                    bucket_size = counters[i];
                    median      = i;
                    break;
                }
                nth_copy -= counters[i];
            }
            for (auto& c : counters)
                c = 0;

            size_type count = 0;
            for (size_type j = 0; count < bucket_size; ++j) {
                uint8_t k       = _nth_byte(key_buff[j], n_bytes - b - 1U);
                uint8_t next_k  = _nth_byte(key_buff[j], max(2U, n_bytes - b) - 2U);
                key_buff[count] = key_buff[j];
                counters[next_k] += (k == median);
                count += (k == median);
            }
            if (count == 1)
                break;
        }

        _three_partition(container, key_buff[0], nth - nth_copy, key);
    }

    ////////////////////////////// RADIX NTH ELEMENT /////////////////////////////////////
private:
    template <typename C1, typename C2, size_type Nm>
    static void _unwind_moves(C1&       cont1,
                              C2&       cont2,
                              size_type i,
                              size_type (&begs)[Nm],
                              size_type (&ends)[Nm]) {
        for (; i < Nm; ++i) {
            for (size_type j = begs[i]; j < ends[i]; ++j)
                _move_uninit(cont1[j], cont2[j]);
            if (++i >= Nm)
                return;
            for (size_type j = begs[i]; j < ends[i]; ++j)
                _move_uninit(cont2[j], cont1[j]);
        }
    }

    template <typename C1, typename C2, typename K, size_type Nm>
    static void _byte_nth_elem(C1&       cont1,
                               C2&       cont2,
                               size_type nth,
                               K         key,
                               size_type b,
                               size_type (&counters)[2][256],
                               size_type (&beg)[Nm],
                               size_type (&end)[Nm]) {
        constexpr size_type n_bytes = sizeof(key_t<C1, K>);

        bool      active_c = (n_bytes - 1 - b) & 1U;
        size_type total    = beg[b + 1];
        size_type median   = 0;
        for (size_type i = 0; i < 256; ++i) {
            size_type old_count    = counters[active_c][i];
            counters[active_c][i]  = total;
            counters[!active_c][i] = 0;
            median                 = total <= nth ? i : median;
            total += old_count;
        }
        beg[b] = counters[active_c][median];

        for (size_type j = beg[b + 1]; j < end[b + 1]; ++j) {
            auto&   elem   = cont1[j];
            uint8_t k      = _nth_byte(_to_uint(key(elem)), b);
            uint8_t next_k = _nth_byte(_to_uint(key(elem)), max(1U, b) - 1U);
            assert(beg[b + 1] <= counters[active_c][k] && counters[active_c][k] < end[b + 1]);
            _move_uninit(cont2[counters[active_c][k]], elem);
            ++counters[active_c][k];
            counters[!active_c][next_k] += (k == median);
        }
        end[b] = counters[active_c][median];
    }

public:
    template <typename C, typename K = IdentityFtor>
    void radix_nth_elem(C& container, size_type nth, K key = {}) {
        constexpr size_type n_bytes = sizeof(key_t<C, K>);

        size_type counters[2][256] = {};
        for (auto const& elem : container)
            ++counters[0][_nth_byte<n_bytes - 1U>(_to_uint(key(elem)))];

        auto      val_buff          = _get_span<value_t<C>>(size(container));
        size_type begs[n_bytes + 1] = {}, ends[n_bytes + 1] = {};
        ends[n_bytes] = size(container);

        for (size_type b = n_bytes - 1;;) {
            _byte_nth_elem(container, val_buff, nth, key, b, counters, begs, ends);
            if (ends[b] - begs[b] == 1 || b == 0)
                return _unwind_moves(container, val_buff, b + 1, begs, ends);
            --b;
            _byte_nth_elem(val_buff, container, nth, key, b, counters, begs, ends);
            if (ends[b] - begs[b] == 1 || b == 0)
                return _unwind_moves(val_buff, container, b + 1, begs, ends);
            --b;
        }
    }

    //////////////////////////////////////////////////////////////////////////////////////
    //////////////////////// ADAPTIVE SORT AND NTH ELEMENT ///////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////

private:
    template <typename C1, typename C2, typename K>
    static void _sort_impl(C1& container, C2& buff, K key) {

        // Best effort to detect native-types sorting
        if (std::is_same<value_t<C1>, key_t<C1, K>>::value && std::is_empty<K>::value)
            if (size(container) < sizeof(key_t<C1, K>) * 24)  // Native types
                _net_sort_impl(container, buff, key);
            else
                _radix_sort_impl(container, buff, key);

        else if (size(container) < sizeof(key_t<C1, K>) * 12)
            insertion_sort(container, key);
        else if (sizeof(key_t<C1, K>) < 8 || size(container) < (1ULL << 20U))
            _radix_sort_impl(container, buff, key);
        else
            _radix_sort_impl2(container, buff, key);

        assert(std::is_sorted(std::begin(container), std::end(container), make_comp_wrap(key)));
    }

public:
    template <typename C, typename K = IdentityFtor>
    void radix_sort(C& container, K key = {}) {
        auto val_buff = _get_span<value_t<C>>(size(container));
        _radix_sort_impl(container, val_buff, key);
    }

    template <typename C, typename K = IdentityFtor>
    void radix_sort2(C& container, K key = {}) {
        auto val_buff = _get_span<value_t<C>>(size(container));
        _radix_sort_impl2(container, val_buff, key);
    }

    template <typename C, typename K = IdentityFtor>
    void net_sort(C& container, K key = {}) {
        if (size(container) <= CAV_MAX_NET_SIZE)
            return net_dispatch(container, key);

        auto buffer = _get_span<value_t<C>>(size(container));
        _net_sort_impl(container, buffer, key);
    }

    /////////////////////////////// INSERTION SORT ///////////////////////////////////////
    template <typename C, typename K = IdentityFtor>
    static void insertion_sort(C& cont, K key = {}) {
        if (size(cont) < 2)
            return;
        if (size(cont) < 3)
            return netsort::s2(cont, key);

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

    template <typename C, typename K = IdentityFtor>
    void sort(C& container, K key = {}) {
        assert(size(container) < limits<size_type>::max() && "Container size exceeds SizeT max");

        if (std::is_same<value_t<C>, key_t<C, K>>::value && std::is_empty<K>::value) {
            if (size(container) < sizeof(key_t<C, K>) * 24)  // Native types
                return net_sort(container, key);
        } else if (size(container) < sizeof(key_t<C, K>) * 12)
            return insertion_sort(container, key);

        auto buff = _get_span<value_t<C>>(size(container));
        _sort_impl(container, buff, key);

        assert(std::is_sorted(std::begin(container), std::end(container), make_comp_wrap(key)));
    }

    template <typename C, typename K = IdentityFtor>
    void nth_element(C& container, size_type nth, K key = {}) {
        assert(size(container) < limits<size_type>::max() && "Container size exceeds SizeT max");
        if (size(container) < 48U)
            sort(container, key);
        else
            radix_nth_elem(container, nth, key);  // Faster for smaller types

#ifndef NDEBUG
        for (size_type i = 0; i < nth; ++i)
            assert(_to_uint(key(container[i])) <= _to_uint(key(container[nth])));
        for (size_type i = nth; i < size(container); ++i)
            assert(_to_uint(key(container[i])) >= _to_uint(key(container[nth])));
#endif
    }
};

template <typename SzT = uint32_t, typename AlcT = std::allocator<char>>
inline Sorter<SzT, AlcT> make_sorter() {
    return Sorter<SzT, AlcT>();
}

template <typename SzT = uint32_t, typename AlcT = std::allocator<char>>
inline Sorter<SzT, no_cvr<AlcT>> make_sorter(AlcT&& allocator) {
    return Sorter<SzT, no_cvr<AlcT>>{std::forward<AlcT>(allocator)};
}

}  // namespace cav

#endif /* CAV_INCLUDE_RADIX_STUFF_HPP */
