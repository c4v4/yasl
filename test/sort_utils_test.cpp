// SPDX-FileCopyrightText: 2024 Francesco Cavaliere <francescocava95@gmail.com>
// SPDX-License-Identifier: MIT


#include <cstdint>

#include "Span.hpp"
#include "tests_utils.hpp"
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#define DOCTEST_CONFIG_SUPER_FAST_ASSERTS

#include <doctest/doctest.h>

#include <vector>

#include "limits.hpp"
#include "sort_utils.hpp"

namespace cav {
TEST_CASE("to_uint") {
    SUBCASE("to_uint(unsigned integers)") {
        for (uint64_t k = 0; k < 1024; k = k * 37 / 11 + 17) {
            CHECK(cav::to_uint(static_cast<uint8_t>(k)) == static_cast<uint8_t>(k));
            CHECK(cav::to_uint(static_cast<uint16_t>(k)) == static_cast<uint16_t>(k));
            CHECK(cav::to_uint(static_cast<uint32_t>(k)) == static_cast<uint32_t>(k));
            CHECK(cav::to_uint(k) == k);
        }
    }


    SUBCASE("to_uint(signed integers)") {
        int8_t  prev8  = limits<int8_t>::min();
        int16_t prev16 = limits<int16_t>::min();
        int32_t prev32 = limits<int32_t>::min();
        int64_t prev64 = limits<int64_t>::min();
        for (uint64_t k = 0; k < 1024; ++k) {
            auto val64 = static_cast<int64_t>(static_cast<uint64_t>(rand() - RAND_MAX / 2));
            auto val32 = static_cast<int32_t>(static_cast<uint32_t>(val64));
            auto val16 = static_cast<int16_t>(static_cast<uint16_t>(val32));
            auto val8  = static_cast<int8_t>(static_cast<uint8_t>(val16));

            CHECK((val8 < prev8) == to_uint(val8) < to_uint(prev8));
            CHECK((val16 < prev16) == to_uint(val16) < to_uint(prev16));
            CHECK((val32 < prev32) == to_uint(val32) < to_uint(prev32));
            CHECK((val64 < prev64) == to_uint(val64) < to_uint(prev64));

            prev8  = val8;
            prev16 = val16;
            prev32 = val32;
            prev64 = val64;
        }
    }

    SUBCASE("to_uint(signed integers)") {
        float  prev32 = limits<float>::min();
        double prev64 = limits<double>::min();
        for (uint64_t k = 0; k < 1024; ++k) {
            auto val64 = static_cast<double>(rand() - RAND_MAX / 2) /
                         (1.0 + static_cast<double>(rand()));
            auto val32 = static_cast<float>(val64);

            CHECK((val32 < prev32) == to_uint(val32) < to_uint(prev32));
            CHECK((val64 < prev64) == to_uint(val64) < to_uint(prev64));

            prev32 = val32;
            prev64 = val64;
        }
    }
}

TEST_CASE("nth_byte") {
    SUBCASE("uint8_t") {
        uint32_t k = 0x12;
        CHECK(cav::nth_byte<0>(k) == 0x12);
    }

    SUBCASE("uint16_t") {
        uint32_t k = 0x1234;
        CHECK(cav::nth_byte<1>(k) == 0x12);
        CHECK(cav::nth_byte<0>(k) == 0x34);
    }

    SUBCASE("uint32_t") {
        uint32_t k = 0x12345678;
        CHECK(cav::nth_byte<3>(k) == 0x12);
        CHECK(cav::nth_byte<2>(k) == 0x34);
        CHECK(cav::nth_byte<1>(k) == 0x56);
        CHECK(cav::nth_byte<0>(k) == 0x78);
    }

    SUBCASE("uint32_t") {
        uint64_t k = 0x0123456789ABCDEF;
        CHECK(cav::nth_byte<7>(k) == 0x01);
        CHECK(cav::nth_byte<6>(k) == 0x23);
        CHECK(cav::nth_byte<5>(k) == 0x45);
        CHECK(cav::nth_byte<4>(k) == 0x67);
        CHECK(cav::nth_byte<3>(k) == 0x89);
        CHECK(cav::nth_byte<2>(k) == 0xAB);
        CHECK(cav::nth_byte<1>(k) == 0xCD);
        CHECK(cav::nth_byte<0>(k) == 0xEF);
    }
}

TEST_CASE("move_uninit") {
    SUBCASE("move_uninit(D&, S&) - trivially copiable types") {
        int src = 42;
        int dest;
        cav::move_uninit(dest, src);
        CHECK(dest == 42);
    }

    SUBCASE("move_uninit(D&, S&) - non trivially copiable types") {
        std::vector<int> src = {1, 2, 3};
        char             dest[sizeof(std::vector<int>)];
        cav::move_uninit(dest, src);
        CHECK(*reinterpret_cast<std::vector<int>*>(dest) == src);
    }
}

TEST_CASE("move_uninit_span") {
    SUBCASE("move_uninit_span(D&&, S&&) - trivially copiable types") {
        std::vector<int> src = {1, 2, 3};
        std::vector<int> dest(3);
        cav::move_uninit_span(dest, src);
        CHECK(dest == src);
    }

    SUBCASE("move_uninit_span(D&&, S&&) - non trivially copiable types") {
        std::vector<ClassType<int>> src = {ClassType<int>{1}, ClassType<int>{2}, ClassType<int>{3}};
        std::vector<ClassType<int>> dest(3);
        cav::move_uninit_span(dest, src);
        CHECK(dest[0] == ClassType<int>{1});
        CHECK(dest[1] == ClassType<int>{2});
        CHECK(dest[2] == ClassType<int>{3});
    }
}

TEST_CASE("insertion_sort int") {
    auto arr  = std::vector<int>(200);
    auto buff = std::vector<int>(200);
    for (size_t i = 0; i < 100; ++i) {
        for (size_t s = 2; s <= 200; s = s * 17 / 3) {
            auto subseq = make_span(arr.data(), s);

            for (int& elem : subseq)
                elem = rand() % 1024;
            REQUIRE_NOTHROW(insertion_sort(subseq));
            CHECK(is_sorted(subseq));

            for (int& elem : subseq)
                elem = rand() % 1024;
            REQUIRE_NOTHROW(insertion_sort(subseq, [](int x) { return -x; }));
            CHECK(is_sorted(subseq, [](int x) { return -x; }));
        }
    }
}

TEST_CASE("insertion_sort double") {
    auto arr  = std::vector<double>(200);
    auto buff = std::vector<double>(200);
    for (size_t i = 0; i < 100; ++i) {
        for (size_t s = 2; s <= 200; s = s * 17 / 3) {
            auto subseq = make_span(arr.data(), s);

            for (double& elem : subseq)
                elem = rand() / 1024.0;
            REQUIRE_NOTHROW(insertion_sort(subseq));
            CHECK(is_sorted(subseq));

            for (double& elem : subseq)
                elem = rand() / 1024.0;
            REQUIRE_NOTHROW(insertion_sort(subseq, [](double x) { return -x; }));
            CHECK(is_sorted(subseq, [](double x) { return -x; }));
        }
    }
}

TEST_CASE("insertion_sort int") {
    auto arr  = std::vector<ClassType<int>>(200);
    auto buff = std::vector<ClassType<int>>(200);
    for (size_t i = 0; i < 100; ++i) {
        for (size_t s = 2; s <= 200; s = s * 17 / 3) {
            auto subseq = make_span(arr.data(), s);

            for (ClassType<int>& elem : subseq)
                elem = ClassType<int>(rand() % 1024);
            REQUIRE_NOTHROW(insertion_sort(subseq));
            CHECK(is_sorted(subseq));

            for (ClassType<int>& elem : subseq)
                elem = ClassType<int>(rand() % 1024);
            REQUIRE_NOTHROW(insertion_sort(subseq, [](ClassType<int> x) { return -x; }));
            CHECK(is_sorted(subseq, [](ClassType<int> x) { return -x; }));
        }
    }
}

TEST_CASE("insertion_sort double") {
    auto arr  = std::vector<ClassType<double>>(200);
    auto buff = std::vector<ClassType<double>>(200);
    for (size_t i = 0; i < 100; ++i) {
        for (size_t s = 2; s <= 200; s = s * 17 / 3) {
            auto subseq = make_span(arr.data(), s);

            for (ClassType<double>& elem : subseq)
                elem = ClassType<double>(rand() / 1024.0);
            REQUIRE_NOTHROW(insertion_sort(subseq));
            CHECK(is_sorted(subseq));

            for (ClassType<double>& elem : subseq)
                elem = ClassType<double>(rand() / 1024.0);
            REQUIRE_NOTHROW(insertion_sort(subseq, [](ClassType<double> x) { return -x; }));
            CHECK(is_sorted(subseq, [](ClassType<double> x) { return -x; }));
        }
    }
}
}  // namespace cav
