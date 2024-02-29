#include <fmt/core.h>
#include <fmt/ranges.h>

#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <random>
#include <vector>

#include "../include/sort.hpp"
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
        auto span = cft::make_span(seq.data() + offsets[o], seq.data() + offsets[o + 1]);
        sorter(span);
        // assert(std::is_sorted(span.begin(), span.end()));
    }
}

#define MY_RAND_MAX cft::limits<size_t>::max()

size_t my_rand() {  // period 2^96-1
    static size_t x = 123456789, y = 362436069, z = 521288629;
    size_t        t = 0;
    x ^= x << 16U;
    x ^= x >> 5U;
    x ^= x << 1U;
    t = x;
    x = y;
    y = z;
    z = t ^ x ^ y;
    return z;
}

template <typename T, size_t N>
void run_test(char const (&name)[N], cft::Sorter<>& sorter, int sub_size, int tot_elems) {
    T    min = cft::limits<T>::min() / static_cast<T>(32 * sizeof(T));
    T    max = cft::limits<T>::max() / static_cast<T>(32 * sizeof(T));
    auto dis = [min, max]() {
        double rand_val = static_cast<double>(my_rand()) / static_cast<double>(MY_RAND_MAX);
        return min + rand_val * (max - min);
    };

    auto   origin        = std::vector<T>(tot_elems);
    double duration_sec  = 0;
    double duration0_sec = 0;
    double duration1_sec = 0;
    double duration2_sec = 0;
    double duration3_sec = 0;
    double duration4_sec = 0;
    for (int i = 0; i < tot_elems; ++i)
        origin[i] = static_cast<T>(dis());

    fmt::print("Sorting ~{}x{} [{:7.1},{:6.1}]",
               sub_size,
               tot_elems / sub_size,
               double(min),
               double(max));
    for (int i = 0; i < 5; ++i) {
        auto seq  = origin;
        auto seq0 = origin;
        auto seq1 = origin;
        auto seq2 = origin;
        auto seq3 = origin;
        auto seq4 = origin;

        auto idis    = [&] { return sub_size - sub_size / 8 + my_rand() % (sub_size / 4); };
        auto offsets = std::vector<int>();
        for (size_t offset = 0; offset < seq.size(); offset += idis())
            offsets.push_back(offset);
        if (offsets.back() != static_cast<int>(seq.size()))
            offsets.push_back(seq.size());

        auto t0 = std::chrono::high_resolution_clock::now();
        sort_sequence([&](cft::Span<T*> c) { sorter.net_sort(c); }, seq, offsets);
        auto t1 = std::chrono::high_resolution_clock::now();
        sort_sequence([&](cft::Span<T*> c) { sorter.radix_sort(c); }, seq0, offsets);
        auto t2 = std::chrono::high_resolution_clock::now();
        sort_sequence([&](cft::Span<T*> c) { sorter.sort(c); }, seq1, offsets);
        auto t3 = std::chrono::high_resolution_clock::now();
        sort_sequence([&](cft::Span<T*> c) { sorter.nth_element(c, size(c) / 2); }, seq2, offsets);
        auto t4 = std::chrono::high_resolution_clock::now();
        sort_sequence([&](cft::Span<T*> c) { std::sort(c.begin(), c.end()); }, seq3, offsets);
        auto t5 = std::chrono::high_resolution_clock::now();
        sort_sequence(
            [&](cft::Span<T*> c) {
                std::nth_element(c.begin(), c.begin() + c.size() / 2, c.end());
            },
            seq4,
            offsets);
        auto t6 = std::chrono::high_resolution_clock::now();

        duration_sec += std::chrono::duration<double>(t1 - t0).count();
        duration0_sec += std::chrono::duration<double>(t2 - t1).count();
        duration1_sec += std::chrono::duration<double>(t3 - t2).count();
        duration2_sec += std::chrono::duration<double>(t4 - t3).count();
        duration3_sec += std::chrono::duration<double>(t5 - t4).count();
        duration4_sec += std::chrono::duration<double>(t6 - t5).count();
        assert(std::equal(seq.begin(), seq.end(), seq3.begin()));
        assert(std::equal(seq0.begin(), seq0.end(), seq3.begin()));
    }

    fmt::print(" {:8} net sort {:8.2}, rdx sort {:8.2}, my sort {:8.2}, my nth {:8.2}, std sort "
               "{:8.2}, std nth {:8.2}\n",
               name,
               sub_size * duration_sec / tot_elems,
               sub_size * duration0_sec / tot_elems,
               sub_size * duration1_sec / tot_elems,
               sub_size * duration2_sec / tot_elems,
               sub_size * duration3_sec / tot_elems,
               sub_size * duration4_sec / tot_elems);
}

template <typename T, size_t P, size_t N>
void run_test_fat(char const (&name)[N], cft::Sorter<>& sorter, int sub_size, int tot_elems) {
    T    min = cft::limits<T>::min() / static_cast<T>(32 * sizeof(T));
    T    max = cft::limits<T>::max() / static_cast<T>(32 * sizeof(T));
    auto dis = [min, max]() {
        double rand_val = static_cast<double>(my_rand()) / static_cast<double>(MY_RAND_MAX);
        return min + rand_val * (max - min);
    };

    auto   origin        = std::vector<Fat<T, P>>(tot_elems);
    double duration_sec  = 0;
    double duration0_sec = 0;
    double duration1_sec = 0;
    double duration2_sec = 0;
    double duration3_sec = 0;
    double duration4_sec = 0;
    for (int i = 0; i < tot_elems; ++i)
        origin[i] = Fat<T, P>{static_cast<T>(dis()), {}};

    fmt::print("Sorting ~{}x{} [{:7.1},{:6.1}]",
               sub_size,
               tot_elems / sub_size,
               double(min),
               double(max));
    for (int i = 0; i < 5; ++i) {
        auto seq  = origin;
        auto seq0 = origin;
        auto seq1 = origin;
        auto seq2 = origin;
        auto seq3 = origin;
        auto seq4 = origin;

        auto idis    = [&] { return sub_size - sub_size / 8 + my_rand() % (sub_size / 4); };
        auto offsets = std::vector<int>();
        for (size_t offset = 0; offset < seq.size(); offset += idis())
            offsets.push_back(offset);
        if (offsets.back() != static_cast<int>(seq.size()))
            offsets.push_back(seq.size());

        auto k = [](Fat<T, P> const& f) { return f.elem; };

        auto t0 = std::chrono::high_resolution_clock::now();
        sort_sequence([&](cft::Span<Fat<T, P>*> c) { sorter.net_sort(c, k); }, seq, offsets);
        auto t1 = std::chrono::high_resolution_clock::now();
        sort_sequence([&](cft::Span<Fat<T, P>*> c) { sorter.radix_sort(c, k); }, seq0, offsets);
        auto t2 = std::chrono::high_resolution_clock::now();
        sort_sequence([&](cft::Span<Fat<T, P>*> c) { sorter.sort(c, k); }, seq1, offsets);
        auto t3 = std::chrono::high_resolution_clock::now();
        sort_sequence([&](cft::Span<Fat<T, P>*> c) { sorter.nth_element(c, size(c) / 2, k); },
                      seq2,
                      offsets);
        auto t4 = std::chrono::high_resolution_clock::now();
        sort_sequence([&](cft::Span<Fat<T, P>*> c) { std::sort(c.begin(), c.end()); },
                      seq3,
                      offsets);
        auto t5 = std::chrono::high_resolution_clock::now();
        sort_sequence(
            [&](cft::Span<Fat<T, P>*> c) {
                std::nth_element(c.begin(), c.begin() + c.size() / 2, c.end());
            },
            seq4,
            offsets);
        auto t6 = std::chrono::high_resolution_clock::now();

        duration_sec += std::chrono::duration<double>(t1 - t0).count();
        duration0_sec += std::chrono::duration<double>(t2 - t1).count();
        duration1_sec += std::chrono::duration<double>(t3 - t2).count();
        duration2_sec += std::chrono::duration<double>(t4 - t3).count();
        duration3_sec += std::chrono::duration<double>(t5 - t4).count();
        duration4_sec += std::chrono::duration<double>(t6 - t5).count();
    }

    fmt::print(" {:8} net sort {:8.2}, rdx sort {:8.2}, my sort {:8.2}, my nth {:8.2}, std sort "
               "{:8.2}, std nth {:8.2}\n",
               name,
               sub_size * duration_sec / tot_elems,
               sub_size * duration0_sec / tot_elems,
               sub_size * duration1_sec / tot_elems,
               sub_size * duration2_sec / tot_elems,
               sub_size * duration3_sec / tot_elems,
               sub_size * duration4_sec / tot_elems);
}

template <typename T, size_t N>
void run_test_indirect(char const (&name)[N], cft::Sorter<>& sorter, int sub_size, int tot_elems) {
    T    min = cft::limits<T>::min() / static_cast<T>(32 * sizeof(T));
    T    max = cft::limits<T>::max() / static_cast<T>(32 * sizeof(T));
    auto dis = [min, max]() {
        double rand_val = static_cast<double>(my_rand()) / static_cast<double>(MY_RAND_MAX);
        return min + rand_val * (max - min);
    };

    auto   order         = std::vector<T>(tot_elems);
    double duration_sec  = 0;
    double duration0_sec = 0;
    double duration1_sec = 0;
    double duration2_sec = 0;
    double duration3_sec = 0;
    double duration4_sec = 0;
    for (int i = 0; i < tot_elems; ++i)
        order[i] = static_cast<T>(dis());

    fmt::print("Sorting ~{}x{} [{:7.1},{:6.1}]",
               sub_size,
               tot_elems / sub_size,
               double(min),
               double(max));
    for (int j = 0; j < 5; ++j) {
        std::vector<int> seq(order.size());
        std::vector<int> seq0(order.size());
        std::vector<int> seq1(order.size());
        std::vector<int> seq2(order.size());
        std::vector<int> seq3(order.size());
        std::vector<int> seq4(order.size());
        for (size_t i = 0; i < order.size(); ++i) {
            seq[i]  = static_cast<int>(i);
            seq0[i] = static_cast<int>(i);
            seq1[i] = static_cast<int>(i);
            seq2[i] = static_cast<int>(i);
            seq3[i] = static_cast<int>(i);
            seq4[i] = static_cast<int>(i);
        }

        auto k = [&](int i) { return order[i]; };

        auto idis    = [&] { return sub_size - sub_size / 8 + my_rand() % (sub_size / 4); };
        auto offsets = std::vector<int>();
        for (size_t offset = 0; offset < seq.size(); offset += idis())
            offsets.push_back(offset);
        if (offsets.back() != static_cast<int>(seq.size()))
            offsets.push_back(seq.size());

        auto t0 = std::chrono::high_resolution_clock::now();
        sort_sequence([&](cft::Span<int*> c) { sorter.net_sort(c, k); }, seq, offsets);
        auto t1 = std::chrono::high_resolution_clock::now();
        sort_sequence([&](cft::Span<int*> c) { sorter.radix_sort(c, k); }, seq0, offsets);
        auto t2 = std::chrono::high_resolution_clock::now();
        sort_sequence([&](cft::Span<int*> c) { sorter.sort(c, k); }, seq1, offsets);
        auto t3 = std::chrono::high_resolution_clock::now();
        sort_sequence([&](cft::Span<int*> c) { sorter.nth_element(c, size(c) / 2, k); },
                      seq2,
                      offsets);
        auto t4 = std::chrono::high_resolution_clock::now();
        sort_sequence(
            [&](cft::Span<int*> c) {
                std::sort(c.begin(), c.end(), [&](int a, int b) { return order[a] < order[b]; });
            },
            seq3,
            offsets);
        auto t5 = std::chrono::high_resolution_clock::now();
        sort_sequence(
            [&](cft::Span<int*> c) {
                std::nth_element(c.begin(), c.begin() + c.size() / 2, c.end(), [&](int a, int b) {
                    return order[a] < order[b];
                });
            },
            seq4,
            offsets);
        auto t6 = std::chrono::high_resolution_clock::now();

        duration_sec += std::chrono::duration<double>(t1 - t0).count();
        duration0_sec += std::chrono::duration<double>(t2 - t1).count();
        duration1_sec += std::chrono::duration<double>(t3 - t2).count();
        duration2_sec += std::chrono::duration<double>(t4 - t3).count();
        duration3_sec += std::chrono::duration<double>(t5 - t4).count();
        duration4_sec += std::chrono::duration<double>(t6 - t5).count();
    }

    fmt::print(" {:8} net sort {:8.2}, rdx sort {:8.2}, my sort {:8.2}, my nth {:8.2}, std sort "
               "{:8.2}, std nth {:8.2}\n",
               name,
               sub_size * duration_sec / tot_elems,
               sub_size * duration0_sec / tot_elems,
               sub_size * duration1_sec / tot_elems,
               sub_size * duration2_sec / tot_elems,
               sub_size * duration3_sec / tot_elems,
               sub_size * duration4_sec / tot_elems);
}

#ifndef NDEBUG
#define TOT_ELEMS 10000
#else
#define TOT_ELEMS 10000000
#endif
int main(int argc, char const** argv) {
    auto args = cft::make_span(argv, argc);

    auto sorter   = cft::make_sorter();
    int  sub_size = std::stoi(args[1]);

    // run_test<int8_t>("int8_t", sorter, sub_size, TOT_ELEMS);
    run_test<int16_t>("int16_t", sorter, sub_size, TOT_ELEMS);
    run_test<int32_t>("int32_t", sorter, sub_size, TOT_ELEMS);
    run_test<int64_t>("int64_t", sorter, sub_size, TOT_ELEMS);

    // run_test<uint8_t>("uint8_t", sorter, sub_size, TOT_ELEMS);
    // run_test<uint16_t>("uint16_t", sorter, sub_size, TOT_ELEMS);
    // run_test<uint32_t>("uint32_t", sorter, sub_size, TOT_ELEMS);
    // run_test<uint64_t>("uint64_t", sorter, sub_size, TOT_ELEMS);

    run_test<float>("float", sorter, sub_size, TOT_ELEMS);
    run_test<double>("double", sorter, sub_size, TOT_ELEMS);

    run_test_fat<int32_t, 8>("i32_8b", sorter, sub_size, TOT_ELEMS);
    run_test_fat<int32_t, 16>("i32_16b", sorter, sub_size, TOT_ELEMS);
    run_test_fat<int32_t, 32>("i32_32b", sorter, sub_size, TOT_ELEMS);
    run_test_fat<int32_t, 64>("i32_64b", sorter, sub_size, TOT_ELEMS);
    // run_test_fat<int32_t, 128>("i32_128b", sorter, sub_size, TOT_ELEMS);
    // run_test_fat<int32_t, 256>("i32_256b", sorter, sub_size, TOT_ELEMS);

    run_test_fat<double, 16>("dbl_16b", sorter, sub_size, TOT_ELEMS);
    run_test_fat<double, 32>("dbl_32b", sorter, sub_size, TOT_ELEMS);
    run_test_fat<double, 64>("dbl_64b", sorter, sub_size, TOT_ELEMS);
    // run_test_fat<double, 128>("dbl_128b", sorter, sub_size, TOT_ELEMS);
    // run_test_fat<double, 256>("dbl_256b", sorter, sub_size, TOT_ELEMS);

    run_test_indirect<int32_t>("i32_ind", sorter, sub_size, TOT_ELEMS);
    run_test_indirect<int64_t>("i64_ind", sorter, sub_size, TOT_ELEMS);
    run_test_indirect<float>("flt_ind", sorter, sub_size, TOT_ELEMS);
    run_test_indirect<double>("dbl_ind", sorter, sub_size, TOT_ELEMS);

    return EXIT_SUCCESS;
}
