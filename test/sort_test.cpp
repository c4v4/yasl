// SPDX-FileCopyrightText: 2024 Francesco Cavaliere <francescocava95@gmail.com>
// SPDX-License-Identifier: MIT


#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#define DOCTEST_CONFIG_SUPER_FAST_ASSERTS

#include "sort.hpp"

#include <doctest/doctest.h>

#include "Span.hpp"
#include "tests_utils.hpp"

namespace cav {

TEST_CASE("sort int") {
    auto arr    = std::vector<int>(10000);
    auto buff   = std::vector<int>(10000);
    auto sorter = cav::Sorter<>();
    for (size_t i = 0; i < 100; ++i) {
        for (size_t s = 2; s <= 10000; s = s * 17 / 3) {
            auto subseq = make_span(arr.data(), s);

            for (int& elem : subseq)
                elem = rand() % 1024;
            REQUIRE_NOTHROW(sorter.sort(subseq));
            CHECK(is_sorted(subseq));

            for (int& elem : subseq)
                elem = rand() % 1024;
            REQUIRE_NOTHROW(sorter.sort(subseq, [](int x) { return -x; }));
            CHECK(is_sorted(subseq, [](int x) { return -x; }));
        }
    }
}

TEST_CASE("sort double") {
    auto arr    = std::vector<double>(10000);
    auto buff   = std::vector<double>(10000);
    auto sorter = cav::Sorter<>();
    for (size_t i = 0; i < 100; ++i) {
        for (size_t s = 2; s <= 10000; s = s * 17 / 3) {
            auto subseq = make_span(arr.data(), s);

            for (double& elem : subseq)
                elem = rand() / 1024.0;
            REQUIRE_NOTHROW(sorter.sort(subseq));
            CHECK(is_sorted(subseq));

            for (double& elem : subseq)
                elem = rand() / 1024.0;
            REQUIRE_NOTHROW(sorter.sort(subseq, [](double x) { return -x; }));
            CHECK(is_sorted(subseq, [](double x) { return -x; }));
        }
    }
}

TEST_CASE("sort ClassType int") {
    auto arr    = std::vector<ClassType<int>>(10000);
    auto buff   = std::vector<ClassType<int>>(10000);
    auto sorter = cav::Sorter<>();
    for (size_t i = 0; i < 100; ++i) {
        for (size_t s = 2; s <= 10000; s = s * 17 / 3) {
            auto subseq = make_span(arr.data(), s);

            for (ClassType<int>& elem : subseq)
                elem = ClassType<int>(rand() % 1024);
            REQUIRE_NOTHROW(sorter.sort(subseq, [](ClassType<int> x) { return int(x); }));
            CHECK(is_sorted(subseq));

            for (ClassType<int>& elem : subseq)
                elem = ClassType<int>(rand() % 1024);
            REQUIRE_NOTHROW(sorter.sort(subseq, [](ClassType<int> x) { return int(-x); }));
            CHECK(is_sorted(subseq, [](ClassType<int> x) { return int(-x); }));
        }
    }
}

TEST_CASE("sort ClassType double") {
    auto arr    = std::vector<ClassType<double>>(10000);
    auto buff   = std::vector<ClassType<double>>(10000);
    auto sorter = cav::Sorter<>();
    for (size_t i = 0; i < 100; ++i) {
        for (size_t s = 2; s <= 10000; s = s * 17 / 3) {
            auto subseq = make_span(arr.data(), s);

            for (ClassType<double>& elem : subseq)
                elem = ClassType<double>(rand() / 1024.0);
            REQUIRE_NOTHROW(sorter.sort(subseq, [](ClassType<double> x) { return double(x); }));
            CHECK(is_sorted(subseq));

            for (ClassType<double>& elem : subseq)
                elem = ClassType<double>(rand() / 1024.0);
            REQUIRE_NOTHROW(sorter.sort(subseq, [](ClassType<double> x) { return double(-x); }));
            CHECK(is_sorted(subseq, [](ClassType<double> x) { return double(-x); }));
        }
    }
}

TEST_CASE("nth_element int") {
    auto arr    = std::vector<int>(10000);
    auto buff   = std::vector<int>(10000);
    auto sorter = cav::Sorter<>();
    for (size_t i = 0; i < 100; ++i) {
        for (size_t s = 2; s <= 10000; s = s * 17 / 3) {
            auto subseq = make_span(arr.data(), s);

            for (int& elem : subseq)
                elem = rand() % 1024;
            REQUIRE_NOTHROW(sorter.nth_element(subseq, cav::size(subseq) / 2));
            CHECK(is_nth_elem(subseq, cav::size(subseq) / 2));

            for (int& elem : subseq)
                elem = rand() % 1024;
            REQUIRE_NOTHROW(
                sorter.nth_element(subseq, cav::size(subseq) / 2, [](int x) { return -x; }));
            CHECK(is_nth_elem(subseq, cav::size(subseq) / 2, [](int x) { return -x; }));
        }
    }
}

TEST_CASE("nth_element double") {
    auto arr    = std::vector<double>(10000);
    auto buff   = std::vector<double>(10000);
    auto sorter = cav::Sorter<>();
    for (size_t i = 0; i < 100; ++i) {
        for (size_t s = 2; s <= 10000; s = s * 17 / 3) {
            auto subseq = make_span(arr.data(), s);

            for (double& elem : subseq)
                elem = rand() / 1024.0;
            REQUIRE_NOTHROW(sorter.nth_element(subseq, cav::size(subseq) / 2));
            CHECK(is_nth_elem(subseq, cav::size(subseq) / 2));

            for (double& elem : subseq)
                elem = rand() / 1024.0;
            REQUIRE_NOTHROW(
                sorter.nth_element(subseq, cav::size(subseq) / 2, [](double x) { return -x; }));
            CHECK(is_nth_elem(subseq, cav::size(subseq) / 2, [](double x) { return -x; }));
        }
    }
}

TEST_CASE("nth_element nth_element int") {
    auto arr    = std::vector<ClassType<int>>(10000);
    auto buff   = std::vector<ClassType<int>>(10000);
    auto sorter = cav::Sorter<>();
    for (size_t i = 0; i < 100; ++i) {
        for (size_t s = 2; s <= 10000; s = s * 17 / 3) {
            auto subseq = make_span(arr.data(), s);

            for (ClassType<int>& elem : subseq)
                elem = ClassType<int>(rand() % 1024);
            REQUIRE_NOTHROW(sorter.nth_element(subseq, cav::size(subseq) / 2, [](ClassType<int> x) {
                return int(x);
            }));
            CHECK(is_nth_elem(subseq, cav::size(subseq) / 2));

            for (ClassType<int>& elem : subseq)
                elem = ClassType<int>(rand() % 1024);
            REQUIRE_NOTHROW(sorter.nth_element(subseq, cav::size(subseq) / 2, [](ClassType<int> x) {
                return int(-x);
            }));
            CHECK(is_nth_elem(subseq, cav::size(subseq) / 2, [](ClassType<int> x) {
                return int(-x);
            }));
        }
    }
}

TEST_CASE("nth_element nth_element double") {
    auto arr    = std::vector<ClassType<double>>(10000);
    auto buff   = std::vector<ClassType<double>>(10000);
    auto sorter = cav::Sorter<>();
    for (size_t i = 0; i < 100; ++i) {
        for (size_t s = 2; s <= 10000; s = s * 17 / 3) {
            auto subseq = make_span(arr.data(), s);

            for (ClassType<double>& elem : subseq)
                elem = ClassType<double>(rand() / 1024.0);
            REQUIRE_NOTHROW(sorter.nth_element(subseq,
                                               cav::size(subseq) / 2,
                                               [](ClassType<double> x) { return double(x); }));
            CHECK(is_nth_elem(subseq, cav::size(subseq) / 2));

            for (ClassType<double>& elem : subseq)
                elem = ClassType<double>(rand() / 1024.0);
            REQUIRE_NOTHROW(sorter.nth_element(subseq,
                                               cav::size(subseq) / 2,
                                               [](ClassType<double> x) { return double(-x); }));
            CHECK(is_nth_elem(subseq, cav::size(subseq) / 2, [](ClassType<double> x) {
                return double(-x);
            }));
        }
    }
}


}  // namespace cav