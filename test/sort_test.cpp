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
    auto sorter = cav::Sorter<>();
    for (size_t i = 0; i < 100; ++i) {
        for (size_t sz = 2; sz <= 10000; sz = sz * 17 / 3) {
            auto subseq = make_span(arr.data(), sz);

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
    auto sorter = cav::Sorter<>();
    for (size_t i = 0; i < 100; ++i) {
        for (size_t sz = 2; sz <= 10000; sz = sz * 17 / 3) {
            auto subseq = make_span(arr.data(), sz);

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
    auto sorter = cav::Sorter<>();
    for (size_t i = 0; i < 100; ++i) {
        for (size_t sz = 2; sz <= 10000; sz = sz * 17 / 3) {
            auto subseq = make_span(arr.data(), sz);

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
    auto sorter = cav::Sorter<>();
    for (size_t i = 0; i < 100; ++i) {
        for (size_t sz = 2; sz <= 10000; sz = sz * 17 / 3) {
            auto subseq = make_span(arr.data(), sz);

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
    auto sorter = cav::Sorter<>();
    for (size_t i = 0; i < 100; ++i) {
        for (size_t sz = 2; sz <= 10000; sz = sz * 17 / 3) {
            auto subseq = make_span(arr.data(), sz);

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
    auto sorter = cav::Sorter<>();
    for (size_t i = 0; i < 100; ++i) {
        for (size_t sz = 2; sz <= 10000; sz = sz * 17 / 3) {
            auto subseq = make_span(arr.data(), sz);

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

TEST_CASE("nth_element ClassType int") {
    auto arr    = std::vector<ClassType<int>>(10000);
    auto sorter = cav::Sorter<>();
    for (size_t i = 0; i < 100; ++i) {
        for (size_t sz = 2; sz <= 10000; sz = sz * 17 / 3) {
            auto subseq = make_span(arr.data(), sz);

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

TEST_CASE("nth_element ClassType double") {
    auto arr    = std::vector<ClassType<double>>(10000);
    auto sorter = cav::Sorter<>();
    for (size_t i = 0; i < 100; ++i) {
        for (size_t sz = 2; sz <= 10000; sz = sz * 17 / 3) {
            auto subseq = make_span(arr.data(), sz);

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

TEST_CASE("sort algo specific int") {
    auto arr1   = std::vector<int>(1000);
    auto arr2   = std::vector<int>(1000);
    auto arr3   = std::vector<int>(1000);
    auto arr4   = std::vector<int>(1000);
    auto sorter = cav::Sorter<>();
    for (size_t i = 0; i < 10; ++i) {
        for (size_t sz = 2; sz <= 1000; sz = sz * 17 / 3) {
            auto subseq1 = make_span(arr1.data(), sz);
            auto subseq2 = make_span(arr2.data(), sz);
            auto subseq3 = make_span(arr3.data(), sz);
            auto subseq4 = make_span(arr4.data(), sz);

            for (int j = 0; j < sz; ++j) {
                auto elem  = rand() % 1024;
                subseq1[j] = elem;
                subseq2[j] = elem;
                subseq3[j] = elem;
                subseq4[j] = elem;
            }

            REQUIRE_NOTHROW(sorter.radix_sort_lsd(subseq1));
            REQUIRE_NOTHROW(sorter.radix_sort_msd(subseq2));
            REQUIRE_NOTHROW(sorter.net_sort(subseq3));
            REQUIRE_NOTHROW(sorter.insertion_sort(subseq4));
            CHECK(is_sorted(subseq1));
            CHECK(is_sorted(subseq2));
            CHECK(is_sorted(subseq3));
            CHECK(is_sorted(subseq4));

            for (int j = 0; j < sz; ++j) {
                auto elem  = rand() % 1024;
                subseq1[j] = elem;
                subseq2[j] = elem;
                subseq3[j] = elem;
                subseq4[j] = elem;
            }

            REQUIRE_NOTHROW(sorter.radix_sort_lsd(subseq1, [](int x) { return -x; }));
            REQUIRE_NOTHROW(sorter.radix_sort_msd(subseq2, [](int x) { return -x; }));
            REQUIRE_NOTHROW(sorter.net_sort(subseq3, [](int x) { return -x; }));
            REQUIRE_NOTHROW(sorter.insertion_sort(subseq4, [](int x) { return -x; }));
            CHECK(is_sorted(subseq1, [](int x) { return -x; }));
            CHECK(is_sorted(subseq2, [](int x) { return -x; }));
            CHECK(is_sorted(subseq3, [](int x) { return -x; }));
            CHECK(is_sorted(subseq4, [](int x) { return -x; }));
        }
    }
}

TEST_CASE("sort algo specific double") {
    auto arr1   = std::vector<double>(1000);
    auto arr2   = std::vector<double>(1000);
    auto arr3   = std::vector<double>(1000);
    auto arr4   = std::vector<double>(1000);
    auto sorter = cav::Sorter<>();
    for (size_t i = 0; i < 10; ++i) {
        for (size_t sz = 2; sz <= 1000; sz = sz * 17 / 3) {
            auto subseq1 = make_span(arr1.data(), sz);
            auto subseq2 = make_span(arr2.data(), sz);
            auto subseq3 = make_span(arr3.data(), sz);
            auto subseq4 = make_span(arr4.data(), sz);

            for (int j = 0; j < sz; ++j) {
                auto elem  = rand() / 1024.0;
                subseq1[j] = elem;
                subseq2[j] = elem;
                subseq3[j] = elem;
                subseq4[j] = elem;
            }

            REQUIRE_NOTHROW(sorter.radix_sort_lsd(subseq1));
            REQUIRE_NOTHROW(sorter.radix_sort_msd(subseq2));
            REQUIRE_NOTHROW(sorter.net_sort(subseq3));
            REQUIRE_NOTHROW(sorter.insertion_sort(subseq4));
            CHECK(is_sorted(subseq1));
            CHECK(is_sorted(subseq2));
            CHECK(is_sorted(subseq3));
            CHECK(is_sorted(subseq4));

            for (int j = 0; j < sz; ++j) {
                auto elem  = rand() / 1024.0;
                subseq1[j] = elem;
                subseq2[j] = elem;
                subseq3[j] = elem;
                subseq4[j] = elem;
            }

            REQUIRE_NOTHROW(sorter.radix_sort_lsd(subseq1, [](double x) { return -x; }));
            REQUIRE_NOTHROW(sorter.radix_sort_msd(subseq2, [](double x) { return -x; }));
            REQUIRE_NOTHROW(sorter.net_sort(subseq3, [](double x) { return -x; }));
            REQUIRE_NOTHROW(sorter.insertion_sort(subseq4, [](double x) { return -x; }));
            CHECK(is_sorted(subseq1, [](double x) { return -x; }));
            CHECK(is_sorted(subseq2, [](double x) { return -x; }));
            CHECK(is_sorted(subseq3, [](double x) { return -x; }));
            CHECK(is_sorted(subseq4, [](double x) { return -x; }));
        }
    }
}

TEST_CASE("sort algo specific ClassType<int>") {
    auto arr1   = std::vector<ClassType<int>>(1000);
    auto arr2   = std::vector<ClassType<int>>(1000);
    auto arr3   = std::vector<ClassType<int>>(1000);
    auto arr4   = std::vector<ClassType<int>>(1000);
    auto sorter = cav::Sorter<>();
    for (size_t i = 0; i < 10; ++i) {
        for (size_t sz = 2; sz <= 1000; sz = sz * 17 / 3) {
            auto subseq1 = make_span(arr1.data(), sz);
            auto subseq2 = make_span(arr2.data(), sz);
            auto subseq3 = make_span(arr3.data(), sz);
            auto subseq4 = make_span(arr4.data(), sz);

            for (int j = 0; j < sz; ++j) {
                auto elem  = ClassType<int>(rand() % 1024);
                subseq1[j] = elem;
                subseq2[j] = elem;
                subseq3[j] = elem;
                subseq4[j] = elem;
            }

            REQUIRE_NOTHROW(
                sorter.radix_sort_lsd(subseq1, [](ClassType<int> x) { return int(x); }));
            REQUIRE_NOTHROW(
                sorter.radix_sort_msd(subseq2, [](ClassType<int> x) { return int(x); }));
            REQUIRE_NOTHROW(sorter.net_sort(subseq3, [](ClassType<int> x) { return int(x); }));
            REQUIRE_NOTHROW(
                sorter.insertion_sort(subseq4, [](ClassType<int> x) { return int(x); }));
            CHECK(is_sorted(subseq1));
            CHECK(is_sorted(subseq2));
            CHECK(is_sorted(subseq3));
            CHECK(is_sorted(subseq4));

            for (int j = 0; j < sz; ++j) {
                auto elem  = ClassType<int>(rand() % 1024);
                subseq1[j] = elem;
                subseq2[j] = elem;
                subseq3[j] = elem;
                subseq4[j] = elem;
            }

            REQUIRE_NOTHROW(
                sorter.radix_sort_lsd(subseq1, [](ClassType<int> x) { return int(-x); }));
            REQUIRE_NOTHROW(
                sorter.radix_sort_msd(subseq2, [](ClassType<int> x) { return int(-x); }));
            REQUIRE_NOTHROW(sorter.net_sort(subseq3, [](ClassType<int> x) { return int(-x); }));
            REQUIRE_NOTHROW(
                sorter.insertion_sort(subseq4, [](ClassType<int> x) { return int(-x); }));
            CHECK(is_sorted(subseq1, [](ClassType<int> x) { return int(-x); }));
            CHECK(is_sorted(subseq2, [](ClassType<int> x) { return int(-x); }));
            CHECK(is_sorted(subseq3, [](ClassType<int> x) { return int(-x); }));
            CHECK(is_sorted(subseq4, [](ClassType<int> x) { return int(-x); }));
        }
    }
}

TEST_CASE("sort algo specific ClassType<double>") {
    auto arr1   = std::vector<ClassType<double>>(1000);
    auto arr2   = std::vector<ClassType<double>>(1000);
    auto arr3   = std::vector<ClassType<double>>(1000);
    auto arr4   = std::vector<ClassType<double>>(1000);
    auto sorter = cav::Sorter<>();
    for (size_t i = 0; i < 10; ++i) {
        for (size_t sz = 2; sz <= 1000; sz = sz * 17 / 3) {
            auto subseq1 = make_span(arr1.data(), sz);
            auto subseq2 = make_span(arr2.data(), sz);
            auto subseq3 = make_span(arr3.data(), sz);
            auto subseq4 = make_span(arr4.data(), sz);

            for (int j = 0; j < sz; ++j) {
                auto elem  = ClassType<double>(rand() / 1024.0);
                subseq1[j] = elem;
                subseq2[j] = elem;
                subseq3[j] = elem;
                subseq4[j] = elem;
            }

            REQUIRE_NOTHROW(
                sorter.radix_sort_lsd(subseq1, [](ClassType<double> x) { return double(x); }));
            REQUIRE_NOTHROW(
                sorter.radix_sort_msd(subseq2, [](ClassType<double> x) { return double(x); }));
            REQUIRE_NOTHROW(
                sorter.net_sort(subseq3, [](ClassType<double> x) { return double(x); }));
            REQUIRE_NOTHROW(
                sorter.insertion_sort(subseq4, [](ClassType<double> x) { return double(x); }));
            CHECK(is_sorted(subseq1));
            CHECK(is_sorted(subseq2));
            CHECK(is_sorted(subseq3));
            CHECK(is_sorted(subseq4));

            for (int j = 0; j < sz; ++j) {
                auto elem  = ClassType<double>(rand() / 1024.0);
                subseq1[j] = elem;
                subseq2[j] = elem;
                subseq3[j] = elem;
                subseq4[j] = elem;
            }

            REQUIRE_NOTHROW(
                sorter.radix_sort_lsd(subseq1, [](ClassType<double> x) { return double(-x); }));
            REQUIRE_NOTHROW(
                sorter.radix_sort_msd(subseq2, [](ClassType<double> x) { return double(-x); }));
            REQUIRE_NOTHROW(
                sorter.net_sort(subseq3, [](ClassType<double> x) { return double(-x); }));
            REQUIRE_NOTHROW(
                sorter.insertion_sort(subseq4, [](ClassType<double> x) { return double(-x); }));
            CHECK(is_sorted(subseq1, [](ClassType<double> x) { return double(-x); }));
            CHECK(is_sorted(subseq2, [](ClassType<double> x) { return double(-x); }));
            CHECK(is_sorted(subseq3, [](ClassType<double> x) { return double(-x); }));
            CHECK(is_sorted(subseq4, [](ClassType<double> x) { return double(-x); }));
        }
    }
}

TEST_CASE("nth_element algo specific int") {
    auto arr1   = std::vector<int>(1000);
    auto arr2   = std::vector<int>(1000);
    auto arr3   = std::vector<int>(1000);
    auto arr4   = std::vector<int>(1000);
    auto sorter = cav::Sorter<>();
    for (size_t i = 0; i < 10; ++i) {
        for (size_t sz = 2; sz <= 1000; sz = sz * 17 / 3) {
            auto subseq1 = make_span(arr1.data(), sz);
            auto subseq2 = make_span(arr2.data(), sz);

            for (int j = 0; j < sz; ++j) {
                auto elem  = rand() % 1024;
                subseq1[j] = elem;
                subseq2[j] = elem;
            }

            REQUIRE_NOTHROW(sorter.radix_nth_elem(subseq1, cav::size(subseq1) / 2));
            REQUIRE_NOTHROW(sorter.dutch_nth_elem(subseq2, cav::size(subseq2) / 2));
            CHECK(is_nth_elem(subseq1, cav::size(subseq1) / 2));
            CHECK(is_nth_elem(subseq2, cav::size(subseq2) / 2));

            for (int j = 0; j < sz; ++j) {
                auto elem  = rand() % 1024;
                subseq1[j] = elem;
                subseq2[j] = elem;
            }

            REQUIRE_NOTHROW(
                sorter.radix_nth_elem(subseq1, cav::size(subseq1) / 2, [](int x) { return -x; }));
            REQUIRE_NOTHROW(
                sorter.dutch_nth_elem(subseq2, cav::size(subseq2) / 2, [](int x) { return -x; }));
            CHECK(is_nth_elem(subseq1, cav::size(subseq1) / 2, [](int x) { return -x; }));
            CHECK(is_nth_elem(subseq2, cav::size(subseq2) / 2, [](int x) { return -x; }));
        }
    }
}

TEST_CASE("nth_element algo specific double") {
    auto arr1   = std::vector<double>(1000);
    auto arr2   = std::vector<double>(1000);
    auto arr3   = std::vector<double>(1000);
    auto arr4   = std::vector<double>(1000);
    auto sorter = cav::Sorter<>();
    for (size_t i = 0; i < 10; ++i) {
        for (size_t sz = 2; sz <= 1000; sz = sz * 17 / 3) {
            auto subseq1 = make_span(arr1.data(), sz);
            auto subseq2 = make_span(arr2.data(), sz);

            for (int j = 0; j < sz; ++j) {
                auto elem  = rand() / 1024.0;
                subseq1[j] = elem;
                subseq2[j] = elem;
            }

            REQUIRE_NOTHROW(sorter.radix_nth_elem(subseq1, cav::size(subseq1) / 2));
            REQUIRE_NOTHROW(sorter.dutch_nth_elem(subseq2, cav::size(subseq2) / 2));
            CHECK(is_nth_elem(subseq1, cav::size(subseq1) / 2));
            CHECK(is_nth_elem(subseq2, cav::size(subseq2) / 2));

            for (int j = 0; j < sz; ++j) {
                auto elem  = rand() / 1024.0;
                subseq1[j] = elem;
                subseq2[j] = elem;
            }

            REQUIRE_NOTHROW(sorter.radix_nth_elem(subseq1, cav::size(subseq1) / 2, [](double x) {
                return -x;
            }));
            REQUIRE_NOTHROW(sorter.dutch_nth_elem(subseq2, cav::size(subseq2) / 2, [](double x) {
                return -x;
            }));
            CHECK(is_nth_elem(subseq1, cav::size(subseq1) / 2, [](double x) { return -x; }));
            CHECK(is_nth_elem(subseq2, cav::size(subseq2) / 2, [](double x) { return -x; }));
        }
    }
}

TEST_CASE("nth_element algo specific ClassType<int>") {
    auto arr1   = std::vector<ClassType<int>>(1000);
    auto arr2   = std::vector<ClassType<int>>(1000);
    auto arr3   = std::vector<ClassType<int>>(1000);
    auto arr4   = std::vector<ClassType<int>>(1000);
    auto sorter = cav::Sorter<>();
    for (size_t i = 0; i < 10; ++i) {
        for (size_t sz = 2; sz <= 1000; sz = sz * 17 / 3) {
            auto subseq1 = make_span(arr1.data(), sz);
            auto subseq2 = make_span(arr2.data(), sz);

            for (int j = 0; j < sz; ++j) {
                auto elem  = ClassType<int>(rand() % 1024);
                subseq1[j] = elem;
                subseq2[j] = elem;
            }

            REQUIRE_NOTHROW(sorter.radix_nth_elem(subseq1,
                                                  cav::size(subseq1) / 2,
                                                  [](ClassType<int> x) { return int(x); }));
            REQUIRE_NOTHROW(sorter.dutch_nth_elem(subseq2,
                                                  cav::size(subseq2) / 2,
                                                  [](ClassType<int> x) { return int(x); }));
            CHECK(is_nth_elem(subseq1, cav::size(subseq1) / 2));
            CHECK(is_nth_elem(subseq2, cav::size(subseq2) / 2));

            for (int j = 0; j < sz; ++j) {
                auto elem  = ClassType<int>(rand() % 1024);
                subseq1[j] = elem;
                subseq2[j] = elem;
            }

            REQUIRE_NOTHROW(sorter.radix_nth_elem(subseq1,
                                                  cav::size(subseq1) / 2,
                                                  [](ClassType<int> x) { return int(-x); }));
            REQUIRE_NOTHROW(sorter.dutch_nth_elem(subseq2,
                                                  cav::size(subseq2) / 2,
                                                  [](ClassType<int> x) { return int(-x); }));
            CHECK(is_nth_elem(subseq1, cav::size(subseq1) / 2, [](ClassType<int> x) {
                return int(-x);
            }));
            CHECK(is_nth_elem(subseq2, cav::size(subseq2) / 2, [](ClassType<int> x) {
                return int(-x);
            }));
        }
    }
}

TEST_CASE("nth_element algo specific ClassType<double>") {
    auto arr1   = std::vector<ClassType<double>>(1000);
    auto arr2   = std::vector<ClassType<double>>(1000);
    auto arr3   = std::vector<ClassType<double>>(1000);
    auto arr4   = std::vector<ClassType<double>>(1000);
    auto sorter = cav::Sorter<>();
    for (size_t i = 0; i < 10; ++i) {
        for (size_t sz = 2; sz <= 1000; sz = sz * 17 / 3) {
            auto subseq1 = make_span(arr1.data(), sz);
            auto subseq2 = make_span(arr2.data(), sz);

            for (int j = 0; j < sz; ++j) {
                auto elem  = ClassType<double>(rand() / 1024.0);
                subseq1[j] = elem;
                subseq2[j] = elem;
            }

            REQUIRE_NOTHROW(sorter.radix_nth_elem(subseq1,
                                                  cav::size(subseq1) / 2,
                                                  [](ClassType<double> x) { return double(x); }));
            REQUIRE_NOTHROW(sorter.dutch_nth_elem(subseq2,
                                                  cav::size(subseq2) / 2,
                                                  [](ClassType<double> x) { return double(x); }));
            CHECK(is_nth_elem(subseq1, cav::size(subseq1) / 2));
            CHECK(is_nth_elem(subseq2, cav::size(subseq2) / 2));

            for (int j = 0; j < sz; ++j) {
                auto elem  = ClassType<double>(rand() / 1024.0);
                subseq1[j] = elem;
                subseq2[j] = elem;
            }

            REQUIRE_NOTHROW(sorter.radix_nth_elem(subseq1,
                                                  cav::size(subseq1) / 2,
                                                  [](ClassType<double> x) { return double(-x); }));
            REQUIRE_NOTHROW(sorter.dutch_nth_elem(subseq2,
                                                  cav::size(subseq2) / 2,
                                                  [](ClassType<double> x) { return double(-x); }));
            CHECK(is_nth_elem(subseq1, cav::size(subseq1) / 2, [](ClassType<double> x) {
                return double(-x);
            }));
            CHECK(is_nth_elem(subseq2, cav::size(subseq2) / 2, [](ClassType<double> x) {
                return double(-x);
            }));
        }
    }
}

}  // namespace cav