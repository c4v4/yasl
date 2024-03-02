// Copyright (c) 2022 Francesco Cavaliere
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.


/// @brief All the smallest sorting networks up to an
/// input size of 32 are implemented.
/// Souce: https://bertdobbelaere.github.io/sorting_networks.html
///
/// Notice that including all networks increase substantially the size of the binary,
/// so consider what kind of small-sorting you **really** need.
///
/// Some tests on a AMD Ryzen 9 5950X.
///
/// Full 32 cases tried with vector of increasing size.
/// The numbers are milliseconds (to keep the duration similar, bigger the vectors, lesser they are
/// in number)
///
/// Sz: stl sort         ,   jump-table    ,  switch version
///    2: Std::sort: 4157,   jump sort: 771,  switch sort: 1104 <-- Huge code penalize small sizes
///    3: Std::sort: 4925,   jump sort: 529,  switch sort: 885
///    4: Std::sort: 5446,   jump sort: 507,  switch sort: 722
///    5: Std::sort: 5960,   jump sort: 496,  switch sort: 668
///    6: Std::sort: 6529,   jump sort: 489,  switch sort: 633
///    7: Std::sort: 7077,   jump sort: 515,  switch sort: 636
///    8: Std::sort: 7619,   jump sort: 542,  switch sort: 596
///    9: Std::sort: 8044,   jump sort: 627,  switch sort: 632
///    10: Std::sort: 8436,  jump sort: 637,  switch sort: 643
///    11: Std::sort: 8825,  jump sort: 698,  switch sort: 705
///    12: Std::sort: 9134,  jump sort: 735,  switch sort: 740
///    13: Std::sort: 9454,  jump sort: 770,  switch sort: 774
///    14: Std::sort: 9702,  jump sort: 815,  switch sort: 818
///    15: Std::sort: 9934,  jump sort: 836,  switch sort: 840
///    16: Std::sort: 10110, jump sort: 847,  switch sort: 849
///    17: Std::sort: 13075, jump sort: 921,  switch sort: 930
///    18: Std::sort: 13211, jump sort: 949,  switch sort: 951
///    19: Std::sort: 13427, jump sort: 987,  switch sort: 988
///    20: Std::sort: 13789, jump sort: 1010, switch sort: 1016
///    21: Std::sort: 14154, jump sort: 1060, switch sort: 1052
///    22: Std::sort: 14372, jump sort: 1107, switch sort: 1107
///    23: Std::sort: 14651, jump sort: 1121, switch sort: 1108
///    24: Std::sort: 14914, jump sort: 1564, switch sort: 1122
///    25: Std::sort: 15493, jump sort: 1219, switch sort: 1202
///    26: Std::sort: 15408, jump sort: 3437, switch sort: 1229 <-- jump-table bad
///    27: Std::sort: 15637, jump sort: 3802, switch sort: 1307
///    28: Std::sort: 15869, jump sort: 3766, switch sort: 1282
///    29: Std::sort: 16117, jump sort: 2674, switch sort: 1385
///    30: Std::sort: 16307, jump sort: 4980, switch sort: 1402
///    31: Std::sort: 16526, jump sort: 4540, switch sort: 1440
///    32: Std::sort: 16707, jump sort: 6497, switch sort: 1532
///    33: Std::sort: 16884, jump sort: 16687, switch sort: 16175 <-- threshold reached
///    34: Std::sort: 17080, jump sort: 16895, switch sort: 16353
///
///    With only sorting net up to size 4
///    2: Std::sort: 4181, jump sort: 686, switch sort: 593
///    3: Std::sort: 5039, jump sort: 529, switch sort: 495
///    4: Std::sort: 5429, jump sort: 520, switch sort: 454
///    5: Std::sort: 6001, jump sort: 6141, switch sort: 5953
///
///    With only sorting net up to size 8
///    2: Std::sort: 4292, jump sort: 668, switch sort: 515
///    3: Std::sort: 5020, jump sort: 527, switch sort: 458
///    4: Std::sort: 5516, jump sort: 522, switch sort: 421
///    5: Std::sort: 5985, jump sort: 495, switch sort: 453
///
///    With only sorting net up to size 16
///    2: Std::sort: 4157, jump sort: 676, switch sort: 524
///    3: Std::sort: 4968, jump sort: 544, switch sort: 540
///    4: Std::sort: 5479, jump sort: 514, switch sort: 421
///    5: Std::sort: 5992, jump sort: 509, switch sort: 508
///
///    With only sorting net up to size 20 <-- last aceptable case
///    2: Std::sort: 4196, jump sort: 670, switch sort: 497
///    3: Std::sort: 4909, jump sort: 544, switch sort: 540
///    4: Std::sort: 5475, jump sort: 516, switch sort: 403
///    5: Std::sort: 5999, jump sort: 497, switch sort: 433
///
///    With only sorting net up to size 24
///    2: Std::sort: 4183, jump sort: 666, switch sort: 1099
///    3: Std::sort: 4943, jump sort: 527, switch sort: 881
///    4: Std::sort: 5346, jump sort: 505, switch sort: 718
///    5: Std::sort: 5940, jump sort: 494, switch sort: 665
///
///    With only sorting net up to size 28
///    2: Std::sort: 4226, jump sort: 669, switch sort: 1105
///    3: Std::sort: 4974, jump sort: 528, switch sort: 885
///    4: Std::sort: 5451, jump sort: 507, switch sort: 722
///    5: Std::sort: 5963, jump sort: 508, switch sort: 677
///
///    With sorting net up to size 32 (all)
///    2: Std::sort: 4157, jump sort: 771, switch sort: 1104
///    3: Std::sort: 4925, jump sort: 529, switch sort: 885
///    4: Std::sort: 5446, jump sort: 507, switch sort: 722
///    5: Std::sort: 5960, jump sort: 496, switch sort: 668

#ifndef CAV_INCLUDE_UTILS_SORTING_NETWORKS_HPP
#define CAV_INCLUDE_UTILS_SORTING_NETWORKS_HPP

#include <cstring>
#include <type_traits>
#include <utility>

#include "util_functions.hpp"


#ifndef NDEBUG
#define CAV_MAX_NET_SIZE 8U
#else
#define CAV_MAX_NET_SIZE 32U
#endif

namespace cav {
namespace netsort {

    template <typename C, typename Key>
    auto cmp_swap(C& c, Key key, uint8_t i1, uint8_t i2)
        -> CAV_REQUIRES(std::is_fundamental<container_value_type_t<C>>::value ||
                        !std::is_trivially_copyable<container_value_type_t<C>>::value) {
        auto k1  = key(c[i1]);
        auto k2  = key(c[i2]);
        auto tmp = k1 < k2 ? std::move(c[i1]) : std::move(c[i2]);
        c[i2]    = k1 < k2 ? std::move(c[i2]) : std::move(c[i1]);
        c[i1]    = std::move(tmp);
    }

    template <typename C, typename Key>
    auto cmp_swap(C& c, Key key, uint8_t i1, uint8_t i2)
        -> CAV_REQUIRES(!std::is_fundamental<container_value_type_t<C>>::value &&
                        std::is_trivially_copyable<container_value_type_t<C>>::value) {
        if (key(c[i1]) > key(c[i2])) {
            char tmp[sizeof(container_value_type_t<C>)];
            std::memcpy(tmp, &c[i1], sizeof(tmp));
            std::memcpy(&c[i1], &c[i2], sizeof(tmp));
            std::memcpy(&c[i2], tmp, sizeof(tmp));
        }
    }

    template <typename C, typename Key>
    void s0(C& /*unused*/, Key /*unused*/) {
    }

    template <typename C, typename Key>
    void s1(C& /*unused*/, Key /*unused*/) {
    }

    template <typename C, typename Key>
    void s2(C& c, Key key) {
        cmp_swap(c, key, 0, 1);
    }

    template <typename C, typename Key>
    void s3(C& c, Key key) {
        cmp_swap(c, key, 0, 2);
        cmp_swap(c, key, 0, 1);
        cmp_swap(c, key, 1, 2);
    }

    template <typename C, typename Key>
    void s4(C& c, Key key) {
        cmp_swap(c, key, 0, 2);
        cmp_swap(c, key, 1, 3);
        cmp_swap(c, key, 0, 1);
        cmp_swap(c, key, 2, 3);
        cmp_swap(c, key, 1, 2);
    }

    template <typename C, typename Key>
    void s5(C& c, Key key) {
        cmp_swap(c, key, 0, 3);
        cmp_swap(c, key, 1, 4);
        cmp_swap(c, key, 0, 2);
        cmp_swap(c, key, 1, 3);
        cmp_swap(c, key, 0, 1);
        cmp_swap(c, key, 2, 4);
        cmp_swap(c, key, 1, 2);
        cmp_swap(c, key, 3, 4);
        cmp_swap(c, key, 2, 3);
    }

    template <typename C, typename Key>
    void s6(C& c, Key key) {
        cmp_swap(c, key, 0, 5);
        cmp_swap(c, key, 1, 3);
        cmp_swap(c, key, 2, 4);
        cmp_swap(c, key, 1, 2);
        cmp_swap(c, key, 3, 4);
        cmp_swap(c, key, 0, 3);
        cmp_swap(c, key, 2, 5);
        cmp_swap(c, key, 0, 1);
        cmp_swap(c, key, 2, 3);
        cmp_swap(c, key, 4, 5);
        cmp_swap(c, key, 1, 2);
        cmp_swap(c, key, 3, 4);
    }

    template <typename C, typename Key>
    void s7(C& c, Key key) {
        cmp_swap(c, key, 0, 6);
        cmp_swap(c, key, 2, 3);
        cmp_swap(c, key, 4, 5);
        cmp_swap(c, key, 0, 2);
        cmp_swap(c, key, 1, 4);
        cmp_swap(c, key, 3, 6);
        cmp_swap(c, key, 0, 1);
        cmp_swap(c, key, 2, 5);
        cmp_swap(c, key, 3, 4);
        cmp_swap(c, key, 1, 2);
        cmp_swap(c, key, 4, 6);
        cmp_swap(c, key, 2, 3);
        cmp_swap(c, key, 4, 5);
        cmp_swap(c, key, 1, 2);
        cmp_swap(c, key, 3, 4);
        cmp_swap(c, key, 5, 6);
    }

    template <typename C, typename Key>
    void s8(C& c, Key key) {
        cmp_swap(c, key, 0, 2);
        cmp_swap(c, key, 1, 3);
        cmp_swap(c, key, 4, 6);
        cmp_swap(c, key, 5, 7);
        cmp_swap(c, key, 0, 4);
        cmp_swap(c, key, 1, 5);
        cmp_swap(c, key, 2, 6);
        cmp_swap(c, key, 3, 7);
        cmp_swap(c, key, 0, 1);
        cmp_swap(c, key, 2, 3);
        cmp_swap(c, key, 4, 5);
        cmp_swap(c, key, 6, 7);
        cmp_swap(c, key, 2, 4);
        cmp_swap(c, key, 3, 5);
        cmp_swap(c, key, 1, 4);
        cmp_swap(c, key, 3, 6);
        cmp_swap(c, key, 1, 2);
        cmp_swap(c, key, 3, 4);
        cmp_swap(c, key, 5, 6);
    }

    template <typename C, typename Key>
    void s9(C& c, Key key) {
        cmp_swap(c, key, 0, 3);
        cmp_swap(c, key, 1, 7);
        cmp_swap(c, key, 2, 5);
        cmp_swap(c, key, 4, 8);
        cmp_swap(c, key, 0, 7);
        cmp_swap(c, key, 2, 4);
        cmp_swap(c, key, 3, 8);
        cmp_swap(c, key, 5, 6);
        cmp_swap(c, key, 0, 2);
        cmp_swap(c, key, 1, 3);
        cmp_swap(c, key, 4, 5);
        cmp_swap(c, key, 7, 8);
        cmp_swap(c, key, 1, 4);
        cmp_swap(c, key, 3, 6);
        cmp_swap(c, key, 5, 7);
        cmp_swap(c, key, 0, 1);
        cmp_swap(c, key, 2, 4);
        cmp_swap(c, key, 3, 5);
        cmp_swap(c, key, 6, 8);
        cmp_swap(c, key, 2, 3);
        cmp_swap(c, key, 4, 5);
        cmp_swap(c, key, 6, 7);
        cmp_swap(c, key, 1, 2);
        cmp_swap(c, key, 3, 4);
        cmp_swap(c, key, 5, 6);
    }

    template <typename C, typename Key>
    void s10(C& c, Key key) {
        cmp_swap(c, key, 0, 8);
        cmp_swap(c, key, 1, 9);
        cmp_swap(c, key, 2, 7);
        cmp_swap(c, key, 3, 5);
        cmp_swap(c, key, 4, 6);
        cmp_swap(c, key, 0, 2);
        cmp_swap(c, key, 1, 4);
        cmp_swap(c, key, 5, 8);
        cmp_swap(c, key, 7, 9);
        cmp_swap(c, key, 0, 3);
        cmp_swap(c, key, 2, 4);
        cmp_swap(c, key, 5, 7);
        cmp_swap(c, key, 6, 9);
        cmp_swap(c, key, 0, 1);
        cmp_swap(c, key, 3, 6);
        cmp_swap(c, key, 8, 9);
        cmp_swap(c, key, 1, 5);
        cmp_swap(c, key, 2, 3);
        cmp_swap(c, key, 4, 8);
        cmp_swap(c, key, 6, 7);
        cmp_swap(c, key, 1, 2);
        cmp_swap(c, key, 3, 5);
        cmp_swap(c, key, 4, 6);
        cmp_swap(c, key, 7, 8);
        cmp_swap(c, key, 2, 3);
        cmp_swap(c, key, 4, 5);
        cmp_swap(c, key, 6, 7);
        cmp_swap(c, key, 3, 4);
        cmp_swap(c, key, 5, 6);
    }

    template <typename C, typename Key>
    void s11(C& c, Key key) {
        cmp_swap(c, key, 0, 9);
        cmp_swap(c, key, 1, 6);
        cmp_swap(c, key, 2, 4);
        cmp_swap(c, key, 3, 7);
        cmp_swap(c, key, 5, 8);
        cmp_swap(c, key, 0, 1);
        cmp_swap(c, key, 3, 5);
        cmp_swap(c, key, 4, 10);
        cmp_swap(c, key, 6, 9);
        cmp_swap(c, key, 7, 8);
        cmp_swap(c, key, 1, 3);
        cmp_swap(c, key, 2, 5);
        cmp_swap(c, key, 4, 7);
        cmp_swap(c, key, 8, 10);
        cmp_swap(c, key, 0, 4);
        cmp_swap(c, key, 1, 2);
        cmp_swap(c, key, 3, 7);
        cmp_swap(c, key, 5, 9);
        cmp_swap(c, key, 6, 8);
        cmp_swap(c, key, 0, 1);
        cmp_swap(c, key, 2, 6);
        cmp_swap(c, key, 4, 5);
        cmp_swap(c, key, 7, 8);
        cmp_swap(c, key, 9, 10);
        cmp_swap(c, key, 2, 4);
        cmp_swap(c, key, 3, 6);
        cmp_swap(c, key, 5, 7);
        cmp_swap(c, key, 8, 9);
        cmp_swap(c, key, 1, 2);
        cmp_swap(c, key, 3, 4);
        cmp_swap(c, key, 5, 6);
        cmp_swap(c, key, 7, 8);
        cmp_swap(c, key, 2, 3);
        cmp_swap(c, key, 4, 5);
        cmp_swap(c, key, 6, 7);
    }

    template <typename C, typename Key>
    void s12(C& c, Key key) {
        cmp_swap(c, key, 0, 8);
        cmp_swap(c, key, 1, 7);
        cmp_swap(c, key, 2, 6);
        cmp_swap(c, key, 3, 11);
        cmp_swap(c, key, 4, 10);
        cmp_swap(c, key, 5, 9);
        cmp_swap(c, key, 0, 2);
        cmp_swap(c, key, 1, 4);
        cmp_swap(c, key, 3, 5);
        cmp_swap(c, key, 6, 8);
        cmp_swap(c, key, 7, 10);
        cmp_swap(c, key, 9, 11);
        cmp_swap(c, key, 0, 1);
        cmp_swap(c, key, 2, 9);
        cmp_swap(c, key, 4, 7);
        cmp_swap(c, key, 5, 6);
        cmp_swap(c, key, 10, 11);
        cmp_swap(c, key, 1, 3);
        cmp_swap(c, key, 2, 7);
        cmp_swap(c, key, 4, 9);
        cmp_swap(c, key, 8, 10);
        cmp_swap(c, key, 0, 1);
        cmp_swap(c, key, 2, 3);
        cmp_swap(c, key, 4, 5);
        cmp_swap(c, key, 6, 7);
        cmp_swap(c, key, 8, 9);
        cmp_swap(c, key, 10, 11);
        cmp_swap(c, key, 1, 2);
        cmp_swap(c, key, 3, 5);
        cmp_swap(c, key, 6, 8);
        cmp_swap(c, key, 9, 10);
        cmp_swap(c, key, 2, 4);
        cmp_swap(c, key, 3, 6);
        cmp_swap(c, key, 5, 8);
        cmp_swap(c, key, 7, 9);
        cmp_swap(c, key, 1, 2);
        cmp_swap(c, key, 3, 4);
        cmp_swap(c, key, 5, 6);
        cmp_swap(c, key, 7, 8);
        cmp_swap(c, key, 9, 10);
    }

    template <typename C, typename Key>
    void s13(C& c, Key key) {
        cmp_swap(c, key, 0, 11);
        cmp_swap(c, key, 1, 7);
        cmp_swap(c, key, 2, 4);
        cmp_swap(c, key, 3, 5);
        cmp_swap(c, key, 8, 9);
        cmp_swap(c, key, 10, 12);
        cmp_swap(c, key, 0, 2);
        cmp_swap(c, key, 3, 6);
        cmp_swap(c, key, 4, 12);
        cmp_swap(c, key, 5, 7);
        cmp_swap(c, key, 8, 10);
        cmp_swap(c, key, 0, 8);
        cmp_swap(c, key, 1, 3);
        cmp_swap(c, key, 2, 5);
        cmp_swap(c, key, 4, 9);
        cmp_swap(c, key, 6, 11);
        cmp_swap(c, key, 7, 12);
        cmp_swap(c, key, 0, 1);
        cmp_swap(c, key, 2, 10);
        cmp_swap(c, key, 3, 8);
        cmp_swap(c, key, 4, 6);
        cmp_swap(c, key, 9, 11);
        cmp_swap(c, key, 1, 3);
        cmp_swap(c, key, 2, 4);
        cmp_swap(c, key, 5, 10);
        cmp_swap(c, key, 6, 8);
        cmp_swap(c, key, 7, 9);
        cmp_swap(c, key, 11, 12);
        cmp_swap(c, key, 1, 2);
        cmp_swap(c, key, 3, 4);
        cmp_swap(c, key, 5, 8);
        cmp_swap(c, key, 6, 9);
        cmp_swap(c, key, 7, 10);
        cmp_swap(c, key, 2, 3);
        cmp_swap(c, key, 4, 7);
        cmp_swap(c, key, 5, 6);
        cmp_swap(c, key, 8, 11);
        cmp_swap(c, key, 9, 10);
        cmp_swap(c, key, 4, 5);
        cmp_swap(c, key, 6, 7);
        cmp_swap(c, key, 8, 9);
        cmp_swap(c, key, 10, 11);
        cmp_swap(c, key, 3, 4);
        cmp_swap(c, key, 5, 6);
        cmp_swap(c, key, 7, 8);
        cmp_swap(c, key, 9, 10);
    }

    template <typename C, typename Key>
    void s14(C& c, Key key) {
        cmp_swap(c, key, 0, 3);
        cmp_swap(c, key, 1, 9);
        cmp_swap(c, key, 2, 6);
        cmp_swap(c, key, 4, 12);
        cmp_swap(c, key, 5, 10);
        cmp_swap(c, key, 7, 11);
        cmp_swap(c, key, 8, 13);
        cmp_swap(c, key, 0, 2);
        cmp_swap(c, key, 3, 12);
        cmp_swap(c, key, 4, 5);
        cmp_swap(c, key, 6, 10);
        cmp_swap(c, key, 7, 8);
        cmp_swap(c, key, 11, 13);
        cmp_swap(c, key, 0, 1);
        cmp_swap(c, key, 2, 11);
        cmp_swap(c, key, 3, 6);
        cmp_swap(c, key, 4, 7);
        cmp_swap(c, key, 5, 9);
        cmp_swap(c, key, 10, 12);
        cmp_swap(c, key, 0, 4);
        cmp_swap(c, key, 1, 7);
        cmp_swap(c, key, 2, 5);
        cmp_swap(c, key, 3, 8);
        cmp_swap(c, key, 6, 13);
        cmp_swap(c, key, 9, 11);
        cmp_swap(c, key, 1, 2);
        cmp_swap(c, key, 3, 4);
        cmp_swap(c, key, 5, 7);
        cmp_swap(c, key, 6, 9);
        cmp_swap(c, key, 8, 10);
        cmp_swap(c, key, 12, 13);
        cmp_swap(c, key, 1, 3);
        cmp_swap(c, key, 2, 4);
        cmp_swap(c, key, 5, 9);
        cmp_swap(c, key, 6, 10);
        cmp_swap(c, key, 7, 8);
        cmp_swap(c, key, 11, 12);
        cmp_swap(c, key, 2, 3);
        cmp_swap(c, key, 4, 5);
        cmp_swap(c, key, 6, 7);
        cmp_swap(c, key, 8, 11);
        cmp_swap(c, key, 9, 10);
        cmp_swap(c, key, 12, 13);
        cmp_swap(c, key, 4, 6);
        cmp_swap(c, key, 5, 7);
        cmp_swap(c, key, 8, 9);
        cmp_swap(c, key, 10, 11);
        cmp_swap(c, key, 3, 4);
        cmp_swap(c, key, 5, 6);
        cmp_swap(c, key, 7, 8);
        cmp_swap(c, key, 9, 10);
        cmp_swap(c, key, 11, 12);
    }

    template <typename C, typename Key>
    void s15(C& c, Key key) {
        cmp_swap(c, key, 0, 6);
        cmp_swap(c, key, 1, 10);
        cmp_swap(c, key, 2, 14);
        cmp_swap(c, key, 3, 9);
        cmp_swap(c, key, 4, 12);
        cmp_swap(c, key, 5, 13);
        cmp_swap(c, key, 7, 11);
        cmp_swap(c, key, 0, 7);
        cmp_swap(c, key, 2, 5);
        cmp_swap(c, key, 3, 4);
        cmp_swap(c, key, 6, 11);
        cmp_swap(c, key, 8, 10);
        cmp_swap(c, key, 9, 12);
        cmp_swap(c, key, 13, 14);
        cmp_swap(c, key, 1, 13);
        cmp_swap(c, key, 2, 3);
        cmp_swap(c, key, 4, 6);
        cmp_swap(c, key, 5, 9);
        cmp_swap(c, key, 7, 8);
        cmp_swap(c, key, 10, 14);
        cmp_swap(c, key, 11, 12);
        cmp_swap(c, key, 0, 3);
        cmp_swap(c, key, 1, 4);
        cmp_swap(c, key, 5, 7);
        cmp_swap(c, key, 6, 13);
        cmp_swap(c, key, 8, 9);
        cmp_swap(c, key, 10, 11);
        cmp_swap(c, key, 12, 14);
        cmp_swap(c, key, 0, 2);
        cmp_swap(c, key, 1, 5);
        cmp_swap(c, key, 3, 8);
        cmp_swap(c, key, 4, 6);
        cmp_swap(c, key, 7, 10);
        cmp_swap(c, key, 9, 11);
        cmp_swap(c, key, 12, 13);
        cmp_swap(c, key, 0, 1);
        cmp_swap(c, key, 2, 5);
        cmp_swap(c, key, 3, 10);
        cmp_swap(c, key, 4, 8);
        cmp_swap(c, key, 6, 7);
        cmp_swap(c, key, 9, 12);
        cmp_swap(c, key, 11, 13);
        cmp_swap(c, key, 1, 2);
        cmp_swap(c, key, 3, 4);
        cmp_swap(c, key, 5, 6);
        cmp_swap(c, key, 7, 9);
        cmp_swap(c, key, 8, 10);
        cmp_swap(c, key, 11, 12);
        cmp_swap(c, key, 3, 5);
        cmp_swap(c, key, 4, 6);
        cmp_swap(c, key, 7, 8);
        cmp_swap(c, key, 9, 10);
        cmp_swap(c, key, 2, 3);
        cmp_swap(c, key, 4, 5);
        cmp_swap(c, key, 6, 7);
        cmp_swap(c, key, 8, 9);
        cmp_swap(c, key, 10, 11);
    }

    template <typename C, typename Key>
    void s16(C& c, Key key) {
        cmp_swap(c, key, 0, 5);
        cmp_swap(c, key, 1, 4);
        cmp_swap(c, key, 2, 12);
        cmp_swap(c, key, 3, 13);
        cmp_swap(c, key, 6, 7);
        cmp_swap(c, key, 8, 9);
        cmp_swap(c, key, 10, 15);
        cmp_swap(c, key, 11, 14);
        cmp_swap(c, key, 0, 2);
        cmp_swap(c, key, 1, 10);
        cmp_swap(c, key, 3, 6);
        cmp_swap(c, key, 4, 7);
        cmp_swap(c, key, 5, 14);
        cmp_swap(c, key, 8, 11);
        cmp_swap(c, key, 9, 12);
        cmp_swap(c, key, 13, 15);
        cmp_swap(c, key, 0, 8);
        cmp_swap(c, key, 1, 3);
        cmp_swap(c, key, 2, 11);
        cmp_swap(c, key, 4, 13);
        cmp_swap(c, key, 5, 9);
        cmp_swap(c, key, 6, 10);
        cmp_swap(c, key, 7, 15);
        cmp_swap(c, key, 12, 14);
        cmp_swap(c, key, 0, 1);
        cmp_swap(c, key, 2, 4);
        cmp_swap(c, key, 3, 8);
        cmp_swap(c, key, 5, 6);
        cmp_swap(c, key, 7, 12);
        cmp_swap(c, key, 9, 10);
        cmp_swap(c, key, 11, 13);
        cmp_swap(c, key, 14, 15);
        cmp_swap(c, key, 1, 3);
        cmp_swap(c, key, 2, 5);
        cmp_swap(c, key, 4, 8);
        cmp_swap(c, key, 6, 9);
        cmp_swap(c, key, 7, 11);
        cmp_swap(c, key, 10, 13);
        cmp_swap(c, key, 12, 14);
        cmp_swap(c, key, 1, 2);
        cmp_swap(c, key, 3, 5);
        cmp_swap(c, key, 4, 11);
        cmp_swap(c, key, 6, 8);
        cmp_swap(c, key, 7, 9);
        cmp_swap(c, key, 10, 12);
        cmp_swap(c, key, 13, 14);
        cmp_swap(c, key, 2, 3);
        cmp_swap(c, key, 4, 5);
        cmp_swap(c, key, 6, 7);
        cmp_swap(c, key, 8, 9);
        cmp_swap(c, key, 10, 11);
        cmp_swap(c, key, 12, 13);
        cmp_swap(c, key, 4, 6);
        cmp_swap(c, key, 5, 7);
        cmp_swap(c, key, 8, 10);
        cmp_swap(c, key, 9, 11);
        cmp_swap(c, key, 3, 4);
        cmp_swap(c, key, 5, 6);
        cmp_swap(c, key, 7, 8);
        cmp_swap(c, key, 9, 10);
        cmp_swap(c, key, 11, 12);
    }

    template <typename C, typename Key>
    void s17(C& c, Key key) {
        cmp_swap(c, key, 0, 11);
        cmp_swap(c, key, 1, 15);
        cmp_swap(c, key, 2, 10);
        cmp_swap(c, key, 3, 5);
        cmp_swap(c, key, 4, 6);
        cmp_swap(c, key, 8, 12);
        cmp_swap(c, key, 9, 16);
        cmp_swap(c, key, 13, 14);
        cmp_swap(c, key, 0, 6);
        cmp_swap(c, key, 1, 13);
        cmp_swap(c, key, 2, 8);
        cmp_swap(c, key, 4, 14);
        cmp_swap(c, key, 5, 15);
        cmp_swap(c, key, 7, 11);
        cmp_swap(c, key, 0, 8);
        cmp_swap(c, key, 3, 7);
        cmp_swap(c, key, 4, 9);
        cmp_swap(c, key, 6, 16);
        cmp_swap(c, key, 10, 11);
        cmp_swap(c, key, 12, 14);
        cmp_swap(c, key, 0, 2);
        cmp_swap(c, key, 1, 4);
        cmp_swap(c, key, 5, 6);
        cmp_swap(c, key, 7, 13);
        cmp_swap(c, key, 8, 9);
        cmp_swap(c, key, 10, 12);
        cmp_swap(c, key, 11, 14);
        cmp_swap(c, key, 15, 16);
        cmp_swap(c, key, 0, 3);
        cmp_swap(c, key, 2, 5);
        cmp_swap(c, key, 6, 11);
        cmp_swap(c, key, 7, 10);
        cmp_swap(c, key, 9, 13);
        cmp_swap(c, key, 12, 15);
        cmp_swap(c, key, 14, 16);
        cmp_swap(c, key, 0, 1);
        cmp_swap(c, key, 3, 4);
        cmp_swap(c, key, 5, 10);
        cmp_swap(c, key, 6, 9);
        cmp_swap(c, key, 7, 8);
        cmp_swap(c, key, 11, 15);
        cmp_swap(c, key, 13, 14);
        cmp_swap(c, key, 1, 2);
        cmp_swap(c, key, 3, 7);
        cmp_swap(c, key, 4, 8);
        cmp_swap(c, key, 6, 12);
        cmp_swap(c, key, 11, 13);
        cmp_swap(c, key, 14, 15);
        cmp_swap(c, key, 1, 3);
        cmp_swap(c, key, 2, 7);
        cmp_swap(c, key, 4, 5);
        cmp_swap(c, key, 9, 11);
        cmp_swap(c, key, 10, 12);
        cmp_swap(c, key, 13, 14);
        cmp_swap(c, key, 2, 3);
        cmp_swap(c, key, 4, 6);
        cmp_swap(c, key, 5, 7);
        cmp_swap(c, key, 8, 10);
        cmp_swap(c, key, 3, 4);
        cmp_swap(c, key, 6, 8);
        cmp_swap(c, key, 7, 9);
        cmp_swap(c, key, 10, 12);
        cmp_swap(c, key, 5, 6);
        cmp_swap(c, key, 7, 8);
        cmp_swap(c, key, 9, 10);
        cmp_swap(c, key, 11, 12);
        cmp_swap(c, key, 4, 5);
        cmp_swap(c, key, 6, 7);
        cmp_swap(c, key, 8, 9);
        cmp_swap(c, key, 10, 11);
        cmp_swap(c, key, 12, 13);
    }

    template <typename C, typename Key>
    void s18(C& c, Key key) {
        cmp_swap(c, key, 0, 1);
        cmp_swap(c, key, 2, 3);
        cmp_swap(c, key, 4, 5);
        cmp_swap(c, key, 6, 7);
        cmp_swap(c, key, 8, 9);
        cmp_swap(c, key, 10, 11);
        cmp_swap(c, key, 12, 13);
        cmp_swap(c, key, 14, 15);
        cmp_swap(c, key, 16, 17);
        cmp_swap(c, key, 1, 5);
        cmp_swap(c, key, 2, 6);
        cmp_swap(c, key, 3, 7);
        cmp_swap(c, key, 4, 10);
        cmp_swap(c, key, 8, 16);
        cmp_swap(c, key, 9, 17);
        cmp_swap(c, key, 12, 14);
        cmp_swap(c, key, 13, 15);
        cmp_swap(c, key, 0, 8);
        cmp_swap(c, key, 1, 10);
        cmp_swap(c, key, 2, 12);
        cmp_swap(c, key, 3, 14);
        cmp_swap(c, key, 6, 13);
        cmp_swap(c, key, 7, 15);
        cmp_swap(c, key, 9, 16);
        cmp_swap(c, key, 11, 17);
        cmp_swap(c, key, 0, 4);
        cmp_swap(c, key, 1, 9);
        cmp_swap(c, key, 5, 17);
        cmp_swap(c, key, 8, 11);
        cmp_swap(c, key, 10, 16);
        cmp_swap(c, key, 0, 2);
        cmp_swap(c, key, 1, 6);
        cmp_swap(c, key, 4, 10);
        cmp_swap(c, key, 5, 9);
        cmp_swap(c, key, 14, 16);
        cmp_swap(c, key, 15, 17);
        cmp_swap(c, key, 1, 2);
        cmp_swap(c, key, 3, 10);
        cmp_swap(c, key, 4, 12);
        cmp_swap(c, key, 5, 7);
        cmp_swap(c, key, 6, 14);
        cmp_swap(c, key, 9, 13);
        cmp_swap(c, key, 15, 16);
        cmp_swap(c, key, 3, 8);
        cmp_swap(c, key, 5, 12);
        cmp_swap(c, key, 7, 11);
        cmp_swap(c, key, 9, 10);
        cmp_swap(c, key, 3, 4);
        cmp_swap(c, key, 6, 8);
        cmp_swap(c, key, 7, 14);
        cmp_swap(c, key, 9, 12);
        cmp_swap(c, key, 11, 13);
        cmp_swap(c, key, 1, 3);
        cmp_swap(c, key, 2, 4);
        cmp_swap(c, key, 7, 9);
        cmp_swap(c, key, 8, 12);
        cmp_swap(c, key, 11, 15);
        cmp_swap(c, key, 13, 16);
        cmp_swap(c, key, 2, 3);
        cmp_swap(c, key, 4, 5);
        cmp_swap(c, key, 6, 7);
        cmp_swap(c, key, 10, 11);
        cmp_swap(c, key, 12, 14);
        cmp_swap(c, key, 13, 15);
        cmp_swap(c, key, 4, 6);
        cmp_swap(c, key, 5, 8);
        cmp_swap(c, key, 9, 10);
        cmp_swap(c, key, 11, 14);
        cmp_swap(c, key, 3, 4);
        cmp_swap(c, key, 5, 7);
        cmp_swap(c, key, 8, 9);
        cmp_swap(c, key, 10, 12);
        cmp_swap(c, key, 13, 14);
        cmp_swap(c, key, 5, 6);
        cmp_swap(c, key, 7, 8);
        cmp_swap(c, key, 9, 10);
        cmp_swap(c, key, 11, 12);
    }

    template <typename C, typename Key>
    void s19(C& c, Key key) {
        cmp_swap(c, key, 0, 12);
        cmp_swap(c, key, 1, 4);
        cmp_swap(c, key, 2, 8);
        cmp_swap(c, key, 3, 5);
        cmp_swap(c, key, 6, 17);
        cmp_swap(c, key, 7, 11);
        cmp_swap(c, key, 9, 14);
        cmp_swap(c, key, 10, 13);
        cmp_swap(c, key, 15, 16);
        cmp_swap(c, key, 0, 2);
        cmp_swap(c, key, 1, 7);
        cmp_swap(c, key, 3, 6);
        cmp_swap(c, key, 4, 11);
        cmp_swap(c, key, 5, 17);
        cmp_swap(c, key, 8, 12);
        cmp_swap(c, key, 10, 15);
        cmp_swap(c, key, 13, 16);
        cmp_swap(c, key, 14, 18);
        cmp_swap(c, key, 3, 10);
        cmp_swap(c, key, 4, 14);
        cmp_swap(c, key, 5, 15);
        cmp_swap(c, key, 6, 13);
        cmp_swap(c, key, 7, 9);
        cmp_swap(c, key, 11, 17);
        cmp_swap(c, key, 16, 18);
        cmp_swap(c, key, 0, 7);
        cmp_swap(c, key, 1, 10);
        cmp_swap(c, key, 4, 6);
        cmp_swap(c, key, 9, 15);
        cmp_swap(c, key, 11, 16);
        cmp_swap(c, key, 12, 17);
        cmp_swap(c, key, 13, 14);
        cmp_swap(c, key, 0, 3);
        cmp_swap(c, key, 2, 6);
        cmp_swap(c, key, 5, 7);
        cmp_swap(c, key, 8, 11);
        cmp_swap(c, key, 12, 16);
        cmp_swap(c, key, 1, 8);
        cmp_swap(c, key, 2, 9);
        cmp_swap(c, key, 3, 4);
        cmp_swap(c, key, 6, 15);
        cmp_swap(c, key, 7, 13);
        cmp_swap(c, key, 10, 11);
        cmp_swap(c, key, 12, 18);
        cmp_swap(c, key, 1, 3);
        cmp_swap(c, key, 2, 5);
        cmp_swap(c, key, 6, 9);
        cmp_swap(c, key, 7, 12);
        cmp_swap(c, key, 8, 10);
        cmp_swap(c, key, 11, 14);
        cmp_swap(c, key, 17, 18);
        cmp_swap(c, key, 0, 1);
        cmp_swap(c, key, 2, 3);
        cmp_swap(c, key, 4, 8);
        cmp_swap(c, key, 6, 10);
        cmp_swap(c, key, 9, 12);
        cmp_swap(c, key, 14, 15);
        cmp_swap(c, key, 16, 17);
        cmp_swap(c, key, 1, 2);
        cmp_swap(c, key, 5, 8);
        cmp_swap(c, key, 6, 7);
        cmp_swap(c, key, 9, 11);
        cmp_swap(c, key, 10, 13);
        cmp_swap(c, key, 14, 16);
        cmp_swap(c, key, 15, 17);
        cmp_swap(c, key, 3, 6);
        cmp_swap(c, key, 4, 5);
        cmp_swap(c, key, 7, 9);
        cmp_swap(c, key, 8, 10);
        cmp_swap(c, key, 11, 12);
        cmp_swap(c, key, 13, 14);
        cmp_swap(c, key, 15, 16);
        cmp_swap(c, key, 3, 4);
        cmp_swap(c, key, 5, 6);
        cmp_swap(c, key, 7, 8);
        cmp_swap(c, key, 9, 10);
        cmp_swap(c, key, 11, 13);
        cmp_swap(c, key, 12, 14);
        cmp_swap(c, key, 2, 3);
        cmp_swap(c, key, 4, 5);
        cmp_swap(c, key, 6, 7);
        cmp_swap(c, key, 8, 9);
        cmp_swap(c, key, 10, 11);
        cmp_swap(c, key, 12, 13);
        cmp_swap(c, key, 14, 15);
    }

    template <typename C, typename Key>
    void s20(C& c, Key key) {
        cmp_swap(c, key, 0, 3);
        cmp_swap(c, key, 1, 7);
        cmp_swap(c, key, 2, 5);
        cmp_swap(c, key, 4, 8);
        cmp_swap(c, key, 6, 9);
        cmp_swap(c, key, 10, 13);
        cmp_swap(c, key, 11, 15);
        cmp_swap(c, key, 12, 18);
        cmp_swap(c, key, 14, 17);
        cmp_swap(c, key, 16, 19);
        cmp_swap(c, key, 0, 14);
        cmp_swap(c, key, 1, 11);
        cmp_swap(c, key, 2, 16);
        cmp_swap(c, key, 3, 17);
        cmp_swap(c, key, 4, 12);
        cmp_swap(c, key, 5, 19);
        cmp_swap(c, key, 6, 10);
        cmp_swap(c, key, 7, 15);
        cmp_swap(c, key, 8, 18);
        cmp_swap(c, key, 9, 13);
        cmp_swap(c, key, 0, 4);
        cmp_swap(c, key, 1, 2);
        cmp_swap(c, key, 3, 8);
        cmp_swap(c, key, 5, 7);
        cmp_swap(c, key, 11, 16);
        cmp_swap(c, key, 12, 14);
        cmp_swap(c, key, 15, 19);
        cmp_swap(c, key, 17, 18);
        cmp_swap(c, key, 1, 6);
        cmp_swap(c, key, 2, 12);
        cmp_swap(c, key, 3, 5);
        cmp_swap(c, key, 4, 11);
        cmp_swap(c, key, 7, 17);
        cmp_swap(c, key, 8, 15);
        cmp_swap(c, key, 13, 18);
        cmp_swap(c, key, 14, 16);
        cmp_swap(c, key, 0, 1);
        cmp_swap(c, key, 2, 6);
        cmp_swap(c, key, 7, 10);
        cmp_swap(c, key, 9, 12);
        cmp_swap(c, key, 13, 17);
        cmp_swap(c, key, 18, 19);
        cmp_swap(c, key, 1, 6);
        cmp_swap(c, key, 5, 9);
        cmp_swap(c, key, 7, 11);
        cmp_swap(c, key, 8, 12);
        cmp_swap(c, key, 10, 14);
        cmp_swap(c, key, 13, 18);
        cmp_swap(c, key, 3, 5);
        cmp_swap(c, key, 4, 7);
        cmp_swap(c, key, 8, 10);
        cmp_swap(c, key, 9, 11);
        cmp_swap(c, key, 12, 15);
        cmp_swap(c, key, 14, 16);
        cmp_swap(c, key, 1, 3);
        cmp_swap(c, key, 2, 4);
        cmp_swap(c, key, 5, 7);
        cmp_swap(c, key, 6, 10);
        cmp_swap(c, key, 9, 13);
        cmp_swap(c, key, 12, 14);
        cmp_swap(c, key, 15, 17);
        cmp_swap(c, key, 16, 18);
        cmp_swap(c, key, 1, 2);
        cmp_swap(c, key, 3, 4);
        cmp_swap(c, key, 6, 7);
        cmp_swap(c, key, 8, 9);
        cmp_swap(c, key, 10, 11);
        cmp_swap(c, key, 12, 13);
        cmp_swap(c, key, 15, 16);
        cmp_swap(c, key, 17, 18);
        cmp_swap(c, key, 2, 3);
        cmp_swap(c, key, 4, 6);
        cmp_swap(c, key, 5, 8);
        cmp_swap(c, key, 7, 9);
        cmp_swap(c, key, 10, 12);
        cmp_swap(c, key, 11, 14);
        cmp_swap(c, key, 13, 15);
        cmp_swap(c, key, 16, 17);
        cmp_swap(c, key, 4, 5);
        cmp_swap(c, key, 6, 8);
        cmp_swap(c, key, 7, 10);
        cmp_swap(c, key, 9, 12);
        cmp_swap(c, key, 11, 13);
        cmp_swap(c, key, 14, 15);
        cmp_swap(c, key, 3, 4);
        cmp_swap(c, key, 5, 6);
        cmp_swap(c, key, 7, 8);
        cmp_swap(c, key, 9, 10);
        cmp_swap(c, key, 11, 12);
        cmp_swap(c, key, 13, 14);
        cmp_swap(c, key, 15, 16);
    }

    template <typename C, typename Key>
    void s21(C& c, Key key) {
        cmp_swap(c, key, 0, 1);
        cmp_swap(c, key, 2, 3);
        cmp_swap(c, key, 4, 5);
        cmp_swap(c, key, 6, 7);
        cmp_swap(c, key, 8, 9);
        cmp_swap(c, key, 10, 11);
        cmp_swap(c, key, 12, 13);
        cmp_swap(c, key, 14, 15);
        cmp_swap(c, key, 16, 17);
        cmp_swap(c, key, 18, 19);
        cmp_swap(c, key, 0, 2);
        cmp_swap(c, key, 1, 3);
        cmp_swap(c, key, 4, 6);
        cmp_swap(c, key, 5, 7);
        cmp_swap(c, key, 8, 10);
        cmp_swap(c, key, 9, 11);
        cmp_swap(c, key, 12, 14);
        cmp_swap(c, key, 13, 15);
        cmp_swap(c, key, 16, 18);
        cmp_swap(c, key, 17, 19);
        cmp_swap(c, key, 0, 4);
        cmp_swap(c, key, 1, 5);
        cmp_swap(c, key, 2, 6);
        cmp_swap(c, key, 3, 7);
        cmp_swap(c, key, 8, 12);
        cmp_swap(c, key, 9, 13);
        cmp_swap(c, key, 10, 14);
        cmp_swap(c, key, 11, 15);
        cmp_swap(c, key, 0, 8);
        cmp_swap(c, key, 1, 9);
        cmp_swap(c, key, 2, 10);
        cmp_swap(c, key, 3, 11);
        cmp_swap(c, key, 4, 12);
        cmp_swap(c, key, 5, 13);
        cmp_swap(c, key, 6, 14);
        cmp_swap(c, key, 7, 15);
        cmp_swap(c, key, 3, 18);
        cmp_swap(c, key, 7, 20);
        cmp_swap(c, key, 2, 7);
        cmp_swap(c, key, 3, 6);
        cmp_swap(c, key, 14, 18);
        cmp_swap(c, key, 19, 20);
        cmp_swap(c, key, 2, 16);
        cmp_swap(c, key, 3, 8);
        cmp_swap(c, key, 6, 14);
        cmp_swap(c, key, 7, 17);
        cmp_swap(c, key, 11, 19);
        cmp_swap(c, key, 15, 20);
        cmp_swap(c, key, 0, 2);
        cmp_swap(c, key, 7, 10);
        cmp_swap(c, key, 9, 16);
        cmp_swap(c, key, 13, 17);
        cmp_swap(c, key, 15, 19);
        cmp_swap(c, key, 1, 7);
        cmp_swap(c, key, 2, 3);
        cmp_swap(c, key, 4, 9);
        cmp_swap(c, key, 5, 10);
        cmp_swap(c, key, 11, 16);
        cmp_swap(c, key, 12, 13);
        cmp_swap(c, key, 17, 18);
        cmp_swap(c, key, 1, 4);
        cmp_swap(c, key, 5, 11);
        cmp_swap(c, key, 6, 12);
        cmp_swap(c, key, 7, 8);
        cmp_swap(c, key, 10, 13);
        cmp_swap(c, key, 14, 16);
        cmp_swap(c, key, 15, 17);
        cmp_swap(c, key, 18, 19);
        cmp_swap(c, key, 1, 2);
        cmp_swap(c, key, 3, 4);
        cmp_swap(c, key, 5, 6);
        cmp_swap(c, key, 10, 14);
        cmp_swap(c, key, 11, 12);
        cmp_swap(c, key, 13, 16);
        cmp_swap(c, key, 17, 18);
        cmp_swap(c, key, 2, 3);
        cmp_swap(c, key, 4, 5);
        cmp_swap(c, key, 6, 9);
        cmp_swap(c, key, 10, 11);
        cmp_swap(c, key, 12, 13);
        cmp_swap(c, key, 14, 15);
        cmp_swap(c, key, 16, 17);
        cmp_swap(c, key, 6, 7);
        cmp_swap(c, key, 8, 9);
        cmp_swap(c, key, 13, 15);
        cmp_swap(c, key, 4, 6);
        cmp_swap(c, key, 7, 8);
        cmp_swap(c, key, 9, 12);
        cmp_swap(c, key, 15, 16);
        cmp_swap(c, key, 3, 4);
        cmp_swap(c, key, 5, 7);
        cmp_swap(c, key, 8, 10);
        cmp_swap(c, key, 9, 11);
        cmp_swap(c, key, 12, 14);
        cmp_swap(c, key, 5, 6);
        cmp_swap(c, key, 7, 8);
        cmp_swap(c, key, 9, 10);
        cmp_swap(c, key, 11, 12);
        cmp_swap(c, key, 13, 14);
    }

    template <typename C, typename Key>
    void s22(C& c, Key key) {
        cmp_swap(c, key, 0, 1);
        cmp_swap(c, key, 2, 3);
        cmp_swap(c, key, 4, 5);
        cmp_swap(c, key, 6, 7);
        cmp_swap(c, key, 8, 9);
        cmp_swap(c, key, 10, 11);
        cmp_swap(c, key, 12, 13);
        cmp_swap(c, key, 14, 15);
        cmp_swap(c, key, 16, 17);
        cmp_swap(c, key, 18, 19);
        cmp_swap(c, key, 20, 21);
        cmp_swap(c, key, 0, 2);
        cmp_swap(c, key, 1, 3);
        cmp_swap(c, key, 4, 6);
        cmp_swap(c, key, 5, 7);
        cmp_swap(c, key, 8, 10);
        cmp_swap(c, key, 9, 11);
        cmp_swap(c, key, 12, 14);
        cmp_swap(c, key, 13, 15);
        cmp_swap(c, key, 16, 18);
        cmp_swap(c, key, 17, 19);
        cmp_swap(c, key, 0, 4);
        cmp_swap(c, key, 1, 5);
        cmp_swap(c, key, 2, 6);
        cmp_swap(c, key, 3, 7);
        cmp_swap(c, key, 8, 12);
        cmp_swap(c, key, 9, 13);
        cmp_swap(c, key, 10, 14);
        cmp_swap(c, key, 11, 15);
        cmp_swap(c, key, 17, 18);
        cmp_swap(c, key, 0, 8);
        cmp_swap(c, key, 1, 9);
        cmp_swap(c, key, 2, 10);
        cmp_swap(c, key, 3, 11);
        cmp_swap(c, key, 4, 12);
        cmp_swap(c, key, 5, 13);
        cmp_swap(c, key, 6, 14);
        cmp_swap(c, key, 7, 15);
        cmp_swap(c, key, 17, 21);
        cmp_swap(c, key, 1, 8);
        cmp_swap(c, key, 6, 20);
        cmp_swap(c, key, 7, 17);
        cmp_swap(c, key, 19, 21);
        cmp_swap(c, key, 3, 19);
        cmp_swap(c, key, 4, 7);
        cmp_swap(c, key, 6, 16);
        cmp_swap(c, key, 14, 21);
        cmp_swap(c, key, 18, 20);
        cmp_swap(c, key, 0, 6);
        cmp_swap(c, key, 1, 4);
        cmp_swap(c, key, 3, 12);
        cmp_swap(c, key, 5, 18);
        cmp_swap(c, key, 7, 10);
        cmp_swap(c, key, 9, 16);
        cmp_swap(c, key, 11, 20);
        cmp_swap(c, key, 13, 19);
        cmp_swap(c, key, 14, 17);
        cmp_swap(c, key, 15, 21);
        cmp_swap(c, key, 2, 5);
        cmp_swap(c, key, 3, 7);
        cmp_swap(c, key, 6, 8);
        cmp_swap(c, key, 10, 11);
        cmp_swap(c, key, 12, 13);
        cmp_swap(c, key, 15, 20);
        cmp_swap(c, key, 16, 18);
        cmp_swap(c, key, 17, 19);
        cmp_swap(c, key, 5, 8);
        cmp_swap(c, key, 7, 9);
        cmp_swap(c, key, 10, 12);
        cmp_swap(c, key, 14, 16);
        cmp_swap(c, key, 15, 17);
        cmp_swap(c, key, 19, 20);
        cmp_swap(c, key, 2, 7);
        cmp_swap(c, key, 3, 5);
        cmp_swap(c, key, 8, 9);
        cmp_swap(c, key, 10, 14);
        cmp_swap(c, key, 11, 16);
        cmp_swap(c, key, 12, 18);
        cmp_swap(c, key, 17, 19);
        cmp_swap(c, key, 2, 6);
        cmp_swap(c, key, 4, 7);
        cmp_swap(c, key, 5, 8);
        cmp_swap(c, key, 11, 13);
        cmp_swap(c, key, 12, 14);
        cmp_swap(c, key, 15, 18);
        cmp_swap(c, key, 1, 2);
        cmp_swap(c, key, 4, 6);
        cmp_swap(c, key, 7, 10);
        cmp_swap(c, key, 9, 11);
        cmp_swap(c, key, 13, 16);
        cmp_swap(c, key, 2, 4);
        cmp_swap(c, key, 3, 6);
        cmp_swap(c, key, 5, 7);
        cmp_swap(c, key, 8, 10);
        cmp_swap(c, key, 9, 12);
        cmp_swap(c, key, 11, 14);
        cmp_swap(c, key, 13, 15);
        cmp_swap(c, key, 16, 18);
        cmp_swap(c, key, 3, 4);
        cmp_swap(c, key, 5, 6);
        cmp_swap(c, key, 7, 8);
        cmp_swap(c, key, 9, 10);
        cmp_swap(c, key, 11, 12);
        cmp_swap(c, key, 13, 14);
        cmp_swap(c, key, 15, 16);
        cmp_swap(c, key, 17, 18);
    }

    template <typename C, typename Key>
    void s23(C& c, Key key) {
        cmp_swap(c, key, 0, 1);
        cmp_swap(c, key, 2, 3);
        cmp_swap(c, key, 4, 5);
        cmp_swap(c, key, 6, 7);
        cmp_swap(c, key, 8, 9);
        cmp_swap(c, key, 10, 11);
        cmp_swap(c, key, 12, 13);
        cmp_swap(c, key, 14, 15);
        cmp_swap(c, key, 16, 17);
        cmp_swap(c, key, 18, 19);
        cmp_swap(c, key, 20, 21);
        cmp_swap(c, key, 0, 2);
        cmp_swap(c, key, 1, 3);
        cmp_swap(c, key, 4, 6);
        cmp_swap(c, key, 5, 7);
        cmp_swap(c, key, 8, 10);
        cmp_swap(c, key, 9, 11);
        cmp_swap(c, key, 12, 14);
        cmp_swap(c, key, 13, 15);
        cmp_swap(c, key, 16, 18);
        cmp_swap(c, key, 17, 19);
        cmp_swap(c, key, 21, 22);
        cmp_swap(c, key, 0, 4);
        cmp_swap(c, key, 1, 5);
        cmp_swap(c, key, 2, 6);
        cmp_swap(c, key, 3, 7);
        cmp_swap(c, key, 8, 12);
        cmp_swap(c, key, 9, 13);
        cmp_swap(c, key, 10, 14);
        cmp_swap(c, key, 11, 15);
        cmp_swap(c, key, 17, 21);
        cmp_swap(c, key, 18, 20);
        cmp_swap(c, key, 19, 22);
        cmp_swap(c, key, 0, 8);
        cmp_swap(c, key, 1, 9);
        cmp_swap(c, key, 2, 10);
        cmp_swap(c, key, 3, 11);
        cmp_swap(c, key, 4, 12);
        cmp_swap(c, key, 5, 13);
        cmp_swap(c, key, 6, 14);
        cmp_swap(c, key, 7, 15);
        cmp_swap(c, key, 1, 2);
        cmp_swap(c, key, 5, 18);
        cmp_swap(c, key, 7, 19);
        cmp_swap(c, key, 9, 16);
        cmp_swap(c, key, 10, 21);
        cmp_swap(c, key, 12, 20);
        cmp_swap(c, key, 15, 22);
        cmp_swap(c, key, 5, 9);
        cmp_swap(c, key, 6, 7);
        cmp_swap(c, key, 10, 18);
        cmp_swap(c, key, 11, 21);
        cmp_swap(c, key, 12, 17);
        cmp_swap(c, key, 13, 20);
        cmp_swap(c, key, 14, 15);
        cmp_swap(c, key, 3, 17);
        cmp_swap(c, key, 6, 16);
        cmp_swap(c, key, 7, 14);
        cmp_swap(c, key, 8, 12);
        cmp_swap(c, key, 15, 19);
        cmp_swap(c, key, 20, 21);
        cmp_swap(c, key, 3, 4);
        cmp_swap(c, key, 5, 8);
        cmp_swap(c, key, 6, 10);
        cmp_swap(c, key, 9, 12);
        cmp_swap(c, key, 13, 16);
        cmp_swap(c, key, 14, 15);
        cmp_swap(c, key, 17, 18);
        cmp_swap(c, key, 19, 21);
        cmp_swap(c, key, 0, 5);
        cmp_swap(c, key, 1, 8);
        cmp_swap(c, key, 2, 12);
        cmp_swap(c, key, 3, 9);
        cmp_swap(c, key, 4, 10);
        cmp_swap(c, key, 7, 13);
        cmp_swap(c, key, 11, 17);
        cmp_swap(c, key, 14, 16);
        cmp_swap(c, key, 18, 20);
        cmp_swap(c, key, 2, 6);
        cmp_swap(c, key, 3, 5);
        cmp_swap(c, key, 4, 8);
        cmp_swap(c, key, 7, 11);
        cmp_swap(c, key, 10, 12);
        cmp_swap(c, key, 13, 18);
        cmp_swap(c, key, 14, 17);
        cmp_swap(c, key, 15, 20);
        cmp_swap(c, key, 1, 3);
        cmp_swap(c, key, 2, 5);
        cmp_swap(c, key, 6, 9);
        cmp_swap(c, key, 7, 10);
        cmp_swap(c, key, 11, 13);
        cmp_swap(c, key, 12, 14);
        cmp_swap(c, key, 15, 18);
        cmp_swap(c, key, 16, 17);
        cmp_swap(c, key, 19, 20);
        cmp_swap(c, key, 2, 3);
        cmp_swap(c, key, 4, 6);
        cmp_swap(c, key, 8, 9);
        cmp_swap(c, key, 11, 12);
        cmp_swap(c, key, 13, 14);
        cmp_swap(c, key, 15, 16);
        cmp_swap(c, key, 17, 19);
        cmp_swap(c, key, 3, 4);
        cmp_swap(c, key, 5, 6);
        cmp_swap(c, key, 7, 8);
        cmp_swap(c, key, 9, 10);
        cmp_swap(c, key, 12, 13);
        cmp_swap(c, key, 14, 15);
        cmp_swap(c, key, 17, 18);
        cmp_swap(c, key, 4, 5);
        cmp_swap(c, key, 6, 7);
        cmp_swap(c, key, 8, 9);
        cmp_swap(c, key, 10, 11);
        cmp_swap(c, key, 16, 17);
    }

    template <typename C, typename Key>
    void s24(C& c, Key key) {
        cmp_swap(c, key, 0, 20);
        cmp_swap(c, key, 1, 12);
        cmp_swap(c, key, 2, 16);
        cmp_swap(c, key, 3, 23);
        cmp_swap(c, key, 4, 6);
        cmp_swap(c, key, 5, 10);
        cmp_swap(c, key, 7, 21);
        cmp_swap(c, key, 8, 14);
        cmp_swap(c, key, 9, 15);
        cmp_swap(c, key, 11, 22);
        cmp_swap(c, key, 13, 18);
        cmp_swap(c, key, 17, 19);
        cmp_swap(c, key, 0, 3);
        cmp_swap(c, key, 1, 11);
        cmp_swap(c, key, 2, 7);
        cmp_swap(c, key, 4, 17);
        cmp_swap(c, key, 5, 13);
        cmp_swap(c, key, 6, 19);
        cmp_swap(c, key, 8, 9);
        cmp_swap(c, key, 10, 18);
        cmp_swap(c, key, 12, 22);
        cmp_swap(c, key, 14, 15);
        cmp_swap(c, key, 16, 21);
        cmp_swap(c, key, 20, 23);
        cmp_swap(c, key, 0, 1);
        cmp_swap(c, key, 2, 4);
        cmp_swap(c, key, 3, 12);
        cmp_swap(c, key, 5, 8);
        cmp_swap(c, key, 6, 9);
        cmp_swap(c, key, 7, 10);
        cmp_swap(c, key, 11, 20);
        cmp_swap(c, key, 13, 16);
        cmp_swap(c, key, 14, 17);
        cmp_swap(c, key, 15, 18);
        cmp_swap(c, key, 19, 21);
        cmp_swap(c, key, 22, 23);
        cmp_swap(c, key, 2, 5);
        cmp_swap(c, key, 4, 8);
        cmp_swap(c, key, 6, 11);
        cmp_swap(c, key, 7, 14);
        cmp_swap(c, key, 9, 16);
        cmp_swap(c, key, 12, 17);
        cmp_swap(c, key, 15, 19);
        cmp_swap(c, key, 18, 21);
        cmp_swap(c, key, 1, 8);
        cmp_swap(c, key, 3, 14);
        cmp_swap(c, key, 4, 7);
        cmp_swap(c, key, 9, 20);
        cmp_swap(c, key, 10, 12);
        cmp_swap(c, key, 11, 13);
        cmp_swap(c, key, 15, 22);
        cmp_swap(c, key, 16, 19);
        cmp_swap(c, key, 0, 7);
        cmp_swap(c, key, 1, 5);
        cmp_swap(c, key, 3, 4);
        cmp_swap(c, key, 6, 11);
        cmp_swap(c, key, 8, 15);
        cmp_swap(c, key, 9, 14);
        cmp_swap(c, key, 10, 13);
        cmp_swap(c, key, 12, 17);
        cmp_swap(c, key, 16, 23);
        cmp_swap(c, key, 18, 22);
        cmp_swap(c, key, 19, 20);
        cmp_swap(c, key, 0, 2);
        cmp_swap(c, key, 1, 6);
        cmp_swap(c, key, 4, 7);
        cmp_swap(c, key, 5, 9);
        cmp_swap(c, key, 8, 10);
        cmp_swap(c, key, 13, 15);
        cmp_swap(c, key, 14, 18);
        cmp_swap(c, key, 16, 19);
        cmp_swap(c, key, 17, 22);
        cmp_swap(c, key, 21, 23);
        cmp_swap(c, key, 2, 3);
        cmp_swap(c, key, 4, 5);
        cmp_swap(c, key, 6, 8);
        cmp_swap(c, key, 7, 9);
        cmp_swap(c, key, 10, 11);
        cmp_swap(c, key, 12, 13);
        cmp_swap(c, key, 14, 16);
        cmp_swap(c, key, 15, 17);
        cmp_swap(c, key, 18, 19);
        cmp_swap(c, key, 20, 21);
        cmp_swap(c, key, 1, 2);
        cmp_swap(c, key, 3, 6);
        cmp_swap(c, key, 4, 10);
        cmp_swap(c, key, 7, 8);
        cmp_swap(c, key, 9, 11);
        cmp_swap(c, key, 12, 14);
        cmp_swap(c, key, 13, 19);
        cmp_swap(c, key, 15, 16);
        cmp_swap(c, key, 17, 20);
        cmp_swap(c, key, 21, 22);
        cmp_swap(c, key, 2, 3);
        cmp_swap(c, key, 5, 10);
        cmp_swap(c, key, 6, 7);
        cmp_swap(c, key, 8, 9);
        cmp_swap(c, key, 13, 18);
        cmp_swap(c, key, 14, 15);
        cmp_swap(c, key, 16, 17);
        cmp_swap(c, key, 20, 21);
        cmp_swap(c, key, 3, 4);
        cmp_swap(c, key, 5, 7);
        cmp_swap(c, key, 10, 12);
        cmp_swap(c, key, 11, 13);
        cmp_swap(c, key, 16, 18);
        cmp_swap(c, key, 19, 20);
        cmp_swap(c, key, 4, 6);
        cmp_swap(c, key, 8, 10);
        cmp_swap(c, key, 9, 12);
        cmp_swap(c, key, 11, 14);
        cmp_swap(c, key, 13, 15);
        cmp_swap(c, key, 17, 19);
        cmp_swap(c, key, 5, 6);
        cmp_swap(c, key, 7, 8);
        cmp_swap(c, key, 9, 10);
        cmp_swap(c, key, 11, 12);
        cmp_swap(c, key, 13, 14);
        cmp_swap(c, key, 15, 16);
        cmp_swap(c, key, 17, 18);
    }

    template <typename C, typename Key>
    void s25(C& c, Key key) {
        cmp_swap(c, key, 0, 1);
        cmp_swap(c, key, 2, 3);
        cmp_swap(c, key, 4, 5);
        cmp_swap(c, key, 6, 7);
        cmp_swap(c, key, 8, 9);
        cmp_swap(c, key, 10, 11);
        cmp_swap(c, key, 12, 13);
        cmp_swap(c, key, 14, 15);
        cmp_swap(c, key, 16, 17);
        cmp_swap(c, key, 18, 19);
        cmp_swap(c, key, 20, 21);
        cmp_swap(c, key, 22, 23);
        cmp_swap(c, key, 0, 2);
        cmp_swap(c, key, 1, 3);
        cmp_swap(c, key, 4, 6);
        cmp_swap(c, key, 5, 7);
        cmp_swap(c, key, 8, 10);
        cmp_swap(c, key, 9, 11);
        cmp_swap(c, key, 12, 14);
        cmp_swap(c, key, 13, 15);
        cmp_swap(c, key, 16, 18);
        cmp_swap(c, key, 17, 19);
        cmp_swap(c, key, 20, 22);
        cmp_swap(c, key, 21, 23);
        cmp_swap(c, key, 0, 4);
        cmp_swap(c, key, 1, 5);
        cmp_swap(c, key, 2, 6);
        cmp_swap(c, key, 3, 7);
        cmp_swap(c, key, 8, 12);
        cmp_swap(c, key, 9, 13);
        cmp_swap(c, key, 10, 14);
        cmp_swap(c, key, 11, 15);
        cmp_swap(c, key, 16, 20);
        cmp_swap(c, key, 17, 21);
        cmp_swap(c, key, 18, 22);
        cmp_swap(c, key, 19, 23);
        cmp_swap(c, key, 0, 8);
        cmp_swap(c, key, 1, 9);
        cmp_swap(c, key, 2, 10);
        cmp_swap(c, key, 3, 11);
        cmp_swap(c, key, 4, 12);
        cmp_swap(c, key, 5, 13);
        cmp_swap(c, key, 6, 14);
        cmp_swap(c, key, 7, 15);
        cmp_swap(c, key, 16, 24);
        cmp_swap(c, key, 17, 18);
        cmp_swap(c, key, 21, 22);
        cmp_swap(c, key, 0, 16);
        cmp_swap(c, key, 2, 8);
        cmp_swap(c, key, 3, 12);
        cmp_swap(c, key, 4, 20);
        cmp_swap(c, key, 5, 19);
        cmp_swap(c, key, 9, 22);
        cmp_swap(c, key, 10, 24);
        cmp_swap(c, key, 1, 8);
        cmp_swap(c, key, 6, 9);
        cmp_swap(c, key, 7, 22);
        cmp_swap(c, key, 10, 18);
        cmp_swap(c, key, 14, 19);
        cmp_swap(c, key, 21, 24);
        cmp_swap(c, key, 1, 17);
        cmp_swap(c, key, 2, 10);
        cmp_swap(c, key, 5, 21);
        cmp_swap(c, key, 11, 24);
        cmp_swap(c, key, 18, 20);
        cmp_swap(c, key, 1, 4);
        cmp_swap(c, key, 2, 16);
        cmp_swap(c, key, 3, 21);
        cmp_swap(c, key, 6, 10);
        cmp_swap(c, key, 8, 18);
        cmp_swap(c, key, 9, 17);
        cmp_swap(c, key, 11, 14);
        cmp_swap(c, key, 12, 20);
        cmp_swap(c, key, 23, 24);
        cmp_swap(c, key, 1, 2);
        cmp_swap(c, key, 3, 5);
        cmp_swap(c, key, 4, 16);
        cmp_swap(c, key, 12, 18);
        cmp_swap(c, key, 13, 23);
        cmp_swap(c, key, 14, 19);
        cmp_swap(c, key, 15, 24);
        cmp_swap(c, key, 17, 21);
        cmp_swap(c, key, 4, 8);
        cmp_swap(c, key, 7, 13);
        cmp_swap(c, key, 9, 16);
        cmp_swap(c, key, 10, 12);
        cmp_swap(c, key, 11, 18);
        cmp_swap(c, key, 14, 20);
        cmp_swap(c, key, 15, 23);
        cmp_swap(c, key, 19, 22);
        cmp_swap(c, key, 2, 4);
        cmp_swap(c, key, 3, 8);
        cmp_swap(c, key, 5, 16);
        cmp_swap(c, key, 6, 9);
        cmp_swap(c, key, 7, 17);
        cmp_swap(c, key, 13, 21);
        cmp_swap(c, key, 15, 19);
        cmp_swap(c, key, 22, 23);
        cmp_swap(c, key, 3, 6);
        cmp_swap(c, key, 5, 10);
        cmp_swap(c, key, 7, 11);
        cmp_swap(c, key, 8, 9);
        cmp_swap(c, key, 12, 16);
        cmp_swap(c, key, 13, 18);
        cmp_swap(c, key, 14, 17);
        cmp_swap(c, key, 19, 22);
        cmp_swap(c, key, 20, 21);
        cmp_swap(c, key, 3, 4);
        cmp_swap(c, key, 6, 8);
        cmp_swap(c, key, 7, 9);
        cmp_swap(c, key, 10, 12);
        cmp_swap(c, key, 11, 14);
        cmp_swap(c, key, 13, 16);
        cmp_swap(c, key, 15, 17);
        cmp_swap(c, key, 18, 20);
        cmp_swap(c, key, 19, 21);
        cmp_swap(c, key, 5, 8);
        cmp_swap(c, key, 7, 10);
        cmp_swap(c, key, 9, 12);
        cmp_swap(c, key, 11, 13);
        cmp_swap(c, key, 14, 16);
        cmp_swap(c, key, 15, 18);
        cmp_swap(c, key, 17, 20);
        cmp_swap(c, key, 5, 6);
        cmp_swap(c, key, 7, 8);
        cmp_swap(c, key, 9, 10);
        cmp_swap(c, key, 11, 12);
        cmp_swap(c, key, 13, 14);
        cmp_swap(c, key, 15, 16);
        cmp_swap(c, key, 17, 18);
        cmp_swap(c, key, 19, 20);
    }

    template <typename C, typename Key>
    void s26(C& c, Key key) {
        cmp_swap(c, key, 0, 25);
        cmp_swap(c, key, 1, 3);
        cmp_swap(c, key, 2, 9);
        cmp_swap(c, key, 4, 19);
        cmp_swap(c, key, 5, 18);
        cmp_swap(c, key, 6, 21);
        cmp_swap(c, key, 7, 20);
        cmp_swap(c, key, 8, 10);
        cmp_swap(c, key, 11, 12);
        cmp_swap(c, key, 13, 14);
        cmp_swap(c, key, 15, 17);
        cmp_swap(c, key, 16, 23);
        cmp_swap(c, key, 22, 24);
        cmp_swap(c, key, 1, 4);
        cmp_swap(c, key, 2, 16);
        cmp_swap(c, key, 3, 19);
        cmp_swap(c, key, 5, 13);
        cmp_swap(c, key, 6, 22);
        cmp_swap(c, key, 7, 11);
        cmp_swap(c, key, 8, 15);
        cmp_swap(c, key, 9, 23);
        cmp_swap(c, key, 10, 17);
        cmp_swap(c, key, 12, 20);
        cmp_swap(c, key, 14, 18);
        cmp_swap(c, key, 21, 24);
        cmp_swap(c, key, 1, 5);
        cmp_swap(c, key, 2, 8);
        cmp_swap(c, key, 3, 14);
        cmp_swap(c, key, 4, 13);
        cmp_swap(c, key, 6, 7);
        cmp_swap(c, key, 9, 15);
        cmp_swap(c, key, 10, 16);
        cmp_swap(c, key, 11, 22);
        cmp_swap(c, key, 12, 21);
        cmp_swap(c, key, 17, 23);
        cmp_swap(c, key, 18, 19);
        cmp_swap(c, key, 20, 24);
        cmp_swap(c, key, 0, 10);
        cmp_swap(c, key, 1, 6);
        cmp_swap(c, key, 3, 7);
        cmp_swap(c, key, 4, 11);
        cmp_swap(c, key, 5, 12);
        cmp_swap(c, key, 13, 20);
        cmp_swap(c, key, 14, 21);
        cmp_swap(c, key, 15, 25);
        cmp_swap(c, key, 18, 22);
        cmp_swap(c, key, 19, 24);
        cmp_swap(c, key, 0, 4);
        cmp_swap(c, key, 8, 10);
        cmp_swap(c, key, 12, 13);
        cmp_swap(c, key, 15, 17);
        cmp_swap(c, key, 21, 25);
        cmp_swap(c, key, 0, 2);
        cmp_swap(c, key, 4, 8);
        cmp_swap(c, key, 10, 12);
        cmp_swap(c, key, 13, 15);
        cmp_swap(c, key, 17, 21);
        cmp_swap(c, key, 23, 25);
        cmp_swap(c, key, 0, 1);
        cmp_swap(c, key, 2, 3);
        cmp_swap(c, key, 4, 5);
        cmp_swap(c, key, 8, 14);
        cmp_swap(c, key, 9, 13);
        cmp_swap(c, key, 11, 17);
        cmp_swap(c, key, 12, 16);
        cmp_swap(c, key, 20, 21);
        cmp_swap(c, key, 22, 23);
        cmp_swap(c, key, 24, 25);
        cmp_swap(c, key, 1, 4);
        cmp_swap(c, key, 3, 10);
        cmp_swap(c, key, 6, 9);
        cmp_swap(c, key, 7, 13);
        cmp_swap(c, key, 8, 11);
        cmp_swap(c, key, 12, 18);
        cmp_swap(c, key, 14, 17);
        cmp_swap(c, key, 15, 22);
        cmp_swap(c, key, 16, 19);
        cmp_swap(c, key, 21, 24);
        cmp_swap(c, key, 2, 6);
        cmp_swap(c, key, 3, 8);
        cmp_swap(c, key, 5, 7);
        cmp_swap(c, key, 9, 12);
        cmp_swap(c, key, 13, 16);
        cmp_swap(c, key, 17, 22);
        cmp_swap(c, key, 18, 20);
        cmp_swap(c, key, 19, 23);
        cmp_swap(c, key, 1, 2);
        cmp_swap(c, key, 4, 6);
        cmp_swap(c, key, 5, 9);
        cmp_swap(c, key, 7, 10);
        cmp_swap(c, key, 11, 12);
        cmp_swap(c, key, 13, 14);
        cmp_swap(c, key, 15, 18);
        cmp_swap(c, key, 16, 20);
        cmp_swap(c, key, 19, 21);
        cmp_swap(c, key, 23, 24);
        cmp_swap(c, key, 2, 4);
        cmp_swap(c, key, 3, 5);
        cmp_swap(c, key, 7, 13);
        cmp_swap(c, key, 8, 9);
        cmp_swap(c, key, 10, 14);
        cmp_swap(c, key, 11, 15);
        cmp_swap(c, key, 12, 18);
        cmp_swap(c, key, 16, 17);
        cmp_swap(c, key, 20, 22);
        cmp_swap(c, key, 21, 23);
        cmp_swap(c, key, 3, 4);
        cmp_swap(c, key, 6, 9);
        cmp_swap(c, key, 7, 11);
        cmp_swap(c, key, 10, 12);
        cmp_swap(c, key, 13, 15);
        cmp_swap(c, key, 14, 18);
        cmp_swap(c, key, 16, 19);
        cmp_swap(c, key, 21, 22);
        cmp_swap(c, key, 5, 7);
        cmp_swap(c, key, 6, 8);
        cmp_swap(c, key, 9, 13);
        cmp_swap(c, key, 10, 11);
        cmp_swap(c, key, 12, 16);
        cmp_swap(c, key, 14, 15);
        cmp_swap(c, key, 17, 19);
        cmp_swap(c, key, 18, 20);
        cmp_swap(c, key, 5, 6);
        cmp_swap(c, key, 7, 8);
        cmp_swap(c, key, 9, 10);
        cmp_swap(c, key, 11, 13);
        cmp_swap(c, key, 12, 14);
        cmp_swap(c, key, 15, 16);
        cmp_swap(c, key, 17, 18);
        cmp_swap(c, key, 19, 20);
        cmp_swap(c, key, 4, 5);
        cmp_swap(c, key, 6, 7);
        cmp_swap(c, key, 8, 9);
        cmp_swap(c, key, 10, 11);
        cmp_swap(c, key, 12, 13);
        cmp_swap(c, key, 14, 15);
        cmp_swap(c, key, 16, 17);
        cmp_swap(c, key, 18, 19);
        cmp_swap(c, key, 20, 21);
    }

    template <typename C, typename Key>
    void s27(C& c, Key key) {
        cmp_swap(c, key, 0, 1);
        cmp_swap(c, key, 2, 3);
        cmp_swap(c, key, 4, 5);
        cmp_swap(c, key, 6, 7);
        cmp_swap(c, key, 8, 9);
        cmp_swap(c, key, 10, 11);
        cmp_swap(c, key, 12, 13);
        cmp_swap(c, key, 14, 15);
        cmp_swap(c, key, 16, 17);
        cmp_swap(c, key, 18, 19);
        cmp_swap(c, key, 20, 21);
        cmp_swap(c, key, 22, 23);
        cmp_swap(c, key, 24, 25);
        cmp_swap(c, key, 0, 2);
        cmp_swap(c, key, 1, 3);
        cmp_swap(c, key, 4, 6);
        cmp_swap(c, key, 5, 7);
        cmp_swap(c, key, 8, 10);
        cmp_swap(c, key, 9, 11);
        cmp_swap(c, key, 12, 14);
        cmp_swap(c, key, 13, 15);
        cmp_swap(c, key, 16, 18);
        cmp_swap(c, key, 17, 19);
        cmp_swap(c, key, 20, 22);
        cmp_swap(c, key, 21, 23);
        cmp_swap(c, key, 24, 26);
        cmp_swap(c, key, 0, 4);
        cmp_swap(c, key, 1, 5);
        cmp_swap(c, key, 2, 6);
        cmp_swap(c, key, 3, 7);
        cmp_swap(c, key, 8, 12);
        cmp_swap(c, key, 9, 13);
        cmp_swap(c, key, 10, 14);
        cmp_swap(c, key, 11, 15);
        cmp_swap(c, key, 16, 20);
        cmp_swap(c, key, 17, 21);
        cmp_swap(c, key, 18, 22);
        cmp_swap(c, key, 19, 23);
        cmp_swap(c, key, 25, 26);
        cmp_swap(c, key, 0, 8);
        cmp_swap(c, key, 1, 9);
        cmp_swap(c, key, 2, 10);
        cmp_swap(c, key, 3, 11);
        cmp_swap(c, key, 4, 12);
        cmp_swap(c, key, 5, 13);
        cmp_swap(c, key, 6, 14);
        cmp_swap(c, key, 7, 15);
        cmp_swap(c, key, 18, 24);
        cmp_swap(c, key, 20, 26);
        cmp_swap(c, key, 22, 25);
        cmp_swap(c, key, 2, 8);
        cmp_swap(c, key, 3, 12);
        cmp_swap(c, key, 5, 10);
        cmp_swap(c, key, 6, 9);
        cmp_swap(c, key, 7, 13);
        cmp_swap(c, key, 11, 14);
        cmp_swap(c, key, 16, 18);
        cmp_swap(c, key, 17, 22);
        cmp_swap(c, key, 19, 26);
        cmp_swap(c, key, 21, 25);
        cmp_swap(c, key, 0, 16);
        cmp_swap(c, key, 1, 8);
        cmp_swap(c, key, 5, 22);
        cmp_swap(c, key, 10, 24);
        cmp_swap(c, key, 11, 25);
        cmp_swap(c, key, 23, 26);
        cmp_swap(c, key, 1, 18);
        cmp_swap(c, key, 4, 8);
        cmp_swap(c, key, 5, 19);
        cmp_swap(c, key, 9, 26);
        cmp_swap(c, key, 10, 20);
        cmp_swap(c, key, 12, 23);
        cmp_swap(c, key, 21, 24);
        cmp_swap(c, key, 2, 10);
        cmp_swap(c, key, 3, 21);
        cmp_swap(c, key, 4, 17);
        cmp_swap(c, key, 6, 20);
        cmp_swap(c, key, 7, 24);
        cmp_swap(c, key, 8, 19);
        cmp_swap(c, key, 12, 18);
        cmp_swap(c, key, 14, 23);
        cmp_swap(c, key, 15, 26);
        cmp_swap(c, key, 2, 4);
        cmp_swap(c, key, 6, 10);
        cmp_swap(c, key, 7, 11);
        cmp_swap(c, key, 8, 16);
        cmp_swap(c, key, 9, 17);
        cmp_swap(c, key, 13, 19);
        cmp_swap(c, key, 14, 22);
        cmp_swap(c, key, 15, 23);
        cmp_swap(c, key, 20, 21);
        cmp_swap(c, key, 1, 8);
        cmp_swap(c, key, 3, 10);
        cmp_swap(c, key, 5, 9);
        cmp_swap(c, key, 12, 16);
        cmp_swap(c, key, 13, 18);
        cmp_swap(c, key, 14, 17);
        cmp_swap(c, key, 15, 24);
        cmp_swap(c, key, 19, 25);
        cmp_swap(c, key, 1, 2);
        cmp_swap(c, key, 3, 12);
        cmp_swap(c, key, 4, 8);
        cmp_swap(c, key, 7, 13);
        cmp_swap(c, key, 9, 16);
        cmp_swap(c, key, 11, 18);
        cmp_swap(c, key, 14, 20);
        cmp_swap(c, key, 17, 21);
        cmp_swap(c, key, 19, 22);
        cmp_swap(c, key, 23, 25);
        cmp_swap(c, key, 2, 4);
        cmp_swap(c, key, 3, 8);
        cmp_swap(c, key, 5, 12);
        cmp_swap(c, key, 6, 9);
        cmp_swap(c, key, 7, 14);
        cmp_swap(c, key, 10, 16);
        cmp_swap(c, key, 11, 17);
        cmp_swap(c, key, 13, 20);
        cmp_swap(c, key, 15, 19);
        cmp_swap(c, key, 18, 21);
        cmp_swap(c, key, 23, 24);
        cmp_swap(c, key, 5, 6);
        cmp_swap(c, key, 7, 10);
        cmp_swap(c, key, 9, 12);
        cmp_swap(c, key, 11, 13);
        cmp_swap(c, key, 14, 16);
        cmp_swap(c, key, 15, 18);
        cmp_swap(c, key, 17, 20);
        cmp_swap(c, key, 21, 22);
        cmp_swap(c, key, 3, 5);
        cmp_swap(c, key, 6, 8);
        cmp_swap(c, key, 7, 9);
        cmp_swap(c, key, 10, 12);
        cmp_swap(c, key, 11, 14);
        cmp_swap(c, key, 13, 16);
        cmp_swap(c, key, 15, 17);
        cmp_swap(c, key, 18, 20);
        cmp_swap(c, key, 19, 21);
        cmp_swap(c, key, 22, 23);
        cmp_swap(c, key, 3, 4);
        cmp_swap(c, key, 5, 6);
        cmp_swap(c, key, 7, 8);
        cmp_swap(c, key, 9, 10);
        cmp_swap(c, key, 11, 12);
        cmp_swap(c, key, 13, 14);
        cmp_swap(c, key, 15, 16);
        cmp_swap(c, key, 17, 18);
        cmp_swap(c, key, 19, 20);
        cmp_swap(c, key, 21, 22);
    }

    template <typename C, typename Key>
    void s28(C& c, Key key) {
        cmp_swap(c, key, 0, 9);
        cmp_swap(c, key, 1, 20);
        cmp_swap(c, key, 2, 21);
        cmp_swap(c, key, 3, 22);
        cmp_swap(c, key, 4, 19);
        cmp_swap(c, key, 5, 24);
        cmp_swap(c, key, 6, 25);
        cmp_swap(c, key, 7, 26);
        cmp_swap(c, key, 8, 23);
        cmp_swap(c, key, 10, 15);
        cmp_swap(c, key, 11, 13);
        cmp_swap(c, key, 12, 17);
        cmp_swap(c, key, 14, 16);
        cmp_swap(c, key, 18, 27);
        cmp_swap(c, key, 0, 18);
        cmp_swap(c, key, 1, 7);
        cmp_swap(c, key, 2, 6);
        cmp_swap(c, key, 3, 5);
        cmp_swap(c, key, 4, 8);
        cmp_swap(c, key, 9, 27);
        cmp_swap(c, key, 10, 12);
        cmp_swap(c, key, 11, 14);
        cmp_swap(c, key, 13, 16);
        cmp_swap(c, key, 15, 17);
        cmp_swap(c, key, 19, 23);
        cmp_swap(c, key, 20, 26);
        cmp_swap(c, key, 21, 25);
        cmp_swap(c, key, 22, 24);
        cmp_swap(c, key, 1, 2);
        cmp_swap(c, key, 3, 4);
        cmp_swap(c, key, 5, 19);
        cmp_swap(c, key, 6, 20);
        cmp_swap(c, key, 7, 21);
        cmp_swap(c, key, 8, 22);
        cmp_swap(c, key, 9, 18);
        cmp_swap(c, key, 10, 11);
        cmp_swap(c, key, 12, 14);
        cmp_swap(c, key, 13, 15);
        cmp_swap(c, key, 16, 17);
        cmp_swap(c, key, 23, 24);
        cmp_swap(c, key, 25, 26);
        cmp_swap(c, key, 0, 3);
        cmp_swap(c, key, 1, 10);
        cmp_swap(c, key, 5, 8);
        cmp_swap(c, key, 6, 7);
        cmp_swap(c, key, 11, 13);
        cmp_swap(c, key, 14, 16);
        cmp_swap(c, key, 17, 26);
        cmp_swap(c, key, 19, 22);
        cmp_swap(c, key, 20, 21);
        cmp_swap(c, key, 24, 27);
        cmp_swap(c, key, 0, 1);
        cmp_swap(c, key, 2, 7);
        cmp_swap(c, key, 3, 10);
        cmp_swap(c, key, 4, 8);
        cmp_swap(c, key, 12, 13);
        cmp_swap(c, key, 14, 15);
        cmp_swap(c, key, 17, 24);
        cmp_swap(c, key, 19, 23);
        cmp_swap(c, key, 20, 25);
        cmp_swap(c, key, 26, 27);
        cmp_swap(c, key, 1, 3);
        cmp_swap(c, key, 2, 6);
        cmp_swap(c, key, 4, 5);
        cmp_swap(c, key, 7, 19);
        cmp_swap(c, key, 8, 20);
        cmp_swap(c, key, 11, 12);
        cmp_swap(c, key, 13, 14);
        cmp_swap(c, key, 15, 16);
        cmp_swap(c, key, 21, 25);
        cmp_swap(c, key, 22, 23);
        cmp_swap(c, key, 24, 26);
        cmp_swap(c, key, 2, 4);
        cmp_swap(c, key, 5, 12);
        cmp_swap(c, key, 7, 8);
        cmp_swap(c, key, 9, 11);
        cmp_swap(c, key, 10, 14);
        cmp_swap(c, key, 13, 17);
        cmp_swap(c, key, 15, 22);
        cmp_swap(c, key, 16, 18);
        cmp_swap(c, key, 19, 20);
        cmp_swap(c, key, 23, 25);
        cmp_swap(c, key, 2, 9);
        cmp_swap(c, key, 4, 11);
        cmp_swap(c, key, 5, 6);
        cmp_swap(c, key, 7, 13);
        cmp_swap(c, key, 8, 10);
        cmp_swap(c, key, 14, 20);
        cmp_swap(c, key, 16, 23);
        cmp_swap(c, key, 17, 19);
        cmp_swap(c, key, 18, 25);
        cmp_swap(c, key, 21, 22);
        cmp_swap(c, key, 1, 2);
        cmp_swap(c, key, 3, 16);
        cmp_swap(c, key, 4, 9);
        cmp_swap(c, key, 6, 12);
        cmp_swap(c, key, 10, 14);
        cmp_swap(c, key, 11, 24);
        cmp_swap(c, key, 13, 17);
        cmp_swap(c, key, 15, 21);
        cmp_swap(c, key, 18, 23);
        cmp_swap(c, key, 25, 26);
        cmp_swap(c, key, 2, 8);
        cmp_swap(c, key, 3, 5);
        cmp_swap(c, key, 4, 7);
        cmp_swap(c, key, 6, 16);
        cmp_swap(c, key, 9, 15);
        cmp_swap(c, key, 11, 21);
        cmp_swap(c, key, 12, 18);
        cmp_swap(c, key, 19, 25);
        cmp_swap(c, key, 20, 23);
        cmp_swap(c, key, 22, 24);
        cmp_swap(c, key, 2, 3);
        cmp_swap(c, key, 5, 8);
        cmp_swap(c, key, 7, 9);
        cmp_swap(c, key, 11, 15);
        cmp_swap(c, key, 12, 16);
        cmp_swap(c, key, 18, 20);
        cmp_swap(c, key, 19, 22);
        cmp_swap(c, key, 24, 25);
        cmp_swap(c, key, 6, 8);
        cmp_swap(c, key, 10, 12);
        cmp_swap(c, key, 11, 13);
        cmp_swap(c, key, 14, 16);
        cmp_swap(c, key, 15, 17);
        cmp_swap(c, key, 19, 21);
        cmp_swap(c, key, 5, 6);
        cmp_swap(c, key, 8, 10);
        cmp_swap(c, key, 9, 11);
        cmp_swap(c, key, 12, 13);
        cmp_swap(c, key, 14, 15);
        cmp_swap(c, key, 16, 18);
        cmp_swap(c, key, 17, 19);
        cmp_swap(c, key, 21, 22);
        cmp_swap(c, key, 4, 5);
        cmp_swap(c, key, 6, 7);
        cmp_swap(c, key, 8, 9);
        cmp_swap(c, key, 10, 11);
        cmp_swap(c, key, 12, 14);
        cmp_swap(c, key, 13, 15);
        cmp_swap(c, key, 16, 17);
        cmp_swap(c, key, 18, 19);
        cmp_swap(c, key, 20, 21);
        cmp_swap(c, key, 22, 23);
        cmp_swap(c, key, 3, 4);
        cmp_swap(c, key, 5, 6);
        cmp_swap(c, key, 7, 8);
        cmp_swap(c, key, 9, 10);
        cmp_swap(c, key, 11, 12);
        cmp_swap(c, key, 13, 14);
        cmp_swap(c, key, 15, 16);
        cmp_swap(c, key, 17, 18);
        cmp_swap(c, key, 19, 20);
        cmp_swap(c, key, 21, 22);
        cmp_swap(c, key, 23, 24);
    }

    template <typename C, typename Key>
    void s29(C& c, Key key) {
        cmp_swap(c, key, 0, 12);
        cmp_swap(c, key, 1, 10);
        cmp_swap(c, key, 2, 9);
        cmp_swap(c, key, 3, 7);
        cmp_swap(c, key, 5, 11);
        cmp_swap(c, key, 6, 8);
        cmp_swap(c, key, 13, 26);
        cmp_swap(c, key, 14, 25);
        cmp_swap(c, key, 15, 28);
        cmp_swap(c, key, 16, 27);
        cmp_swap(c, key, 17, 21);
        cmp_swap(c, key, 18, 19);
        cmp_swap(c, key, 20, 24);
        cmp_swap(c, key, 22, 23);
        cmp_swap(c, key, 1, 6);
        cmp_swap(c, key, 2, 3);
        cmp_swap(c, key, 4, 11);
        cmp_swap(c, key, 7, 9);
        cmp_swap(c, key, 8, 10);
        cmp_swap(c, key, 13, 18);
        cmp_swap(c, key, 14, 20);
        cmp_swap(c, key, 15, 22);
        cmp_swap(c, key, 16, 17);
        cmp_swap(c, key, 19, 26);
        cmp_swap(c, key, 21, 27);
        cmp_swap(c, key, 23, 28);
        cmp_swap(c, key, 24, 25);
        cmp_swap(c, key, 0, 4);
        cmp_swap(c, key, 1, 2);
        cmp_swap(c, key, 3, 6);
        cmp_swap(c, key, 7, 8);
        cmp_swap(c, key, 9, 10);
        cmp_swap(c, key, 11, 12);
        cmp_swap(c, key, 13, 14);
        cmp_swap(c, key, 15, 16);
        cmp_swap(c, key, 17, 18);
        cmp_swap(c, key, 19, 21);
        cmp_swap(c, key, 20, 22);
        cmp_swap(c, key, 23, 24);
        cmp_swap(c, key, 25, 26);
        cmp_swap(c, key, 27, 28);
        cmp_swap(c, key, 4, 6);
        cmp_swap(c, key, 5, 9);
        cmp_swap(c, key, 8, 11);
        cmp_swap(c, key, 10, 12);
        cmp_swap(c, key, 13, 15);
        cmp_swap(c, key, 14, 16);
        cmp_swap(c, key, 17, 23);
        cmp_swap(c, key, 18, 24);
        cmp_swap(c, key, 19, 20);
        cmp_swap(c, key, 21, 22);
        cmp_swap(c, key, 25, 27);
        cmp_swap(c, key, 26, 28);
        cmp_swap(c, key, 0, 5);
        cmp_swap(c, key, 3, 8);
        cmp_swap(c, key, 4, 7);
        cmp_swap(c, key, 6, 11);
        cmp_swap(c, key, 9, 10);
        cmp_swap(c, key, 14, 15);
        cmp_swap(c, key, 16, 25);
        cmp_swap(c, key, 17, 19);
        cmp_swap(c, key, 18, 20);
        cmp_swap(c, key, 21, 23);
        cmp_swap(c, key, 22, 24);
        cmp_swap(c, key, 26, 27);
        cmp_swap(c, key, 0, 1);
        cmp_swap(c, key, 2, 5);
        cmp_swap(c, key, 6, 9);
        cmp_swap(c, key, 7, 8);
        cmp_swap(c, key, 10, 11);
        cmp_swap(c, key, 14, 17);
        cmp_swap(c, key, 15, 19);
        cmp_swap(c, key, 18, 21);
        cmp_swap(c, key, 20, 23);
        cmp_swap(c, key, 22, 26);
        cmp_swap(c, key, 24, 27);
        cmp_swap(c, key, 0, 13);
        cmp_swap(c, key, 1, 3);
        cmp_swap(c, key, 2, 4);
        cmp_swap(c, key, 5, 6);
        cmp_swap(c, key, 9, 10);
        cmp_swap(c, key, 15, 17);
        cmp_swap(c, key, 16, 19);
        cmp_swap(c, key, 22, 25);
        cmp_swap(c, key, 24, 26);
        cmp_swap(c, key, 1, 2);
        cmp_swap(c, key, 3, 4);
        cmp_swap(c, key, 5, 7);
        cmp_swap(c, key, 6, 8);
        cmp_swap(c, key, 16, 18);
        cmp_swap(c, key, 19, 21);
        cmp_swap(c, key, 20, 22);
        cmp_swap(c, key, 23, 25);
        cmp_swap(c, key, 1, 14);
        cmp_swap(c, key, 2, 3);
        cmp_swap(c, key, 4, 5);
        cmp_swap(c, key, 6, 7);
        cmp_swap(c, key, 8, 9);
        cmp_swap(c, key, 16, 17);
        cmp_swap(c, key, 18, 19);
        cmp_swap(c, key, 20, 21);
        cmp_swap(c, key, 22, 23);
        cmp_swap(c, key, 24, 25);
        cmp_swap(c, key, 2, 15);
        cmp_swap(c, key, 3, 4);
        cmp_swap(c, key, 5, 6);
        cmp_swap(c, key, 10, 23);
        cmp_swap(c, key, 11, 24);
        cmp_swap(c, key, 12, 25);
        cmp_swap(c, key, 19, 20);
        cmp_swap(c, key, 21, 22);
        cmp_swap(c, key, 3, 16);
        cmp_swap(c, key, 4, 17);
        cmp_swap(c, key, 5, 18);
        cmp_swap(c, key, 6, 19);
        cmp_swap(c, key, 7, 20);
        cmp_swap(c, key, 8, 21);
        cmp_swap(c, key, 9, 22);
        cmp_swap(c, key, 10, 15);
        cmp_swap(c, key, 6, 10);
        cmp_swap(c, key, 8, 13);
        cmp_swap(c, key, 9, 14);
        cmp_swap(c, key, 11, 16);
        cmp_swap(c, key, 12, 17);
        cmp_swap(c, key, 18, 26);
        cmp_swap(c, key, 19, 27);
        cmp_swap(c, key, 20, 28);
        cmp_swap(c, key, 4, 8);
        cmp_swap(c, key, 5, 9);
        cmp_swap(c, key, 7, 11);
        cmp_swap(c, key, 12, 13);
        cmp_swap(c, key, 14, 18);
        cmp_swap(c, key, 15, 19);
        cmp_swap(c, key, 16, 20);
        cmp_swap(c, key, 17, 21);
        cmp_swap(c, key, 22, 26);
        cmp_swap(c, key, 23, 27);
        cmp_swap(c, key, 24, 28);
        cmp_swap(c, key, 2, 4);
        cmp_swap(c, key, 3, 5);
        cmp_swap(c, key, 6, 8);
        cmp_swap(c, key, 7, 9);
        cmp_swap(c, key, 10, 12);
        cmp_swap(c, key, 11, 14);
        cmp_swap(c, key, 13, 15);
        cmp_swap(c, key, 16, 18);
        cmp_swap(c, key, 17, 19);
        cmp_swap(c, key, 20, 22);
        cmp_swap(c, key, 21, 23);
        cmp_swap(c, key, 24, 26);
        cmp_swap(c, key, 25, 27);
        cmp_swap(c, key, 1, 2);
        cmp_swap(c, key, 3, 4);
        cmp_swap(c, key, 5, 6);
        cmp_swap(c, key, 7, 8);
        cmp_swap(c, key, 9, 10);
        cmp_swap(c, key, 11, 12);
        cmp_swap(c, key, 13, 14);
        cmp_swap(c, key, 15, 16);
        cmp_swap(c, key, 17, 18);
        cmp_swap(c, key, 19, 20);
        cmp_swap(c, key, 21, 22);
        cmp_swap(c, key, 23, 24);
        cmp_swap(c, key, 25, 26);
        cmp_swap(c, key, 27, 28);
    }

    template <typename C, typename Key>
    void s30(C& c, Key key) {
        cmp_swap(c, key, 1, 2);
        cmp_swap(c, key, 3, 10);
        cmp_swap(c, key, 4, 14);
        cmp_swap(c, key, 5, 8);
        cmp_swap(c, key, 6, 13);
        cmp_swap(c, key, 7, 12);
        cmp_swap(c, key, 9, 11);
        cmp_swap(c, key, 16, 17);
        cmp_swap(c, key, 18, 25);
        cmp_swap(c, key, 19, 29);
        cmp_swap(c, key, 20, 23);
        cmp_swap(c, key, 21, 28);
        cmp_swap(c, key, 22, 27);
        cmp_swap(c, key, 24, 26);
        cmp_swap(c, key, 0, 14);
        cmp_swap(c, key, 1, 5);
        cmp_swap(c, key, 2, 8);
        cmp_swap(c, key, 3, 7);
        cmp_swap(c, key, 6, 9);
        cmp_swap(c, key, 10, 12);
        cmp_swap(c, key, 11, 13);
        cmp_swap(c, key, 15, 29);
        cmp_swap(c, key, 16, 20);
        cmp_swap(c, key, 17, 23);
        cmp_swap(c, key, 18, 22);
        cmp_swap(c, key, 21, 24);
        cmp_swap(c, key, 25, 27);
        cmp_swap(c, key, 26, 28);
        cmp_swap(c, key, 0, 7);
        cmp_swap(c, key, 1, 6);
        cmp_swap(c, key, 2, 9);
        cmp_swap(c, key, 4, 10);
        cmp_swap(c, key, 5, 11);
        cmp_swap(c, key, 8, 13);
        cmp_swap(c, key, 12, 14);
        cmp_swap(c, key, 15, 22);
        cmp_swap(c, key, 16, 21);
        cmp_swap(c, key, 17, 24);
        cmp_swap(c, key, 19, 25);
        cmp_swap(c, key, 20, 26);
        cmp_swap(c, key, 23, 28);
        cmp_swap(c, key, 27, 29);
        cmp_swap(c, key, 0, 6);
        cmp_swap(c, key, 2, 4);
        cmp_swap(c, key, 3, 5);
        cmp_swap(c, key, 7, 11);
        cmp_swap(c, key, 8, 10);
        cmp_swap(c, key, 9, 12);
        cmp_swap(c, key, 13, 14);
        cmp_swap(c, key, 15, 21);
        cmp_swap(c, key, 17, 19);
        cmp_swap(c, key, 18, 20);
        cmp_swap(c, key, 22, 26);
        cmp_swap(c, key, 23, 25);
        cmp_swap(c, key, 24, 27);
        cmp_swap(c, key, 28, 29);
        cmp_swap(c, key, 0, 3);
        cmp_swap(c, key, 1, 2);
        cmp_swap(c, key, 4, 7);
        cmp_swap(c, key, 5, 9);
        cmp_swap(c, key, 6, 8);
        cmp_swap(c, key, 10, 11);
        cmp_swap(c, key, 12, 13);
        cmp_swap(c, key, 14, 29);
        cmp_swap(c, key, 15, 18);
        cmp_swap(c, key, 16, 17);
        cmp_swap(c, key, 19, 22);
        cmp_swap(c, key, 20, 24);
        cmp_swap(c, key, 21, 23);
        cmp_swap(c, key, 25, 26);
        cmp_swap(c, key, 27, 28);
        cmp_swap(c, key, 0, 1);
        cmp_swap(c, key, 2, 3);
        cmp_swap(c, key, 4, 6);
        cmp_swap(c, key, 7, 9);
        cmp_swap(c, key, 10, 12);
        cmp_swap(c, key, 11, 13);
        cmp_swap(c, key, 15, 16);
        cmp_swap(c, key, 17, 18);
        cmp_swap(c, key, 19, 21);
        cmp_swap(c, key, 22, 24);
        cmp_swap(c, key, 25, 27);
        cmp_swap(c, key, 26, 28);
        cmp_swap(c, key, 0, 15);
        cmp_swap(c, key, 1, 2);
        cmp_swap(c, key, 3, 5);
        cmp_swap(c, key, 8, 10);
        cmp_swap(c, key, 11, 12);
        cmp_swap(c, key, 13, 28);
        cmp_swap(c, key, 16, 17);
        cmp_swap(c, key, 18, 20);
        cmp_swap(c, key, 23, 25);
        cmp_swap(c, key, 26, 27);
        cmp_swap(c, key, 1, 16);
        cmp_swap(c, key, 3, 4);
        cmp_swap(c, key, 5, 6);
        cmp_swap(c, key, 7, 8);
        cmp_swap(c, key, 9, 10);
        cmp_swap(c, key, 12, 27);
        cmp_swap(c, key, 18, 19);
        cmp_swap(c, key, 20, 21);
        cmp_swap(c, key, 22, 23);
        cmp_swap(c, key, 24, 25);
        cmp_swap(c, key, 2, 3);
        cmp_swap(c, key, 4, 5);
        cmp_swap(c, key, 6, 7);
        cmp_swap(c, key, 8, 9);
        cmp_swap(c, key, 10, 11);
        cmp_swap(c, key, 17, 18);
        cmp_swap(c, key, 19, 20);
        cmp_swap(c, key, 21, 22);
        cmp_swap(c, key, 23, 24);
        cmp_swap(c, key, 25, 26);
        cmp_swap(c, key, 2, 17);
        cmp_swap(c, key, 3, 18);
        cmp_swap(c, key, 4, 19);
        cmp_swap(c, key, 5, 6);
        cmp_swap(c, key, 7, 8);
        cmp_swap(c, key, 9, 24);
        cmp_swap(c, key, 10, 25);
        cmp_swap(c, key, 11, 26);
        cmp_swap(c, key, 20, 21);
        cmp_swap(c, key, 22, 23);
        cmp_swap(c, key, 5, 20);
        cmp_swap(c, key, 6, 21);
        cmp_swap(c, key, 7, 22);
        cmp_swap(c, key, 8, 23);
        cmp_swap(c, key, 9, 16);
        cmp_swap(c, key, 10, 17);
        cmp_swap(c, key, 11, 18);
        cmp_swap(c, key, 12, 19);
        cmp_swap(c, key, 5, 9);
        cmp_swap(c, key, 6, 10);
        cmp_swap(c, key, 7, 11);
        cmp_swap(c, key, 8, 15);
        cmp_swap(c, key, 13, 20);
        cmp_swap(c, key, 14, 21);
        cmp_swap(c, key, 18, 22);
        cmp_swap(c, key, 19, 23);
        cmp_swap(c, key, 3, 5);
        cmp_swap(c, key, 4, 8);
        cmp_swap(c, key, 7, 9);
        cmp_swap(c, key, 12, 15);
        cmp_swap(c, key, 13, 16);
        cmp_swap(c, key, 14, 17);
        cmp_swap(c, key, 20, 24);
        cmp_swap(c, key, 21, 25);
        cmp_swap(c, key, 2, 4);
        cmp_swap(c, key, 6, 8);
        cmp_swap(c, key, 10, 12);
        cmp_swap(c, key, 11, 13);
        cmp_swap(c, key, 14, 15);
        cmp_swap(c, key, 16, 18);
        cmp_swap(c, key, 17, 19);
        cmp_swap(c, key, 20, 22);
        cmp_swap(c, key, 21, 23);
        cmp_swap(c, key, 24, 26);
        cmp_swap(c, key, 25, 27);
        cmp_swap(c, key, 1, 2);
        cmp_swap(c, key, 3, 4);
        cmp_swap(c, key, 5, 6);
        cmp_swap(c, key, 7, 8);
        cmp_swap(c, key, 9, 10);
        cmp_swap(c, key, 11, 12);
        cmp_swap(c, key, 13, 14);
        cmp_swap(c, key, 15, 16);
        cmp_swap(c, key, 17, 18);
        cmp_swap(c, key, 19, 20);
        cmp_swap(c, key, 21, 22);
        cmp_swap(c, key, 23, 24);
        cmp_swap(c, key, 25, 26);
        cmp_swap(c, key, 27, 28);
    }

    template <typename C, typename Key>
    void s31(C& c, Key key) {
        cmp_swap(c, key, 0, 1);
        cmp_swap(c, key, 2, 3);
        cmp_swap(c, key, 4, 5);
        cmp_swap(c, key, 6, 7);
        cmp_swap(c, key, 8, 9);
        cmp_swap(c, key, 10, 11);
        cmp_swap(c, key, 12, 13);
        cmp_swap(c, key, 14, 15);
        cmp_swap(c, key, 16, 17);
        cmp_swap(c, key, 18, 19);
        cmp_swap(c, key, 20, 21);
        cmp_swap(c, key, 22, 23);
        cmp_swap(c, key, 24, 25);
        cmp_swap(c, key, 26, 27);
        cmp_swap(c, key, 28, 29);
        cmp_swap(c, key, 0, 2);
        cmp_swap(c, key, 1, 3);
        cmp_swap(c, key, 4, 6);
        cmp_swap(c, key, 5, 7);
        cmp_swap(c, key, 8, 10);
        cmp_swap(c, key, 9, 11);
        cmp_swap(c, key, 12, 14);
        cmp_swap(c, key, 13, 15);
        cmp_swap(c, key, 16, 18);
        cmp_swap(c, key, 17, 19);
        cmp_swap(c, key, 20, 22);
        cmp_swap(c, key, 21, 23);
        cmp_swap(c, key, 24, 26);
        cmp_swap(c, key, 25, 27);
        cmp_swap(c, key, 28, 30);
        cmp_swap(c, key, 0, 4);
        cmp_swap(c, key, 1, 5);
        cmp_swap(c, key, 2, 6);
        cmp_swap(c, key, 3, 7);
        cmp_swap(c, key, 8, 12);
        cmp_swap(c, key, 9, 13);
        cmp_swap(c, key, 10, 14);
        cmp_swap(c, key, 11, 15);
        cmp_swap(c, key, 16, 20);
        cmp_swap(c, key, 17, 21);
        cmp_swap(c, key, 18, 22);
        cmp_swap(c, key, 19, 23);
        cmp_swap(c, key, 24, 28);
        cmp_swap(c, key, 25, 29);
        cmp_swap(c, key, 26, 30);
        cmp_swap(c, key, 0, 8);
        cmp_swap(c, key, 1, 9);
        cmp_swap(c, key, 2, 10);
        cmp_swap(c, key, 3, 11);
        cmp_swap(c, key, 4, 12);
        cmp_swap(c, key, 5, 13);
        cmp_swap(c, key, 6, 14);
        cmp_swap(c, key, 7, 15);
        cmp_swap(c, key, 16, 24);
        cmp_swap(c, key, 17, 25);
        cmp_swap(c, key, 18, 26);
        cmp_swap(c, key, 19, 27);
        cmp_swap(c, key, 20, 28);
        cmp_swap(c, key, 21, 29);
        cmp_swap(c, key, 22, 30);
        cmp_swap(c, key, 0, 16);
        cmp_swap(c, key, 1, 8);
        cmp_swap(c, key, 2, 4);
        cmp_swap(c, key, 3, 12);
        cmp_swap(c, key, 5, 10);
        cmp_swap(c, key, 6, 9);
        cmp_swap(c, key, 7, 14);
        cmp_swap(c, key, 11, 13);
        cmp_swap(c, key, 17, 24);
        cmp_swap(c, key, 18, 20);
        cmp_swap(c, key, 19, 28);
        cmp_swap(c, key, 21, 26);
        cmp_swap(c, key, 22, 25);
        cmp_swap(c, key, 23, 30);
        cmp_swap(c, key, 27, 29);
        cmp_swap(c, key, 1, 2);
        cmp_swap(c, key, 3, 5);
        cmp_swap(c, key, 4, 8);
        cmp_swap(c, key, 6, 22);
        cmp_swap(c, key, 7, 11);
        cmp_swap(c, key, 9, 25);
        cmp_swap(c, key, 10, 12);
        cmp_swap(c, key, 13, 14);
        cmp_swap(c, key, 17, 18);
        cmp_swap(c, key, 19, 21);
        cmp_swap(c, key, 20, 24);
        cmp_swap(c, key, 23, 27);
        cmp_swap(c, key, 26, 28);
        cmp_swap(c, key, 29, 30);
        cmp_swap(c, key, 1, 17);
        cmp_swap(c, key, 2, 18);
        cmp_swap(c, key, 3, 19);
        cmp_swap(c, key, 4, 20);
        cmp_swap(c, key, 5, 10);
        cmp_swap(c, key, 7, 23);
        cmp_swap(c, key, 8, 24);
        cmp_swap(c, key, 11, 27);
        cmp_swap(c, key, 12, 28);
        cmp_swap(c, key, 13, 29);
        cmp_swap(c, key, 14, 30);
        cmp_swap(c, key, 21, 26);
        cmp_swap(c, key, 3, 17);
        cmp_swap(c, key, 4, 16);
        cmp_swap(c, key, 5, 21);
        cmp_swap(c, key, 6, 18);
        cmp_swap(c, key, 7, 9);
        cmp_swap(c, key, 8, 20);
        cmp_swap(c, key, 10, 26);
        cmp_swap(c, key, 11, 23);
        cmp_swap(c, key, 13, 25);
        cmp_swap(c, key, 14, 28);
        cmp_swap(c, key, 15, 27);
        cmp_swap(c, key, 22, 24);
        cmp_swap(c, key, 1, 4);
        cmp_swap(c, key, 3, 8);
        cmp_swap(c, key, 5, 16);
        cmp_swap(c, key, 7, 17);
        cmp_swap(c, key, 9, 21);
        cmp_swap(c, key, 10, 22);
        cmp_swap(c, key, 11, 19);
        cmp_swap(c, key, 12, 20);
        cmp_swap(c, key, 14, 24);
        cmp_swap(c, key, 15, 26);
        cmp_swap(c, key, 23, 28);
        cmp_swap(c, key, 27, 30);
        cmp_swap(c, key, 2, 5);
        cmp_swap(c, key, 7, 8);
        cmp_swap(c, key, 9, 18);
        cmp_swap(c, key, 11, 17);
        cmp_swap(c, key, 12, 16);
        cmp_swap(c, key, 13, 22);
        cmp_swap(c, key, 14, 20);
        cmp_swap(c, key, 15, 19);
        cmp_swap(c, key, 23, 24);
        cmp_swap(c, key, 26, 29);
        cmp_swap(c, key, 2, 4);
        cmp_swap(c, key, 6, 12);
        cmp_swap(c, key, 9, 16);
        cmp_swap(c, key, 10, 11);
        cmp_swap(c, key, 13, 17);
        cmp_swap(c, key, 14, 18);
        cmp_swap(c, key, 15, 22);
        cmp_swap(c, key, 19, 25);
        cmp_swap(c, key, 20, 21);
        cmp_swap(c, key, 27, 29);
        cmp_swap(c, key, 5, 6);
        cmp_swap(c, key, 8, 12);
        cmp_swap(c, key, 9, 10);
        cmp_swap(c, key, 11, 13);
        cmp_swap(c, key, 14, 16);
        cmp_swap(c, key, 15, 17);
        cmp_swap(c, key, 18, 20);
        cmp_swap(c, key, 19, 23);
        cmp_swap(c, key, 21, 22);
        cmp_swap(c, key, 25, 26);
        cmp_swap(c, key, 3, 5);
        cmp_swap(c, key, 6, 7);
        cmp_swap(c, key, 8, 9);
        cmp_swap(c, key, 10, 12);
        cmp_swap(c, key, 11, 14);
        cmp_swap(c, key, 13, 16);
        cmp_swap(c, key, 15, 18);
        cmp_swap(c, key, 17, 20);
        cmp_swap(c, key, 19, 21);
        cmp_swap(c, key, 22, 23);
        cmp_swap(c, key, 24, 25);
        cmp_swap(c, key, 26, 28);
        cmp_swap(c, key, 3, 4);
        cmp_swap(c, key, 5, 6);
        cmp_swap(c, key, 7, 8);
        cmp_swap(c, key, 9, 10);
        cmp_swap(c, key, 11, 12);
        cmp_swap(c, key, 13, 14);
        cmp_swap(c, key, 15, 16);
        cmp_swap(c, key, 17, 18);
        cmp_swap(c, key, 19, 20);
        cmp_swap(c, key, 21, 22);
        cmp_swap(c, key, 23, 24);
        cmp_swap(c, key, 25, 26);
        cmp_swap(c, key, 27, 28);
    }

    template <typename C, typename Key>
    void s32(C& c, Key key) {
        cmp_swap(c, key, 0, 1);
        cmp_swap(c, key, 2, 3);
        cmp_swap(c, key, 4, 5);
        cmp_swap(c, key, 6, 7);
        cmp_swap(c, key, 8, 9);
        cmp_swap(c, key, 10, 11);
        cmp_swap(c, key, 12, 13);
        cmp_swap(c, key, 14, 15);
        cmp_swap(c, key, 16, 17);
        cmp_swap(c, key, 18, 19);
        cmp_swap(c, key, 20, 21);
        cmp_swap(c, key, 22, 23);
        cmp_swap(c, key, 24, 25);
        cmp_swap(c, key, 26, 27);
        cmp_swap(c, key, 28, 29);
        cmp_swap(c, key, 30, 31);
        cmp_swap(c, key, 0, 2);
        cmp_swap(c, key, 1, 3);
        cmp_swap(c, key, 4, 6);
        cmp_swap(c, key, 5, 7);
        cmp_swap(c, key, 8, 10);
        cmp_swap(c, key, 9, 11);
        cmp_swap(c, key, 12, 14);
        cmp_swap(c, key, 13, 15);
        cmp_swap(c, key, 16, 18);
        cmp_swap(c, key, 17, 19);
        cmp_swap(c, key, 20, 22);
        cmp_swap(c, key, 21, 23);
        cmp_swap(c, key, 24, 26);
        cmp_swap(c, key, 25, 27);
        cmp_swap(c, key, 28, 30);
        cmp_swap(c, key, 29, 31);
        cmp_swap(c, key, 0, 4);
        cmp_swap(c, key, 1, 5);
        cmp_swap(c, key, 2, 6);
        cmp_swap(c, key, 3, 7);
        cmp_swap(c, key, 8, 12);
        cmp_swap(c, key, 9, 13);
        cmp_swap(c, key, 10, 14);
        cmp_swap(c, key, 11, 15);
        cmp_swap(c, key, 16, 20);
        cmp_swap(c, key, 17, 21);
        cmp_swap(c, key, 18, 22);
        cmp_swap(c, key, 19, 23);
        cmp_swap(c, key, 24, 28);
        cmp_swap(c, key, 25, 29);
        cmp_swap(c, key, 26, 30);
        cmp_swap(c, key, 27, 31);
        cmp_swap(c, key, 0, 8);
        cmp_swap(c, key, 1, 9);
        cmp_swap(c, key, 2, 10);
        cmp_swap(c, key, 3, 11);
        cmp_swap(c, key, 4, 12);
        cmp_swap(c, key, 5, 13);
        cmp_swap(c, key, 6, 14);
        cmp_swap(c, key, 7, 15);
        cmp_swap(c, key, 16, 24);
        cmp_swap(c, key, 17, 25);
        cmp_swap(c, key, 18, 26);
        cmp_swap(c, key, 19, 27);
        cmp_swap(c, key, 20, 28);
        cmp_swap(c, key, 21, 29);
        cmp_swap(c, key, 22, 30);
        cmp_swap(c, key, 23, 31);
        cmp_swap(c, key, 0, 16);
        cmp_swap(c, key, 1, 8);
        cmp_swap(c, key, 2, 4);
        cmp_swap(c, key, 3, 12);
        cmp_swap(c, key, 5, 10);
        cmp_swap(c, key, 6, 9);
        cmp_swap(c, key, 7, 14);
        cmp_swap(c, key, 11, 13);
        cmp_swap(c, key, 15, 31);
        cmp_swap(c, key, 17, 24);
        cmp_swap(c, key, 18, 20);
        cmp_swap(c, key, 19, 28);
        cmp_swap(c, key, 21, 26);
        cmp_swap(c, key, 22, 25);
        cmp_swap(c, key, 23, 30);
        cmp_swap(c, key, 27, 29);
        cmp_swap(c, key, 1, 2);
        cmp_swap(c, key, 3, 5);
        cmp_swap(c, key, 4, 8);
        cmp_swap(c, key, 6, 22);
        cmp_swap(c, key, 7, 11);
        cmp_swap(c, key, 9, 25);
        cmp_swap(c, key, 10, 12);
        cmp_swap(c, key, 13, 14);
        cmp_swap(c, key, 17, 18);
        cmp_swap(c, key, 19, 21);
        cmp_swap(c, key, 20, 24);
        cmp_swap(c, key, 23, 27);
        cmp_swap(c, key, 26, 28);
        cmp_swap(c, key, 29, 30);
        cmp_swap(c, key, 1, 17);
        cmp_swap(c, key, 2, 18);
        cmp_swap(c, key, 3, 19);
        cmp_swap(c, key, 4, 20);
        cmp_swap(c, key, 5, 10);
        cmp_swap(c, key, 7, 23);
        cmp_swap(c, key, 8, 24);
        cmp_swap(c, key, 11, 27);
        cmp_swap(c, key, 12, 28);
        cmp_swap(c, key, 13, 29);
        cmp_swap(c, key, 14, 30);
        cmp_swap(c, key, 21, 26);
        cmp_swap(c, key, 3, 17);
        cmp_swap(c, key, 4, 16);
        cmp_swap(c, key, 5, 21);
        cmp_swap(c, key, 6, 18);
        cmp_swap(c, key, 7, 9);
        cmp_swap(c, key, 8, 20);
        cmp_swap(c, key, 10, 26);
        cmp_swap(c, key, 11, 23);
        cmp_swap(c, key, 13, 25);
        cmp_swap(c, key, 14, 28);
        cmp_swap(c, key, 15, 27);
        cmp_swap(c, key, 22, 24);
        cmp_swap(c, key, 1, 4);
        cmp_swap(c, key, 3, 8);
        cmp_swap(c, key, 5, 16);
        cmp_swap(c, key, 7, 17);
        cmp_swap(c, key, 9, 21);
        cmp_swap(c, key, 10, 22);
        cmp_swap(c, key, 11, 19);
        cmp_swap(c, key, 12, 20);
        cmp_swap(c, key, 14, 24);
        cmp_swap(c, key, 15, 26);
        cmp_swap(c, key, 23, 28);
        cmp_swap(c, key, 27, 30);
        cmp_swap(c, key, 2, 5);
        cmp_swap(c, key, 7, 8);
        cmp_swap(c, key, 9, 18);
        cmp_swap(c, key, 11, 17);
        cmp_swap(c, key, 12, 16);
        cmp_swap(c, key, 13, 22);
        cmp_swap(c, key, 14, 20);
        cmp_swap(c, key, 15, 19);
        cmp_swap(c, key, 23, 24);
        cmp_swap(c, key, 26, 29);
        cmp_swap(c, key, 2, 4);
        cmp_swap(c, key, 6, 12);
        cmp_swap(c, key, 9, 16);
        cmp_swap(c, key, 10, 11);
        cmp_swap(c, key, 13, 17);
        cmp_swap(c, key, 14, 18);
        cmp_swap(c, key, 15, 22);
        cmp_swap(c, key, 19, 25);
        cmp_swap(c, key, 20, 21);
        cmp_swap(c, key, 27, 29);
        cmp_swap(c, key, 5, 6);
        cmp_swap(c, key, 8, 12);
        cmp_swap(c, key, 9, 10);
        cmp_swap(c, key, 11, 13);
        cmp_swap(c, key, 14, 16);
        cmp_swap(c, key, 15, 17);
        cmp_swap(c, key, 18, 20);
        cmp_swap(c, key, 19, 23);
        cmp_swap(c, key, 21, 22);
        cmp_swap(c, key, 25, 26);
        cmp_swap(c, key, 3, 5);
        cmp_swap(c, key, 6, 7);
        cmp_swap(c, key, 8, 9);
        cmp_swap(c, key, 10, 12);
        cmp_swap(c, key, 11, 14);
        cmp_swap(c, key, 13, 16);
        cmp_swap(c, key, 15, 18);
        cmp_swap(c, key, 17, 20);
        cmp_swap(c, key, 19, 21);
        cmp_swap(c, key, 22, 23);
        cmp_swap(c, key, 24, 25);
        cmp_swap(c, key, 26, 28);
        cmp_swap(c, key, 3, 4);
        cmp_swap(c, key, 5, 6);
        cmp_swap(c, key, 7, 8);
        cmp_swap(c, key, 9, 10);
        cmp_swap(c, key, 11, 12);
        cmp_swap(c, key, 13, 14);
        cmp_swap(c, key, 15, 16);
        cmp_swap(c, key, 17, 18);
        cmp_swap(c, key, 19, 20);
        cmp_swap(c, key, 21, 22);
        cmp_swap(c, key, 23, 24);
        cmp_swap(c, key, 25, 26);
        cmp_swap(c, key, 27, 28);
    }

}  // namespace netsort

template <typename C, typename K = IdentityFtor>
void net_dispatch(C& container, K key = {}) {
    switch (size(container)) {
    case 0:
    case 1:
        return;
#if CAV_MAX_NET_SIZE > 1
    case 2:
        return netsort::s2(container, key);
#if CAV_MAX_NET_SIZE > 2
    case 3:
        return netsort::s3(container, key);
#if CAV_MAX_NET_SIZE > 3
    case 4:
        return netsort::s4(container, key);
#if CAV_MAX_NET_SIZE > 4
    case 5:
        return netsort::s5(container, key);
#if CAV_MAX_NET_SIZE > 5
    case 6:
        return netsort::s6(container, key);
#if CAV_MAX_NET_SIZE > 6
    case 7:
        return netsort::s7(container, key);
#if CAV_MAX_NET_SIZE > 7
    case 8:
        return netsort::s8(container, key);
#if CAV_MAX_NET_SIZE > 8
    case 9:
        return netsort::s9(container, key);
#if CAV_MAX_NET_SIZE > 9
    case 10:
        return netsort::s10(container, key);
#if CAV_MAX_NET_SIZE > 10
    case 11:
        return netsort::s11(container, key);
#if CAV_MAX_NET_SIZE > 11
    case 12:
        return netsort::s12(container, key);
#if CAV_MAX_NET_SIZE > 12
    case 13:
        return netsort::s13(container, key);
#if CAV_MAX_NET_SIZE > 13
    case 14:
        return netsort::s14(container, key);
#if CAV_MAX_NET_SIZE > 14
    case 15:
        return netsort::s15(container, key);
#if CAV_MAX_NET_SIZE > 15
    case 16:
        return netsort::s16(container, key);
#if CAV_MAX_NET_SIZE > 16
    case 17:
        return netsort::s17(container, key);
#if CAV_MAX_NET_SIZE > 17
    case 18:
        return netsort::s18(container, key);
#if CAV_MAX_NET_SIZE > 18
    case 19:
        return netsort::s19(container, key);
#if CAV_MAX_NET_SIZE > 19
    case 20:
        return netsort::s20(container, key);
#if CAV_MAX_NET_SIZE > 20
    case 21:
        return netsort::s21(container, key);
#if CAV_MAX_NET_SIZE > 21
    case 22:
        return netsort::s22(container, key);
#if CAV_MAX_NET_SIZE > 22
    case 23:
        return netsort::s23(container, key);
#if CAV_MAX_NET_SIZE > 23
    case 24:
        return netsort::s24(container, key);
#if CAV_MAX_NET_SIZE > 24
    case 25:
        return netsort::s25(container, key);
#if CAV_MAX_NET_SIZE > 25
    case 26:
        return netsort::s26(container, key);
#if CAV_MAX_NET_SIZE > 26
    case 27:
        return netsort::s27(container, key);
#if CAV_MAX_NET_SIZE > 27
    case 28:
        return netsort::s28(container, key);
#if CAV_MAX_NET_SIZE > 28
    case 29:
        return netsort::s29(container, key);
#if CAV_MAX_NET_SIZE > 29
    case 30:
        return netsort::s30(container, key);
#if CAV_MAX_NET_SIZE > 30
    case 31:
        return netsort::s31(container, key);
#if CAV_MAX_NET_SIZE > 31
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
}  // namespace cav

#endif /* CAV_INCLUDE_UTILS_SORTING_NETWORKS_HPP */