# SPDX-FileCopyrightText: 2024 Francesco Cavaliere <francescocava95@gmail.com>
# SPDX-License-Identifier: MIT
set(CMAKE_CXX_STANDARD 11)

FetchContent_Declare(doctest GIT_REPOSITORY https://github.com/doctest/doctest GIT_TAG master)
FetchContent_MakeAvailable(doctest)

set(LIBRARIES ${LIBRARIES} doctest)

set(GCC_COVERAGE_COMPILE_FLAGS "-fprofile-arcs -ftest-coverage")
set(GCC_COVERAGE_LINK_FLAGS    "-lgcov --coverage")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS} -DCFT_ASSERT_FAIL_THROWS")
set(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} ${GCC_COVERAGE_LINK_FLAGS} -DCFT_ASSERT_FAIL_THROWS" )

function(add_cav_test name)
    set(sources ${name}.cpp)
    add_executable(${name} ${sources})
    target_link_libraries(${name} PUBLIC ${LIBRARIES})
    add_test(NAME ${name} COMMAND ${name})
endfunction()

add_cav_test(net_sort_test)
add_cav_test(radix_sort_test)
add_cav_test(sort_test)
add_cav_test(sort_utils_test)
add_cav_test(sorting_networks_test)
add_cav_test(Span_test)
add_cav_test(utils_test)
