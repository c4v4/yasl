// SPDX-FileCopyrightText: 2024 Francesco Cavaliere <francescocava95@gmail.com>
// SPDX-License-Identifier: MIT

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#define DOCTEST_CONFIG_SUPER_FAST_ASSERTS
#include "utils.hpp"

#include <doctest/doctest.h>

#include <array>
#include <vector>

namespace cav {

TEST_CASE("test_clamp") {
    SUBCASE("Value within range") {
        CHECK(clamp(5, 0, 10) == 5);
        CHECK(clamp(3.14, 0.0, 5.0) == 3.14);
        CHECK(clamp('c', 'a', 'z') == 'c');
    }

    SUBCASE("Value below lower bound") {
        CHECK(clamp(-5, 0, 10) == 0);
        CHECK(clamp(-3.14, 0.0, 5.0) == 0.0);
        CHECK(clamp('a', 'c', 'z') == 'c');
    }

    SUBCASE("Value above upper bound") {
        CHECK(clamp(15, 0, 10) == 10);
        CHECK(clamp(7.5, 0.0, 5.0) == 5.0);
        CHECK(clamp('z', 'a', 'c') == 'c');
    }
}

TEST_CASE("test_abs") {
    SUBCASE("Positive value") {
        CHECK(abs(5) == 5);
        CHECK(abs(3.14) == 3.14);
    }

    SUBCASE("Negative value") {
        CHECK(abs(-5) == 5);
        CHECK(abs(-3.14) == 3.14);
    }

    SUBCASE("Zero value") {
        CHECK(abs(0) == 0);
    }
}

TEST_CASE("test_max") {
    SUBCASE("Integers") {
        CHECK(max(1, 2) == 2);
        CHECK(max(1, 2, 3) == 3);
        CHECK(max(3, 2, 1) == 3);
    }

    SUBCASE("Floating-point numbers") {
        CHECK(max(1.0, 2.0) == 2.0);
        CHECK(max(1.0, 2.0, 3.0) == 3.0);
        CHECK(max(3.0, 2.0, 1.0) == 3.0);
    }
}

TEST_CASE("test_min") {
    SUBCASE("Integers") {
        CHECK(min(1, 2) == 1);
        CHECK(min(1, 2, 3) == 1);
        CHECK(min(3, 2, 1) == 1);
    }

    SUBCASE("Floating-point numbers") {
        CHECK(min(1.0, 2.0) == 1.0);
        CHECK(min(1.0, 2.0, 3.0) == 1.0);
        CHECK(min(3.0, 2.0, 1.0) == 1.0);
    }
}

TEST_CASE("test_any") {
    SUBCASE("Empty container") {
        std::vector<int> empty_container;
        CHECK_FALSE(any(empty_container, [](int x) { return x > 0; }));
    }

    SUBCASE("Container with positive values") {
        int container[] = {1, 2, 3, 4, 5};
        CHECK(any(container, [](int x) { return x > 0; }));
    }

    SUBCASE("Container with negative values") {
        std::array<int, 5> container = {-1, -2, -3, -4, -5};
        CHECK_FALSE(any(container, [](int x) { return x > 0; }));
    }

    SUBCASE("Container with mixed values") {
        int container[5] = {-1, 0, 1, 2, 3};
        CHECK(any(container, [](int x) { return x > 0; }));
    }
}

TEST_CASE("test_all") {
    SUBCASE("Empty container") {
        std::vector<int> empty_container;
        CHECK(all(empty_container, [](int x) { return x > 0; }));
    }

    SUBCASE("Container with positive values") {
        int container[] = {1, 2, 3, 4, 5};
        CHECK(all(container, [](int x) { return x > 0; }));
    }

    SUBCASE("Container with negative values") {
        std::array<int, 5> container = {-1, -2, -3, -4, -5};
        CHECK_FALSE(all(container, [](int x) { return x > 0; }));
    }

    SUBCASE("Container with mixed values") {
        int container[5] = {-1, 0, 1, 2, 3};
        CHECK_FALSE(all(container, [](int x) { return x > 0; }));
    }
}

TEST_CASE("test_size") {
    SUBCASE("Empty container") {
        std::vector<int> empty_container;
        CHECK(cav::size(empty_container) == 0);
    }

    SUBCASE("Non-empty container") {
        std::array<int, 5> container = {1, 2, 3, 4, 5};
        CHECK(cav::size(container) == 5);
    }

    SUBCASE("Array") {
        int arr[] = {1, 2, 3, 4, 5};
        CHECK(size(arr) == 5);
    }
}

TEST_CASE("is_sorted") {
    SUBCASE("Empty container") {
        std::vector<int> empty;
        CHECK(cav::is_sorted(empty));
    }

    SUBCASE("Sorted container") {
        std::vector<int> sorted = {1, 2, 3, 4, 5};
        CHECK(cav::is_sorted(sorted));
    }

    SUBCASE("Unsorted container") {
        std::vector<int> unsorted = {5, 4, 3, 2, 1};
        CHECK_FALSE(cav::is_sorted(unsorted));
    }
}

TEST_CASE("is_nth_elem") {
    SUBCASE("Empty container") {
        std::vector<int> empty;
        CHECK(cav::is_nth_elem(empty, 0));
    }

    SUBCASE("Nth element is smallest") {
        std::vector<int> container = {1, 2, 3, 4, 5};
        CHECK(cav::is_nth_elem(container, 0));
    }

    SUBCASE("Nth element is largest") {
        std::vector<int> container = {5, 4, 3, 2, 1};
        CHECK(!cav::is_nth_elem(container, 4));
    }

    SUBCASE("Nth element is not smallest or largest") {
        std::vector<int> container = {2, 1, 3, 4, 5};
        CHECK(cav::is_nth_elem(container, 2));
    }
}


}  // namespace cav