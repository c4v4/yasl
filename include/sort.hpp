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

#include <cassert>
#include <cstring>
#include <memory>

#include "Span.hpp"
#include "net_sort.hpp"
#include "radix_sort.hpp"
#include "sort_utils.hpp"

#ifndef NDEBUG
#include <algorithm>

#include "limits.hpp"
#endif

namespace cav {

/// @brief A class to sort and find the nth element of a container, it keeps a buffer for
/// performance and it works with key instead of compartors.
template <typename SzT = uint32_t, typename AlcT = std::allocator<char>>
struct Sorter {
    static_assert(std::is_integral<SzT>::value, "SzT must be an integral type");

    using size_type  = SzT;
    using alloc_type = AlcT;

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

    //////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////// NTH ELEMENT ////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////

    /////////////////////////// DUTCH FLAG NTH ELEMENT ///////////////////////////////////
private:
    template <typename C, typename V, typename K>
    void _three_partition(C& container, V mid, size_type mid_start, K key) {
        auto      buffer = _get_span<sort::value_t<C>>(size(container));
        size_type front = 0, back = size(container) - 1;
        for (auto& elem : container) {
            auto      k   = to_uint(key(elem));
            size_type idx = k < mid ? front++ : k > mid ? back-- : mid_start++;
            move_uninit(buffer[idx], elem);
        }
        move_uninit_span(container, buffer);
    }

public:
    template <typename C, typename K = IdentityFtor>
    void dutch_nth_elem(C& container, size_type nth, K key = {}) {
        constexpr size_type n_bytes  = sizeof(sort::key_t<C, K>);
        size_type           nth_copy = nth;

        size_type counters[256] = {};
        auto      key_buff      = _get_span<sort::ukey_t<C, K>>(size(container));
        for (size_type i = 0; i < size(container); ++i) {
            key_buff[i] = to_uint(key(container[i]));
            ++counters[nth_byte<n_bytes - 1U>(key_buff[i])];
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
                uint8_t k       = nth_byte(key_buff[j], n_bytes - b - 1U);
                uint8_t next_k  = nth_byte(key_buff[j], max(2U, n_bytes - b) - 2U);
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
                move_uninit(cont1[j], cont2[j]);
            if (++i >= Nm)
                return;
            for (size_type j = begs[i]; j < ends[i]; ++j)
                move_uninit(cont2[j], cont1[j]);
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
        constexpr size_type n_bytes = sizeof(sort::key_t<C1, K>);

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
            uint8_t k      = nth_byte(to_uint(key(elem)), b);
            uint8_t next_k = nth_byte(to_uint(key(elem)), max(1U, b) - 1U);
            assert(beg[b + 1] <= counters[active_c][k] && counters[active_c][k] < end[b + 1]);
            move_uninit(cont2[counters[active_c][k]], elem);
            ++counters[active_c][k];
            counters[!active_c][next_k] += (k == median);
        }
        end[b] = counters[active_c][median];
    }

public:
    template <typename C, typename K = IdentityFtor>
    void radix_nth_elem(C& container, size_type nth, K key = {}) {
        constexpr size_type n_bytes = sizeof(sort::key_t<C, K>);

        size_type counters[2][256] = {};
        for (auto const& elem : container)
            ++counters[0][nth_byte<n_bytes - 1U>(to_uint(key(elem)))];

        auto      val_buff          = _get_span<sort::value_t<C>>(size(container));
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

public:
    template <typename C, typename K = IdentityFtor>
    void radix_sort_lsd(C& container, K key = {}) {
        auto val_buff = _get_span<sort::value_t<C>>(size(container));
        cav::radix_sort_lsd<size_type>(container, val_buff, key);
    }

    template <typename C, typename K = IdentityFtor>
    void radix_sort_msd(C& container, K key = {}) {
        auto val_buff = _get_span<sort::value_t<C>>(size(container));
        cav::radix_sort_msd<size_type>(container, val_buff, key);
    }

    template <typename C, typename K = IdentityFtor>
    void net_sort(C& container, K key = {}) {
        if (size(container) <= CAV_MAX_NET_SIZE)
            return net_dispatch(container, key);

        auto buffer = _get_span<sort::value_t<C>>(size(container));
        cav::net_sort<size_type>(container, buffer, key);
    }

    template <typename C, typename K = IdentityFtor>
    void sort(C& container, K key = {}) {
        assert(size(container) < limits<size_type>::max() && "Container size exceeds SizeT max");

        if (std::is_same<sort::value_t<C>, sort::key_t<C, K>>::value && std::is_empty<K>::value)
            if (size(container) < sizeof(sort::key_t<C, K>) * 24)  // Native types
                net_sort(container, key);
            else
                radix_sort_lsd(container, key);

        else if (size(container) < sizeof(sort::key_t<C, K>) * 12)
            insertion_sort(container, key);
        else if (sizeof(sort::key_t<C, K>) < 8 || size(container) < (1ULL << 20U))
            radix_sort_lsd(container, key);
        else
            radix_sort_msd(container, key);

        assert(
            std::is_sorted(std::begin(container), std::end(container), sort::make_comp_wrap(key)));
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
            assert(to_uint(key(container[i])) <= to_uint(key(container[nth])));
        for (size_type i = nth; i < size(container); ++i)
            assert(to_uint(key(container[i])) >= to_uint(key(container[nth])));
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
