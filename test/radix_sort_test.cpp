// SPDX-FileCopyrightText: 2024 Francesco Cavaliere <francescocava95@gmail.com>
// SPDX-License-Identifier: MIT


#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#define DOCTEST_CONFIG_SUPER_FAST_ASSERTS

#include "radix_sort.hpp"

#include <doctest/doctest.h>

#include "Span.hpp"
#include "../src/ClassType.hpp"

namespace cav {

TEST_CASE("radix_sort_lsd int") {
    auto arr  = std::vector<int>(10000);
    auto buff = std::vector<int>(10000);
    for (size_t i = 0; i < 100; ++i) {
        for (size_t s = 2; s <= 10000; s = s * 17 / 3) {
            auto subseq = make_span(arr.data(), s);

            for (int& elem : subseq)
                elem = rand() % 1024;
            REQUIRE_NOTHROW(radix_sort_lsd<int>(subseq, buff));
            CHECK(is_sorted(subseq));

            for (int& elem : subseq)
                elem = rand() % 1024;
            REQUIRE_NOTHROW(radix_sort_lsd<int>(subseq, buff, [](int x) { return -x; }));
            CHECK(is_sorted(subseq, [](int x) { return -x; }));
        }
    }
}

TEST_CASE("radix_sort_lsd double") {
    auto arr  = std::vector<double>(10000);
    auto buff = std::vector<double>(10000);
    for (size_t i = 0; i < 100; ++i) {
        for (size_t s = 2; s <= 10000; s = s * 17 / 3) {
            auto subseq = make_span(arr.data(), s);

            for (double& elem : subseq)
                elem = rand() / 1024.0;
            REQUIRE_NOTHROW(radix_sort_lsd<int>(subseq, buff));
            CHECK(is_sorted(subseq));

            for (double& elem : subseq)
                elem = rand() / 1024.0;
            REQUIRE_NOTHROW(radix_sort_lsd<int>(subseq, buff, [](double x) { return -x; }));
            CHECK(is_sorted(subseq, [](double x) { return -x; }));
        }
    }
}

TEST_CASE("radix_sort_lsd int") {
    auto arr  = std::vector<ClassType<int>>(10000);
    auto buff = std::vector<ClassType<int>>(10000);
    for (size_t i = 0; i < 100; ++i) {
        for (size_t s = 2; s <= 10000; s = s * 17 / 3) {
            auto subseq = make_span(arr.data(), s);

            for (ClassType<int>& elem : subseq)
                elem = ClassType<int>(rand() % 1024);
            REQUIRE_NOTHROW(
                radix_sort_lsd<int>(subseq, buff, [](ClassType<int> x) { return int(x); }));
            CHECK(is_sorted(subseq));

            for (ClassType<int>& elem : subseq)
                elem = ClassType<int>(rand() % 1024);
            REQUIRE_NOTHROW(
                radix_sort_lsd<int>(subseq, buff, [](ClassType<int> x) { return int(-x); }));
            CHECK(is_sorted(subseq, [](ClassType<int> x) { return int(-x); }));
        }
    }
}

TEST_CASE("radix_sort_lsd double") {
    auto arr  = std::vector<ClassType<double>>(10000);
    auto buff = std::vector<ClassType<double>>(10000);
    for (size_t i = 0; i < 100; ++i) {
        for (size_t s = 2; s <= 10000; s = s * 17 / 3) {
            auto subseq = make_span(arr.data(), s);

            for (ClassType<double>& elem : subseq)
                elem = ClassType<double>(rand() / 1024.0);
            REQUIRE_NOTHROW(
                radix_sort_lsd<int>(subseq, buff, [](ClassType<double> x) { return double(x); }));
            CHECK(is_sorted(subseq));

            for (ClassType<double>& elem : subseq)
                elem = ClassType<double>(rand() / 1024.0);
            REQUIRE_NOTHROW(
                radix_sort_lsd<int>(subseq, buff, [](ClassType<double> x) { return double(-x); }));
            CHECK(is_sorted(subseq, [](ClassType<double> x) { return double(-x); }));
        }
    }
}

TEST_CASE("radix_sort_msd int") {
    auto arr  = std::vector<int>(10000);
    auto buff = std::vector<int>(10000);
    for (size_t i = 0; i < 100; ++i) {
        for (size_t s = 2; s <= 10000; s = s * 17 / 3) {
            auto subseq = make_span(arr.data(), s);

            for (int& elem : subseq)
                elem = rand() % 1024;
            REQUIRE_NOTHROW(radix_sort_msd<int>(subseq, buff));
            CHECK(is_sorted(subseq));

            for (int& elem : subseq)
                elem = rand() % 1024;
            REQUIRE_NOTHROW(radix_sort_msd<int>(subseq, buff, [](int x) { return -x; }));
            CHECK(is_sorted(subseq, [](int x) { return -x; }));
        }
    }
}

TEST_CASE("radix_sort_msd double") {
    auto arr  = std::vector<double>(10000);
    auto buff = std::vector<double>(10000);
    for (size_t i = 0; i < 100; ++i) {
        for (size_t s = 2; s <= 10000; s = s * 17 / 3) {
            auto subseq = make_span(arr.data(), s);

            for (double& elem : subseq)
                elem = rand() / 1024.0;
            REQUIRE_NOTHROW(radix_sort_msd<int>(subseq, buff));
            CHECK(is_sorted(subseq));

            for (double& elem : subseq)
                elem = rand() / 1024.0;
            REQUIRE_NOTHROW(radix_sort_msd<int>(subseq, buff, [](double x) { return -x; }));
            CHECK(is_sorted(subseq, [](double x) { return -x; }));
        }
    }
}

TEST_CASE("radix_sort_msd int") {
    auto arr  = std::vector<ClassType<int>>(10000);
    auto buff = std::vector<ClassType<int>>(10000);
    for (size_t i = 0; i < 100; ++i) {
        for (size_t s = 2; s <= 10000; s = s * 17 / 3) {
            auto subseq = make_span(arr.data(), s);

            for (ClassType<int>& elem : subseq)
                elem = ClassType<int>(rand() % 1024);
            REQUIRE_NOTHROW(
                radix_sort_msd<int>(subseq, buff, [](ClassType<int> x) { return int(x); }));
            CHECK(is_sorted(subseq));

            for (ClassType<int>& elem : subseq)
                elem = ClassType<int>(rand() % 1024);
            REQUIRE_NOTHROW(
                radix_sort_msd<int>(subseq, buff, [](ClassType<int> x) { return int(-x); }));
            CHECK(is_sorted(subseq, [](ClassType<int> x) { return -x; }));
        }
    }
}

TEST_CASE("radix_sort_msd double") {
    auto arr  = std::vector<ClassType<double>>(10000);
    auto buff = std::vector<ClassType<double>>(10000);
    for (size_t i = 0; i < 100; ++i) {
        for (size_t s = 2; s <= 10000; s = s * 17 / 3) {
            auto subseq = make_span(arr.data(), s);

            for (ClassType<double>& elem : subseq)
                elem = ClassType<double>(rand() / 1024.0);
            REQUIRE_NOTHROW(
                radix_sort_msd<int>(subseq, buff, [](ClassType<double> x) { return double(x); }));
            CHECK(is_sorted(subseq));

            for (ClassType<double>& elem : subseq)
                elem = ClassType<double>(rand() / 1024.0);
            REQUIRE_NOTHROW(
                radix_sort_msd<int>(subseq, buff, [](ClassType<double> x) { return double(-x); }));
            CHECK(is_sorted(subseq, [](ClassType<double> x) { return -x; }));
        }
    }
}

}  // namespace cav