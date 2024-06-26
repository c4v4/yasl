// SPDX-FileCopyrightText: 2024 Francesco Cavaliere <francescocava95@gmail.com>
// SPDX-License-Identifier: MIT

#ifndef CAV_TEST_TESTS_UTILS_HPP
#define CAV_TEST_TESTS_UTILS_HPP

#include "limits.hpp"

namespace cav {
template <typename T, std::size_t Nbytes = 32>
class ClassType {
private:
    T    elem;
    char padding[Nbytes - sizeof(T)] = {};

public:
    ~ClassType() {
        elem = {};
    }

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

    bool operator==(ClassType const& other) const noexcept {
        return elem == other.elem;
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

    operator T() const noexcept {
        return elem;
    }
};
}  // namespace cav

#endif /* CAV_TEST_TESTS_UTILS_HPP */
