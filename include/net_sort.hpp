// SPDX-FileCopyrightText: 2024 Francesco Cavaliere <francescocava95@gmail.com>
// SPDX-License-Identifier: MIT

#ifndef CAV_INCLUDE_NET_SORT_HPP
#define CAV_INCLUDE_NET_SORT_HPP

#ifndef NDEBUG
#define CAV_MAX_NET_SIZE 8U
#else
#define CAV_MAX_NET_SIZE 32U
#endif

#ifndef NDEBUG
#include <algorithm>
#endif
#include <cassert>

#include "sort_utils.hpp"
#include "sorting_networks.hpp"

namespace cav {
namespace {
    /////////////////////// SORTING NETWORKS SORT //////////////////////////////
    template <typename SzT, typename C1, typename C2, typename C3, typename K>
    void merge(C1 half1, C2 half2, C3 buffer, K key) {
        assert(std::is_sorted(std::begin(half1), std::end(half1), sort::make_comp_wrap(key)));
        assert(std::is_sorted(std::begin(half2), std::end(half2), sort::make_comp_wrap(key)));

        SzT i = 0, j = 0, k = 0;
        if (size(half1) > 0 && size(half2) > 0) {
            auto ki = key(half1[i]), kj = key(half2[j]);
            for (;;) {  // ugly, but avoids redundant invocations to key()
                if (ki < kj) {
                    move_uninit(buffer[k++], half1[i++]);
                    if (i == size(half1))
                        break;
                    ki = key(half1[i]);
                } else {
                    move_uninit(buffer[k++], half2[j++]);
                    if (j == size(half2))
                        break;
                    kj = key(half2[j]);
                }
            }
        }
        while (i < size(half1))
            move_uninit(buffer[k++], half1[i++]);
        while (j < size(half2))
            move_uninit(buffer[k++], half2[j++]);

        assert(std::is_sorted(std::begin(buffer), std::end(buffer), sort::make_comp_wrap(key)));
    }

    /// @brief Merges adjacent chunks pairs of size `curr_size` from `cont1` into `cont2`. The
    /// number of elements that could not be merged at the end is returned. NOTE: an assumption is
    /// made that `curr_size` is doubled between invocation of this function. In this way, the
    /// residual of the previous call can never exceed the new residual (i.e., the residual elements
    /// always grow until they can be merged).
    template <typename SzT, typename C1, typename C2, typename K>
    SzT chunks_merge(C1& cont1, C2& cont2, SzT curr_size, SzT old_residual, K key) {
        SzT csize = size(cont1);
        SzT i     = 0;
        for (; i < csize - curr_size; i += (2 * curr_size)) {
            SzT  residual = min(curr_size, csize - i - curr_size);
            auto piece1   = make_span(std::begin(cont1) + i, curr_size);
            auto piece2   = make_span(std::begin(cont1) + i + curr_size, residual);
            auto out      = make_span(std::begin(cont2) + i, size(piece1) + size(piece2));
            merge<SzT>(piece1, piece2, out, key);
        }

        if (i < csize) {  // manage residual
            auto piece1 = make_span(std::begin(cont1) + i, csize - i - old_residual);
            auto piece2 = make_span(std::begin(cont1) + i + size(piece1), old_residual);
            auto out    = make_span(std::begin(cont2) + i, size(piece1) + size(piece2));
            merge<SzT>(piece1, piece2, out, key);
            return csize - i;
        }

        return 0;
    }
}  // namespace

template <typename SzT, typename C1, typename C2, typename K>
static void net_sort(C1& container, C2& buffer, K key = {}) {
    SzT csize = size(container);

    for (SzT i = 0; i < csize; i += CAV_MAX_NET_SIZE) {
        SzT  residual = min(CAV_MAX_NET_SIZE, csize - i);
        auto chunk    = make_span(std::begin(container) + i, residual);
        net_dispatch(chunk, key);
    }

    SzT curr_size    = CAV_MAX_NET_SIZE;
    SzT old_residual = 0;
    while (curr_size < csize) {

        old_residual = chunks_merge<SzT>(container, buffer, curr_size, old_residual, key);
        curr_size *= 2;
        if (curr_size >= csize) {
            move_uninit_span(container, buffer);
            return;
        }

        old_residual = chunks_merge<SzT>(buffer, container, curr_size, old_residual, key);
        curr_size *= 2;
    }
}
}  // namespace cav

#endif /* CAV_INCLUDE_NET_SORT_HPP */
