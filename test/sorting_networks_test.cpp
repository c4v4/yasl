// SPDX-FileCopyrightText: 2024 Francesco Cavaliere <francescocava95@gmail.com>
// SPDX-License-Identifier: MIT


#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#define DOCTEST_CONFIG_SUPER_FAST_ASSERTS

#include <doctest/doctest.h>

#define CAV_MAX_NET_SIZE 32U
#include "Span.hpp"
#include "sorting_networks.hpp"
#include "../src/ClassType.hpp"

namespace cav {

TEST_CASE("Sorting networks int") {
    for (size_t i = 0; i < 1000; ++i) {
        int arr[32] = {};
        for (size_t s = 2; s <= 32; ++s) {
            auto subseq = make_span(arr, s);

            for (int& elem : subseq)
                elem = rand() % 1024;
            REQUIRE_NOTHROW(cav::net_dispatch(subseq));
            CHECK(is_sorted(subseq));

            for (int& elem : subseq)
                elem = rand() % 1024;
            REQUIRE_NOTHROW(cav::net_dispatch(subseq, [](int x) { return -x; }));
            CHECK(is_sorted(subseq, [](int x) { return -x; }));
        }
    }
}

TEST_CASE("Sorting networks double") {
    for (size_t i = 0; i < 1000; ++i) {
        double arr[32] = {};
        for (size_t s = 2; s <= 32; ++s) {
            auto subseq = make_span(arr, s);

            for (double& elem : subseq)
                elem = rand() / 1024.0;
            REQUIRE_NOTHROW(cav::net_dispatch(subseq));
            CHECK(is_sorted(subseq));

            for (double& elem : subseq)
                elem = rand() / 1024.0;
            REQUIRE_NOTHROW(cav::net_dispatch(subseq, [](double x) { return -x; }));
            CHECK(is_sorted(subseq, [](double x) { return -x; }));
        }
    }
}

TEST_CASE("Sorting networks int") {
    for (size_t i = 0; i < 1000; ++i) {
        ClassType<int> arr[32] = {};
        for (size_t s = 2; s <= 32; ++s) {
            auto subseq = make_span(arr, s);

            for (ClassType<int>& elem : subseq)
                elem = ClassType<int>(rand() % 1024);
            REQUIRE_NOTHROW(cav::net_dispatch(subseq));
            CHECK(is_sorted(subseq));

            for (ClassType<int>& elem : subseq)
                elem = ClassType<int>(rand() % 1024);
            REQUIRE_NOTHROW(cav::net_dispatch(subseq, [](ClassType<int> x) { return -x; }));
            CHECK(is_sorted(subseq, [](ClassType<int> x) { return -x; }));
        }
    }
}

TEST_CASE("Sorting networks double") {
    for (size_t i = 0; i < 1000; ++i) {
        ClassType<double> arr[32] = {};
        for (size_t s = 2; s <= 32; ++s) {
            auto subseq = make_span(arr, s);

            for (ClassType<double>& elem : subseq)
                elem = ClassType<double>(rand() / 1024.0);
            REQUIRE_NOTHROW(cav::net_dispatch(subseq));
            CHECK(is_sorted(subseq));

            for (ClassType<double>& elem : subseq)
                elem = ClassType<double>(rand() / 1024.0);
            REQUIRE_NOTHROW(cav::net_dispatch(subseq, [](ClassType<double> x) { return -x; }));
            CHECK(is_sorted(subseq, [](ClassType<double> x) { return -x; }));
        }
    }
}

}  // namespace cav