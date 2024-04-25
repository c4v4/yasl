// SPDX-FileCopyrightText: 2024 Francesco Cavaliere <francescocava95@gmail.com>
// SPDX-License-Identifier: MIT


#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#define DOCTEST_CONFIG_SUPER_FAST_ASSERTS

#include <doctest/doctest.h>

#include <algorithm>

#define CAV_MAX_NET_SIZE 32U
#include "Span.hpp"
#include "limits.hpp"
#include "sorting_networks.hpp"

namespace cav {

TEST_CASE("Sorting networks int") {
    for (size_t i = 0; i < 10000; ++i) {
        int arr[32] = {};
        for (size_t s = 2; s <= 32; ++s) {
            auto subseq = make_span(arr, s);

            for (int& elem : subseq)
                elem = rand() % 1024;
            REQUIRE_NOTHROW(cav::net_dispatch(subseq));
            CHECK(std::is_sorted(std::begin(subseq), std::end(subseq)));

            for (int& elem : subseq)
                elem = rand() % 1024;
            REQUIRE_NOTHROW(cav::net_dispatch(subseq, [](int x) { return -x; }));
            CHECK(std::is_sorted(std::begin(subseq), std::end(subseq), std::greater<int>{}));
        }
    }
}

TEST_CASE("Sorting networks double") {
    for (size_t i = 0; i < 10000; ++i) {
        double arr[32] = {};
        for (size_t s = 2; s <= 32; ++s) {
            auto subseq = make_span(arr, s);

            for (double& elem : subseq)
                elem = rand() / 1024.0;
            REQUIRE_NOTHROW(cav::net_dispatch(subseq));
            CHECK(std::is_sorted(std::begin(subseq), std::end(subseq)));

            for (double& elem : subseq)
                elem = rand() / 1024.0;
            REQUIRE_NOTHROW(cav::net_dispatch(subseq, [](double x) { return -x; }));
            CHECK(std::is_sorted(std::begin(subseq), std::end(subseq), std::greater<double>{}));
        }
    }
}

namespace local { namespace {
    template <typename T>
    class ClassType {
    private:
        T    elem;
        char padding[32 - sizeof(T)] = {};

    public:
        ClassType()
            : elem{limits<T>::max()} {
        }

        explicit ClassType(T elem)
            : elem(elem) {
        }

        ClassType(ClassType const& other)
            : elem(other.elem) {
        }

        ClassType(ClassType&& other) noexcept
            : elem(other.elem) {
        }

        ClassType& operator=(ClassType const& other) {
            if (this == &other)
                return *this;
            elem = other.elem;
            return *this;
        }

        ClassType& operator=(ClassType&& other) noexcept {
            elem = other.elem;
            return *this;
        }

        bool operator<(ClassType const& other) const noexcept {
            return elem < other.elem;
        }

        bool operator>(ClassType const& other) const noexcept {
            return elem > other.elem;
        }

        ClassType operator-() {
            return ClassType{-elem};
        }
    };
}  // namespace
}  // namespace local

TEST_CASE("Sorting networks int") {
    for (size_t i = 0; i < 10000; ++i) {
        local::ClassType<int> arr[32] = {};
        for (size_t s = 2; s <= 32; ++s) {
            auto subseq = make_span(arr, s);

            for (local::ClassType<int>& elem : subseq)
                elem = local::ClassType<int>(rand() % 1024);
            REQUIRE_NOTHROW(cav::net_dispatch(subseq));
            CHECK(std::is_sorted(std::begin(subseq), std::end(subseq)));

            for (local::ClassType<int>& elem : subseq)
                elem = local::ClassType<int>(rand() % 1024);
            REQUIRE_NOTHROW(cav::net_dispatch(subseq, [](local::ClassType<int> x) { return -x; }));
            CHECK(std::is_sorted(std::begin(subseq),
                                 std::end(subseq),
                                 std::greater<local::ClassType<int>>{}));
        }
    }
}

TEST_CASE("Sorting networks double") {
    for (size_t i = 0; i < 10000; ++i) {
        local::ClassType<double> arr[32] = {};
        for (size_t s = 2; s <= 32; ++s) {
            auto subseq = make_span(arr, s);

            for (local::ClassType<double>& elem : subseq)
                elem = local::ClassType<double>(rand() / 1024.0);
            REQUIRE_NOTHROW(cav::net_dispatch(subseq));
            CHECK(std::is_sorted(std::begin(subseq), std::end(subseq)));

            for (local::ClassType<double>& elem : subseq)
                elem = local::ClassType<double>(rand() / 1024.0);
            REQUIRE_NOTHROW(
                cav::net_dispatch(subseq, [](local::ClassType<double> x) { return -x; }));
            CHECK(std::is_sorted(std::begin(subseq),
                                 std::end(subseq),
                                 std::greater<local::ClassType<double>>{}));
        }
    }
}

}  // namespace cav