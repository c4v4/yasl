# SPDX-FileCopyrightText: 2024 Francesco Cavaliere
# SPDX-License-Identifier: MIT

cmake_minimum_required(VERSION 3.16)
project(yasl)
include(FetchContent)

set(CMAKE_CXX_STANDARD 11)

# Vscode integration for included libraries
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

FetchContent_Declare(fmt GIT_REPOSITORY https://github.com/fmtlib/fmt)
FetchContent_MakeAvailable(fmt)

set(LIBRARIES fmt::fmt dl m)

set(WARNING_FLAGS "")#-Wall -Wextra -Wpedantic -Wuninitialized -Wshadow -Werror")
set(SANITIZERS_FLAGS "-fno-omit-frame-pointer -fsanitize=address -fsanitize-address-use-after-scope -fsanitize=undefined")
#set(OPT_FLAGS "-O3 -flto=auto")
set(OPT_FLAGS "-O3 -flto=auto -march=native -mtune=native")
#set(OPT_FLAGS "-O3 -flto=auto -march=native -mtune=native -ffast-math")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${WARNING_FLAGS}")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${SANITIZERS_FLAGS}")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${OPT_FLAGS}")

set(CMAKE_VERBOSE_MAKEFILE ON)

include_directories(include)
add_executable(sort src/sort.cpp)
target_link_libraries(sort PUBLIC ${LIBRARIES})

add_executable(nth_elem src/nth_elem.cpp)
target_link_libraries(nth_elem PUBLIC ${LIBRARIES})

########################################
############## Unit tests ##############
########################################
option(UNIT_TESTS "Build unit tests." OFF)
message(STATUS "UNIT_TESTS: ${UNIT_TESTS}")
if (UNIT_TESTS) 
    enable_testing()
    add_subdirectory(test)
endif() 