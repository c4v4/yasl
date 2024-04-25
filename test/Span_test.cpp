// SPDX-FileCopyrightText: 2024 Francesco Cavaliere <francescocava95@gmail.com>
// SPDX-License-Identifier: MIT

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#define DOCTEST_CONFIG_SUPER_FAST_ASSERTS
#include "Span.hpp"

#include <doctest/doctest.h>

namespace cav {

TEST_CASE("Span size and empty") {
    int  arr[] = {1, 2, 3, 4, 5};
    auto span  = make_span(arr, 5);

    CHECK(span.size() == 5);
    CHECK_FALSE(span.empty());
}

TEST_CASE("Span back") {
    int  arr[] = {1, 2, 3, 4, 5};
    auto span  = make_span(arr, 5);

    CHECK(span.back() == 5);
}

TEST_CASE("Span iteration") {
    int  arr[] = {1, 2, 3, 4, 5};
    auto span  = make_span(arr, 5);

    int sum = 0;
    for (auto& element : span)
        sum += element;
    CHECK(sum == 15);
}

TEST_CASE("Span indexing") {
    int  arr[] = {1, 2, 3, 4, 5};
    auto span  = make_span(arr, 5);

    CHECK(span[0] == 1);
    CHECK(span[2] == 3);
    CHECK(span[4] == 5);
}

TEST_CASE("Make span") {
    int arr[] = {1, 2, 3, 4, 5};

    SUBCASE("Make span with iterators") {
        auto span = make_span(std::begin(arr), std::end(arr));
        CHECK(span.size() == 5);
        CHECK(span[0] == 1);
        CHECK(span[4] == 5);
    }

    SUBCASE("Make span with size") {
        auto span = make_span(std::begin(arr), 3);
        CHECK(span.size() == 3);
        CHECK(span[0] == 1);
        CHECK(span[2] == 3);
    }

    SUBCASE("Make span with container") {
        auto span = make_span(arr, 0, 3);
        CHECK(span.size() == 3);
        CHECK(span[0] == 1);
        CHECK(span[2] == 3);
    }
}
}  // namespace cav
