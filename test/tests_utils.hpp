#ifndef CAV_TEST_TESTS_UTILS_HPP
#define CAV_TEST_TESTS_UTILS_HPP

#include "limits.hpp"

namespace cav {
template <typename T>
class ClassType {
private:
    T    elem;
    char padding[32 - sizeof(T)] = {};

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
}  // namespace cav

#endif /* CAV_TEST_TESTS_UTILS_HPP */
