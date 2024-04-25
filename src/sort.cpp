// SPDX-FileCopyrightText: 2024 Francesco Cavaliere <francescocava95@gmail.com>
// SPDX-License-Identifier: MIT

#include "sort.hpp"

#include <fmt/core.h>
#include <fmt/ranges.h>

#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <random>
#include <vector>

#include "Span.hpp"
#include "limits.hpp"

template <typename T, size_t K>
struct Fat {
    T    elem;
    char data[K - sizeof(T)];

    constexpr bool operator<(Fat const& other) const noexcept {
        return elem < other.elem;
    }
};

template <typename T, typename U>
__attribute__((noinline)) void sort_sequence(T&&                     sorter,
                                             std::vector<U>&         seq,
                                             std::vector<int> const& offsets) {
    for (size_t o = 0; o < offsets.size() - 1; ++o) {
        auto span = cav::make_span(seq.data() + offsets[o], seq.data() + offsets[o + 1]);
        sorter(span);
        // assert(std::is_sorted(span.begin(), span.end()));
    }
}

template <typename T, size_t N, typename K>
void run_test_loop(char const (&name)[N],
                   cav::Sorter<>&        sorter,
                   std::vector<T> const& origin,
                   int                   sub_size,
                   int                   tot_elems,
                   K                     key) {

    double duration_sec  = 0;
    double duration0_sec = 0;
    double duration1_sec = 0;
    double duration2_sec = 0;
    double duration3_sec = 0;
    double duration4_sec = 0;
    auto   out_buff      = std::vector<T>(origin.size());
    for (int i = 0; i < 1; ++i) {
        auto seq0 = origin;
        auto seq1 = origin;
        auto seq2 = origin;
        auto seq3 = origin;
        auto seq4 = origin;
        auto seq5 = origin;

        auto idis    = [&] { return sub_size - sub_size / 8 + rand() % (sub_size / 4); };
        auto offsets = std::vector<int>();
        for (size_t offset = 0; offset < seq0.size(); offset += idis())
            offsets.push_back(offset);
        if (offsets.back() != static_cast<int>(seq0.size()))
            offsets.push_back(seq0.size());

        auto t0 = std::chrono::high_resolution_clock::now();
        sort_sequence([&](cav::Span<T*> c) { sorter.net_sort(c, key); }, seq0, offsets);
        auto t1 = std::chrono::high_resolution_clock::now();
        sort_sequence([&](cav::Span<T*> c) { sorter.radix_sort_lsd(c, key); }, seq1, offsets);
        auto t2 = std::chrono::high_resolution_clock::now();
        sort_sequence([&](cav::Span<T*> c) { sorter.radix_sort_msd(c, key); }, seq2, offsets);
        auto t3 = std::chrono::high_resolution_clock::now();
        sort_sequence([&](cav::Span<T*> c) { sorter.sort(c, key); }, seq3, offsets);
        auto t4 = std::chrono::high_resolution_clock::now();
        sort_sequence(
            [&](cav::Span<T*> c) { std::sort(c.begin(), c.end(), cav::sort::make_comp_wrap(key)); },
            seq4,
            offsets);
        auto t5 = std::chrono::high_resolution_clock::now();
       sort_sequence(
            [&](cav::Span<T*> c) { std::sort(c.begin(), c.end(), cav::sort::make_comp_wrap(key)); },
            seq4,
            offsets);
        auto t6 = std::chrono::high_resolution_clock::now();

        duration_sec += std::chrono::duration<double>(t1 - t0).count();
        duration0_sec += std::chrono::duration<double>(t2 - t1).count();
        duration1_sec += std::chrono::duration<double>(t3 - t2).count();
        duration2_sec += std::chrono::duration<double>(t4 - t3).count();
        duration3_sec += std::chrono::duration<double>(t5 - t4).count();
        duration4_sec += std::chrono::duration<double>(t6 - t5).count();
#ifndef NDEBUG
        for (size_t i = 0; i < seq4.size(); ++i) {
            assert(key(seq0[i]) == key(seq4[i]));
            assert(key(seq1[i]) == key(seq4[i]));
            assert(key(seq2[i]) == key(seq4[i]));
            assert(key(seq3[i]) == key(seq4[i]));
        }
#endif
    }

    fmt::print(" {:10.0f} {:10.0f} {:10.0f} {:10.0f} {:10.0f} {:10.0f}\n",
               1e9 * duration_sec / cav::size(origin),
               1e9 * duration0_sec / cav::size(origin),
               1e9 * duration1_sec / cav::size(origin),
               1e9 * duration2_sec / cav::size(origin),
               1e9 * duration3_sec / cav::size(origin),
               1e9 * duration4_sec / cav::size(origin));
}

template <typename T, size_t N>
void run_test(char const (&name)[N], cav::Sorter<>& sorter, int sub_size, int tot_elems) {
    T    min = -std::sqrt(-(T{1} + cav::limits<T>::min()));
    T    max = std::sqrt(cav::limits<T>::max());
    auto dis = [min, max]() {
        double rand_val = static_cast<double>(rand()) / static_cast<double>(RAND_MAX);
        return min + rand_val * (max - min);
    };

    auto origin = std::vector<T>(tot_elems);
    for (int i = 0; i < tot_elems; ++i)
        origin[i] = static_cast<T>(dis());

    fmt::print("{:9} {:9} {:9} [{:7.1},{:6.1}]",
               name,
               sub_size,
               tot_elems / sub_size,
               double(min),
               double(max));
    run_test_loop(name, sorter, origin, sub_size, tot_elems, [](T const& a) { return a; });
}

template <typename T, size_t P, size_t N>
void run_test_fat(char const (&name)[N], cav::Sorter<>& sorter, int sub_size, int tot_elems) {
    T    min = -std::sqrt(-(T{1} + cav::limits<T>::min()));
    T    max = std::sqrt(cav::limits<T>::max());
    auto dis = [min, max]() {
        double rand_val = static_cast<double>(rand()) / static_cast<double>(RAND_MAX);
        return min + rand_val * (max - min);
    };

    auto origin = std::vector<Fat<T, P>>(tot_elems);
    for (int i = 0; i < tot_elems; ++i)
        origin[i] = Fat<T, P>{static_cast<T>(dis()), {}};


    fmt::print("{:9} {:9} {:9} [{:7.1},{:6.1}]",
               name,
               sub_size,
               tot_elems / sub_size,
               double(min),
               double(max));
    run_test_loop(name, sorter, origin, sub_size, tot_elems, [](Fat<T, P> const& a) {
        return a.elem;
    });
}

template <typename T, size_t N>
void run_test_indirect(char const (&name)[N], cav::Sorter<>& sorter, int sub_size, int tot_elems) {
    T    min = -std::sqrt(-(T{1} + cav::limits<T>::min()));
    T    max = std::sqrt(cav::limits<T>::max());
    auto dis = [min, max]() {
        double rand_val = static_cast<double>(rand()) / static_cast<double>(RAND_MAX);
        return min + rand_val * (max - min);
    };

    auto order  = std::vector<T>(tot_elems);
    auto origin = std::vector<T>(tot_elems);
    for (int i = 0; i < tot_elems; ++i) {
        order[i]  = static_cast<T>(dis());
        origin[i] = i;
    }

    fmt::print("{:9} {:9} {:9} [{:7.1},{:6.1}]",
               name,
               sub_size,
               tot_elems / sub_size,
               double(min),
               double(max));
    run_test_loop(name, sorter, origin, sub_size, tot_elems, [&](T const& a) { return order[a]; });
}

#ifndef NDEBUG
#define TOT_ELEMS 10000
#else
#define TOT_ELEMS 10000000
#endif
int main(int argc, char const** argv) {
    auto args = cav::make_span(argv, argc);

    auto sorter   = cav::make_sorter();
    int  sub_size = std::stoi(args[1]);

    fmt::print("type         length   samples            range   net-sort    lsd-rdx    msd-rdx   "
               "cav-sort   std-sort   ska-sort\n");
    // natives
    run_test<int8_t>("int8_t", sorter, sub_size, TOT_ELEMS);
    run_test<int16_t>("int16_t", sorter, sub_size, TOT_ELEMS);
    run_test<int32_t>("int32_t", sorter, sub_size, TOT_ELEMS);
    run_test<int64_t>("int64_t", sorter, sub_size, TOT_ELEMS);
    run_test<float>("float", sorter, sub_size, TOT_ELEMS);
    run_test<double>("double", sorter, sub_size, TOT_ELEMS);

    //// struct like
    run_test_fat<float, 8>("flt_8B", sorter, sub_size, TOT_ELEMS);
    run_test_fat<float, 16>("flt_16B", sorter, sub_size, TOT_ELEMS);
    run_test_fat<float, 32>("flt_32B", sorter, sub_size, TOT_ELEMS);
    run_test_fat<float, 64>("flt_64B", sorter, sub_size, TOT_ELEMS);

    run_test_fat<double, 16>("dbl_16B", sorter, sub_size, TOT_ELEMS);
    run_test_fat<double, 32>("dbl_32B", sorter, sub_size, TOT_ELEMS);
    run_test_fat<double, 64>("dbl_64B", sorter, sub_size, TOT_ELEMS);

    // indirect
    run_test_indirect<int32_t>("i32_ind", sorter, sub_size, TOT_ELEMS);
    run_test_indirect<int64_t>("i64_ind", sorter, sub_size, TOT_ELEMS);
    run_test_indirect<float>("flt_ind", sorter, sub_size, TOT_ELEMS);
    run_test_indirect<double>("dbl_ind", sorter, sub_size, TOT_ELEMS);

    return EXIT_SUCCESS;
}
