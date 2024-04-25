// SPDX-FileCopyrightText: 2024 Francesco Cavaliere <francescocava95@gmail.com>
// SPDX-License-Identifier: MIT

#ifndef CAV_INCLUDE_RADIX_SORT_HPP
#define CAV_INCLUDE_RADIX_SORT_HPP

#ifndef NDEBUG
#include <algorithm>
#endif
#include <cassert>
#include <cstdint>

#include "sort_utils.hpp"
#include "util_functions.hpp"

namespace cav {

////////////////////////////////////////////////////////////////////////////
//////////////////////////////// RADIX SORT ////////////////////////////////
////////////////////////////////////////////////////////////////////////////
namespace {
    template <typename SzT, typename C1, typename C2, typename K, size_t Nb>
    SzT byte_sort_lsd(C1& cont1, C2& cont2, K key, SzT b, SzT (&counters)[256], SzT (&nnz)[Nb]) {

        for (auto& elem : cont1) {
            auto k = nth_byte(to_uint(key(elem)), b);
            assert(counters[k] < size(cont2));
            move_uninit(cont2[counters[k]], elem);
            ++counters[k];
        }

        for (;;)
            if (++b == Nb || nnz[b] > 1)
                return b;
    }

    template <typename SzT>
    struct BegEnd {
        SzT beg;
        SzT end;
    };

    template <typename SzT, typename C1, typename C2, typename K = IdentityFtor>
    BegEnd<SzT> byte_sort_msd(C1& cont, C2& buff, K key, uint8_t b, SzT (&counters)[256]) {
        if (size(cont) < sizeof(sort::key_t<C1, K>) * 12) {
            insertion_sort(cont, key);
            return {0, 0};
        }

        SzT beg = 0, end = 0;
        for (auto& elem : cont)
            ++counters[nth_byte(to_uint(key(elem)), b)];

        for (SzT accum = 0; accum < size(cont); ++end) {
            SzT old_count = counters[end];
            counters[end] = accum;
            beg           = accum == 0 ? end : beg;
            accum += old_count;
            assert(end <= 256 && beg <= end);
        }

        for (auto& elem : cont) {
            uint8_t k = nth_byte(to_uint(key(elem)), b);
            move_uninit(buff[counters[k]], elem);
            ++counters[k];
        }
        assert(counters[beg] > 0 && counters[end - 1] == size(cont));
        assert(std::is_sorted(std::begin(buff),
                              std::end(buff),
                              sort::make_comp_wrap([&](sort::value_t<C1> const& c) {
                                  return nth_byte(to_uint(key(c)), b);
                              })));
        return {beg, end};
    }
}  // namespace

template <typename SzT, typename C1, typename C2, typename K = IdentityFtor>
static void radix_sort_lsd(C1& container, C2& buff, K key = {}) {
    constexpr uint8_t n_bytes = sizeof(sort::key_t<C1, K>);

    SzT counters[n_bytes][256] = {};
    for (auto const& elem : container)
        for (uint8_t b = 0; b < n_bytes; ++b)
            ++counters[b][nth_byte(to_uint(key(elem)), b)];

    SzT accum[n_bytes] = {};
    SzT nnz[n_bytes]   = {};  // to skip bytes
    for (SzT i = 0; i < 256; ++i)
        for (uint8_t b = 0; b < n_bytes; ++b) {
            SzT old_count  = counters[b][i];
            counters[b][i] = accum[b];
            accum[b] += old_count;
            nnz[b] += old_count > 0;
        }

    SzT b = 0;
    for (; b < n_bytes;) {
        b = byte_sort_lsd(container, buff, key, b, counters[b], nnz);
        if (b == n_bytes)
            return move_uninit_span(container, buff);
        b = byte_sort_lsd(buff, container, key, b, counters[b], nnz);
    }
}

#define CHECK_CONT(cont, key) \
    assert(std::is_sorted(std::begin(cont), std::end(cont), sort::make_comp_wrap(key)))

template <typename SzT, typename C1, typename C2, typename K = IdentityFtor>
static void radix_sort_msd(C1&     cont,
                           C2&     buff,
                           K       key = {},
                           uint8_t b   = sizeof(sort::key_t<C1, K>) - 1) {
    constexpr uint8_t n_bytes = sizeof(sort::key_t<C1, K>);

    SzT  counts[256] = {};
    auto srng        = byte_sort_msd(cont, buff, key, b, counts);
    if (srng.end == 0)
        return CHECK_CONT(cont, key);
    assert(srng.beg < srng.end);

    if (b == 0) {
        move_uninit_span(cont, buff);
        return CHECK_CONT(cont, key);
    }

    SzT sub_beg = 0;
    for (SzT s = srng.beg; s < srng.end; sub_beg = counts[s++]) {
        if (sub_beg == counts[s])
            continue;
        SzT  sub_counts[256] = {};
        auto sub_buff        = make_span(buff, sub_beg, counts[s]);
        auto sub_cont        = make_span(cont, sub_beg, counts[s]);
        auto ssrng           = byte_sort_msd(sub_buff, sub_cont, key, b - 1, sub_counts);
        if (ssrng.end == 0) {
            move_uninit_span(sub_cont, sub_buff);
            continue;
        }
        assert(ssrng.beg < ssrng.end);

        if (b - 1 == 0)
            continue;

        SzT sub_sub_beg = 0;
        for (SzT ss = ssrng.beg; ss < ssrng.end; sub_sub_beg = sub_counts[ss++]) {
            auto sub_sub_cont = make_span(sub_cont, sub_sub_beg, sub_counts[ss]);
            if (size(sub_sub_cont) <= 4) {
                net_dispatch(sub_sub_cont, key);
                continue;
            }
            auto sub_sub_buff = make_span(sub_buff, sub_sub_beg, sub_counts[ss]);
            radix_sort_msd<SzT>(sub_sub_cont, sub_sub_buff, key, b - 2);
            assert(std::is_sorted(std::begin(sub_sub_cont),
                                  std::end(sub_sub_cont),
                                  sort::make_comp_wrap(key)));
        }
    }

    CHECK_CONT(cont, key);
}
}  // namespace cav

#endif /* CAV_INCLUDE_RADIX_SORT_HPP */
