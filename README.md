<!--
SPDX-FileCopyrightText: 2024 Francesco Cavaliere
SPDX-License-Identifier: MIT
-->

# Yet Another Sorting Library
[![Language](https://img.shields.io/badge/C++-11-purple.svg)](https://en.cppreference.com/w/cpp/11)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Ubuntu CI](https://github.com/c4v4/yasl/actions/workflows/c-cpp.yml/badge.svg?branch=main)](https://github.com/c4v4/yasl/actions/workflows/c-cpp.yml)
[![codecov](https://codecov.io/gh/c4v4/yasl/graph/badge.svg?token=AYB5IHFSCD)](https://codecov.io/gh/c4v4/yasl)
[![Codacy Badge](https://app.codacy.com/project/badge/Grade/f82fe574441d4c26b7ed304ad519ed2a)](https://app.codacy.com/gh/c4v4/yasl/dashboard?utm_source=gh&utm_medium=referral&utm_content=&utm_campaign=Badge_grade)
[![REUSE status](https://api.reuse.software/badge/github.com/c4v4/yasl)](https://api.reuse.software/info/github.com/c4v4/yasl)

**!!! THIS REPO IS STILL WORK IN PROGRESS !!!**

A repo where I fool around with different sorting algorithms. 
I wanted to see if I could find an algorithm faster than `std::sort` when I can make some extra assumptions. 
Here are the assumptions:

- I can use extra O(N) space if it helps make things faster.
- The things I'm sorting can be represented by a key that is a simple numerical type (like integers or floating points)
- Focus on struct-like and indirect sorting (more interesting to my needs than plain native types).
- Values cover a fraction of the whole type range (since I never use the whole range).

## Tests and Coverage

To build the project with unit tests in debug mode:

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Debug -DUNIT_TESTS=ON 
cmake --build build -j
```

Run the tests:

```bash
ctest --test-dir build -j
```

Use the following commands to generate code coverage statistics, note that you need to have [`lcov`](https://github.com/linux-test-project/lcov) installed.

```bash
mkdir -p coverage
lcov -c -d build -o coverage/tests_cov.info
lcov -e coverage/tests_cov.info yasl/include/ -o coverage/tests_cov.info
```

Finally, to generate an HTML-report that can be viewed from your browser:

```bash
genhtml coverage/tests_cov.info --legend --output-directory=./coverage
```

You can check out the coverage statistics by opening `coverage/index.html`.


## Preliminary Results
I tried to handpick good thresholds that select the best algorithm available for any scenario considered. This approach has two main downfalls:

 1. It *strongly* overfits my hardware (ThinkPad P16v, intel i7-13700H, 32Gb RAM)
 2. The "scenario detection" is far from optimal. Without C++ reflection, I have to guess "how" and "what" I'm sorting and try to strike a good balance with common scenarios.

But I tried anyway because it was fun.

### Plots
Here are some plots I've selected, in general, I'm not far from Malte Skarupke's implementation. He switches to MSD radix sort for keys >= 64bit, I do not because in my hardware it pays more to use LSD instead.
I also take advantage of the fact that I do not expect to use the whole range of values (which can help skip some bytes, especially with 64bit integers).

Let's start with plain 32-bit integers. My LSD implementation gets optimize quite aggressively and reaches Malte Skarupke's one. Indeed they perform pretty much in the same way.
For smaller sizes, sorting networks seems to be the best option.

![32bit signed integer sorting](https://docs.google.com/spreadsheets/d/e/2PACX-1vRh2sS1riIJwOcgfrk2cquUx9g1TWi-zR6YUqTR__sn9OdForEh8gI7O_BzF5L3lJJuzWMtDjGsDW1A/pubchart?oid=1003531067&format=image)

For 64bit integers there're some differences, here ska_sort fallbacks to the MSD version and (in my hardware) it loses some of its performance.

![64bit signed integer sorting](https://docs.google.com/spreadsheets/d/e/2PACX-1vRh2sS1riIJwOcgfrk2cquUx9g1TWi-zR6YUqTR__sn9OdForEh8gI7O_BzF5L3lJJuzWMtDjGsDW1A/pubchart?oid=1825503312&format=image)

Same thing for sorting doubles. Here however the LSD optimization that skips bytes if they are all equal should be less useful.

![Double sorting](https://docs.google.com/spreadsheets/d/e/2PACX-1vRh2sS1riIJwOcgfrk2cquUx9g1TWi-zR6YUqTR__sn9OdForEh8gI7O_BzF5L3lJJuzWMtDjGsDW1A/pubchart?oid=905839582&format=image)

Let's consider now a struct-like object (still trivially copyable) with a double field as a key and 56bytes of padding to reach a full cache line of size.
In this case, the sorting network actually struggles, while the STL insertion sort (that std::sort fallbacks to) seems to be the best option for small sizes.
Then, for larger sequences, MDS radix sort seems to beat LSD (at least in my implementation), so I selected that pretty early.

![Sorting 64byte struct with double key field](https://docs.google.com/spreadsheets/d/e/2PACX-1vRh2sS1riIJwOcgfrk2cquUx9g1TWi-zR6YUqTR__sn9OdForEh8gI7O_BzF5L3lJJuzWMtDjGsDW1A/pubchart?oid=1546247968&format=image)

Finally, I considered the case where I want to sort indexes based on keys that are stored outside (this scenario is detected by a non-empty key function object, so it's easily cheatable).
Here again, MSD can perform better than LSD so it's chosen for long sequences.

![Sorting indexes with 64bit signed integer key accessed indirectly](https://docs.google.com/spreadsheets/d/e/2PACX-1vRh2sS1riIJwOcgfrk2cquUx9g1TWi-zR6YUqTR__sn9OdForEh8gI7O_BzF5L3lJJuzWMtDjGsDW1A/pubchart?oid=1568622674&format=image)


## Raw preliminary results
Here are the complete results from which I extracted the plots.

 - `type`: the type sorted, 3 categories: natives, struct with native key + padding (`<key>_<size>B`), type sorted indirectly using external array for keys.
 - `length`: average length of the sequences sorted.
 - `samples`: number of sequences sorted.
 - `range`: range of value.
 - `net-sort`: sort based on sorting networks and merging.
 - `lsd-rdx`: least significant digit radix sort.
 - `msd-rdx`: most stignificant digit radix sort.
 - `cav-sort`: mixup of the various algorithms I have implemented, with handpicked thresholds.
 - `std-sort`: C++ STL sorting algorithm (a hybrid of insertion sort, heap sort, and quick sort).
 - `ska-sort`: Malte Skarupke sorting algorithm (`ska_sort_copy`, hybrid of lsd radix sort, msd radix for keys larger than 8 bytes, and std::sort for small sequences).

Like in Malte Skarupke benchmarks, the numbers report the average nanoseconds spent for each element of the sequence (to better underline differences).

```
type         length   samples            range   net-sort    lsd-rdx    msd-rdx   cav-sort   std-sort   ska-sort
int8_t            8   1250000 [ -1e+01, 1e+01]          2         13          9          2          8          7
int8_t           16    625000 [ -1e+01, 1e+01]          7          7          9          7         11         11
int8_t           32    312500 [ -1e+01, 1e+01]         22          4          5          4         16         16
int8_t           64    156250 [ -1e+01, 1e+01]         23          3          3          3         20         19
int8_t          128     78125 [ -1e+01, 1e+01]         24          2          2          2         21         11
int8_t          256     39062 [ -1e+01, 1e+01]         25          1          2          1         21          1
int8_t          512     19531 [ -1e+01, 1e+01]         25          1          2          1         21          1
int8_t         1024      9765 [ -1e+01, 1e+01]         25          1          1          1         21          1
int8_t         2048      4882 [ -1e+01, 1e+01]         25          1          1          1         21          1
int8_t         4096      2441 [ -1e+01, 1e+01]         25          1          2          1         21          1
int8_t         8192      1220 [ -1e+01, 1e+01]         25          1          2          1         21          1
int8_t        16384       610 [ -1e+01, 1e+01]         26          1          2          1         21          1
int8_t        32768       305 [ -1e+01, 1e+01]         27          1          2          1         21          1
int8_t        65536       152 [ -1e+01, 1e+01]         27          1          3          1         21          1
int8_t       131072        76 [ -1e+01, 1e+01]         27          1          3          1         21          1
int8_t       262144        38 [ -1e+01, 1e+01]         28          1          3          1         21          1
int8_t       524288        19 [ -1e+01, 1e+01]         28          1          3          1         21          1
int8_t      1048576         9 [ -1e+01, 1e+01]         29          1          2          1         21          1
int8_t      2097152         4 [ -1e+01, 1e+01]         30          1          3          1         21          1
int8_t      4194304         2 [ -1e+01, 1e+01]         31          1          2          1         21          1
int8_t      8388608         1 [ -1e+01, 1e+01]         29          1          2          1         21          1

type         length   samples            range   net-sort    lsd-rdx    msd-rdx   cav-sort   std-sort   ska-sort
int16_t           8   1250000 [ -2e+02, 2e+02]          2         24          9          2          7          7
int16_t          16    625000 [ -2e+02, 2e+02]          2         14         10          2         10         10
int16_t          32    312500 [ -2e+02, 2e+02]          4          8         15          4         15         15
int16_t          64    156250 [ -2e+02, 2e+02]          7          5          9          5         18         18
int16_t         128     78125 [ -2e+02, 2e+02]         11          4          6          4         22         12
int16_t         256     39062 [ -2e+02, 2e+02]         14          3          5          3         25          3
int16_t         512     19531 [ -2e+02, 2e+02]         18          3          4          3         28          3
int16_t        1024      9765 [ -2e+02, 2e+02]         22          3          4          3         31          3
int16_t        2048      4882 [ -2e+02, 2e+02]         24          3          4          3         32          3
int16_t        4096      2441 [ -2e+02, 2e+02]         26          3          3          3         32          3
int16_t        8192      1220 [ -2e+02, 2e+02]         28          3          3          3         32          3
int16_t       16384       610 [ -2e+02, 2e+02]         29          3          3          3         32          3
int16_t       32768       305 [ -2e+02, 2e+02]         30          3          4          3         32          3
int16_t       65536       152 [ -2e+02, 2e+02]         31          3          4          3         32          3
int16_t      131072        76 [ -2e+02, 2e+02]         31          3          3          3         31          3
int16_t      262144        38 [ -2e+02, 2e+02]         32          3          4          3         31          3
int16_t      524288        19 [ -2e+02, 2e+02]         32          3          4          3         31          3
int16_t     1048576         9 [ -2e+02, 2e+02]         32          3          4          3         31          3
int16_t     2097152         4 [ -2e+02, 2e+02]         33          3          4          3         31          3
int16_t     4194304         2 [ -2e+02, 2e+02]         34          3          4          3         31          3
int16_t     8388608         1 [ -2e+02, 2e+02]         36          3          4          3         32          3

type         length   samples            range   net-sort    lsd-rdx    msd-rdx   cav-sort   std-sort   ska-sort
int32_t           8   1250000 [ -5e+04, 5e+04]          1         57          9          1          7          8
int32_t          16    625000 [ -5e+04, 5e+04]          1         35         10          1         10         10
int32_t          32    312500 [ -5e+04, 5e+04]          3         20         11          3         15         15
int32_t          64    156250 [ -5e+04, 5e+04]          7         10         15          7         19         19
int32_t         128     78125 [ -5e+04, 5e+04]         11          8         36          8         22         14
int32_t         256     39062 [ -5e+04, 5e+04]         14          7         29          7         25          7
int32_t         512     19531 [ -5e+04, 5e+04]         18          6         23          6         28          6
int32_t        1024      9765 [ -5e+04, 5e+04]         22          6         19          6         31          6
int32_t        2048      4882 [ -5e+04, 5e+04]         25          6         17          6         34          6
int32_t        4096      2441 [ -5e+04, 5e+04]         29          5         16          6         37          6
int32_t        8192      1220 [ -5e+04, 5e+04]         33          6         16          6         40          6
int32_t       16384       610 [ -5e+04, 5e+04]         37          6         15          6         43          6
int32_t       32768       305 [ -5e+04, 5e+04]         41          6         10          6         46          7
int32_t       65536       152 [ -5e+04, 5e+04]         45          6          9          6         50          7
int32_t      131072        76 [ -5e+04, 5e+04]         48          6          8          6         52          6
int32_t      262144        38 [ -5e+04, 5e+04]         52          6          8          6         55          7
int32_t      524288        19 [ -5e+04, 5e+04]         55          6          8          6         56          7
int32_t     1048576         9 [ -5e+04, 5e+04]         57          6          8          6         56          7
int32_t     2097152         4 [ -5e+04, 5e+04]         58          6          9          6         56          7
int32_t     4194304         2 [ -5e+04, 5e+04]         59          6          8          6         56          7
int32_t     8388608         1 [ -5e+04, 5e+04]         61          6          9          6         56          7

type         length   samples            range   net-sort    lsd-rdx    msd-rdx   cav-sort   std-sort   ska-sort
int64_t           8   1250000 [ -3e+09, 3e+09]          2        131          9          2          7          7
int64_t          16    625000 [ -3e+09, 3e+09]          2         80         10          2         10         10
int64_t          32    312500 [ -3e+09, 3e+09]          4         45         12          4         15         16
int64_t          64    156250 [ -3e+09, 3e+09]          7         24         14          7         19         20
int64_t         128     78125 [ -3e+09, 3e+09]         11         17         17         11         22         26
int64_t         256     39062 [ -3e+09, 3e+09]         14         14         38         14         25         32
int64_t         512     19531 [ -3e+09, 3e+09]         18         13         30         13         28         29
int64_t        1024      9765 [ -3e+09, 3e+09]         22         12         25         12         31         24
int64_t        2048      4882 [ -3e+09, 3e+09]         25         12         22         12         34         21
int64_t        4096      2441 [ -3e+09, 3e+09]         29         13         22         13         37         21
int64_t        8192      1220 [ -3e+09, 3e+09]         33         13         23         13         40         25
int64_t       16384       610 [ -3e+09, 3e+09]         37         13         25         13         43         28
int64_t       32768       305 [ -3e+09, 3e+09]         42         13         25         13         46         31
int64_t       65536       152 [ -3e+09, 3e+09]         45         13         20         13         49         25
int64_t      131072        76 [ -3e+09, 3e+09]         49         14         20         14         52         23
int64_t      262144        38 [ -3e+09, 3e+09]         53         14         21         14         54         22
int64_t      524288        19 [ -3e+09, 3e+09]         57         14         24         14         57         23
int64_t     1048576         9 [ -3e+09, 3e+09]         60         14         25         14         59         23
int64_t     2097152         4 [ -3e+09, 3e+09]         64         15         26         15         62         26
int64_t     4194304         2 [ -3e+09, 3e+09]         69         15         30         15         65         29
int64_t     8388608         1 [ -3e+09, 3e+09]         73         15         38         15         66         32

type         length   samples            range   net-sort    lsd-rdx    msd-rdx   cav-sort   std-sort   ska-sort
float             8   1250000 [ -2e+19, 2e+19]          2         58          9          2          8          8
float            16    625000 [ -2e+19, 2e+19]          2         37         10          2         10         10
float            32    312500 [ -2e+19, 2e+19]          4         23         12          4         16         16
float            64    156250 [ -2e+19, 2e+19]          8         10         16          8         20         20
float           128     78125 [ -2e+19, 2e+19]         12          7         15          7         24         15
float           256     39062 [ -2e+19, 2e+19]         16          6         14          6         27          6
float           512     19531 [ -2e+19, 2e+19]         20          6         11          5         30          6
float          1024      9765 [ -2e+19, 2e+19]         24          5         11          5         34          5
float          2048      4882 [ -2e+19, 2e+19]         28          6         12          6         37          5
float          4096      2441 [ -2e+19, 2e+19]         32          5         12          5         40          5
float          8192      1220 [ -2e+19, 2e+19]         37          5         13          5         43          6
float         16384       610 [ -2e+19, 2e+19]         41          5         15          5         46          6
float         32768       305 [ -2e+19, 2e+19]         45          5         25          5         49          6
float         65536       152 [ -2e+19, 2e+19]         49          5         28          5         52          6
float        131072        76 [ -2e+19, 2e+19]         53          5         28          5         56          6
float        262144        38 [ -2e+19, 2e+19]         57          5         26          5         59          6
float        524288        19 [ -2e+19, 2e+19]         61          6         23          6         62          6
float       1048576         9 [ -2e+19, 2e+19]         65          6         21          6         64          6
float       2097152         4 [ -2e+19, 2e+19]         69          6         20          6         66          6
float       4194304         2 [ -2e+19, 2e+19]         73          6         21          6         69          6
float       8388608         1 [ -2e+19, 2e+19]         76          6         19          6         71          6

type         length   samples            range   net-sort    lsd-rdx    msd-rdx   cav-sort   std-sort   ska-sort
double            8   1250000 [-1e+154,1e+154]          2        136          9          2          7          8
double           16    625000 [-1e+154,1e+154]          2         83         10          2         10         11
double           32    312500 [-1e+154,1e+154]          4         52         12          4         16         16
double           64    156250 [-1e+154,1e+154]          8         25         14          8         20         20
double          128     78125 [-1e+154,1e+154]         12         16         18         12         23         27
double          256     39062 [-1e+154,1e+154]         16         13         13         13         27         30
double          512     19531 [-1e+154,1e+154]         20         12         13         12         30         27
double         1024      9765 [-1e+154,1e+154]         25         11         14         11         33         25
double         2048      4882 [-1e+154,1e+154]         29         11         15         11         37         23
double         4096      2441 [-1e+154,1e+154]         33         12         17         12         40         23
double         8192      1220 [-1e+154,1e+154]         37         12         23         12         43         25
double        16384       610 [-1e+154,1e+154]         42         12         25         12         46         24
double        32768       305 [-1e+154,1e+154]         46         12         23         12         50         23
double        65536       152 [-1e+154,1e+154]         50         12         20         12         53         22
double       131072        76 [-1e+154,1e+154]         54         13         19         13         56         21
double       262144        38 [-1e+154,1e+154]         58         13         19         13         59         21
double       524288        19 [-1e+154,1e+154]         63         13         19         13         62         23
double      1048576         9 [-1e+154,1e+154]         67         13         20         13         65         25
double      2097152         4 [-1e+154,1e+154]         72         14         19         14         68         26
double      4194304         2 [-1e+154,1e+154]         76         14         18         14         71         25
double      8388608         1 [-1e+154,1e+154]         80         14         19         14         74         24

type         length   samples            range   net-sort    lsd-rdx    msd-rdx   cav-sort   std-sort   ska-sort
flt_8B            8   1250000 [ -2e+19, 2e+19]          9         58         10          8          8          8
flt_8B           16    625000 [ -2e+19, 2e+19]         14         37         10          9         11         11
flt_8B           32    312500 [ -2e+19, 2e+19]         23         24         12         11         17         17
flt_8B           64    156250 [ -2e+19, 2e+19]         27         11         16         11         21         21
flt_8B          128     78125 [ -2e+19, 2e+19]         30          8         15          8         24         15
flt_8B          256     39062 [ -2e+19, 2e+19]         34          7         14          7         28          6
flt_8B          512     19531 [ -2e+19, 2e+19]         37          6         12          6         31          5
flt_8B         1024      9765 [ -2e+19, 2e+19]         41          6         12          6         35          5
flt_8B         2048      4882 [ -2e+19, 2e+19]         45          6         13          6         38          5
flt_8B         4096      2441 [ -2e+19, 2e+19]         50          6         14          6         42          6
flt_8B         8192      1220 [ -2e+19, 2e+19]         54          6         14          6         45          6
flt_8B        16384       610 [ -2e+19, 2e+19]         58          6         15          6         49          6
flt_8B        32768       305 [ -2e+19, 2e+19]         62          6         24          6         52          6
flt_8B        65536       152 [ -2e+19, 2e+19]         67          6         27          6         56          6
flt_8B       131072        76 [ -2e+19, 2e+19]         72          6         26          6         59          6
flt_8B       262144        38 [ -2e+19, 2e+19]         75          6         24          6         61          6
flt_8B       524288        19 [ -2e+19, 2e+19]         79          6         21          7         64          6
flt_8B      1048576         9 [ -2e+19, 2e+19]         83          7         19          6         67          6
flt_8B      2097152         4 [ -2e+19, 2e+19]         88          7         19          7         70          7
flt_8B      4194304         2 [ -2e+19, 2e+19]         92          7         19          7         73          7
flt_8B      8388608         1 [ -2e+19, 2e+19]         96          7         17          7         77          7

type         length   samples            range   net-sort    lsd-rdx    msd-rdx   cav-sort   std-sort   ska-sort
flt_16B           8   1250000 [ -2e+19, 2e+19]          9         59         10          8          8          8
flt_16B          16    625000 [ -2e+19, 2e+19]         15         38         10          9         11         11
flt_16B          32    312500 [ -2e+19, 2e+19]         24         27         12         11         17         17
flt_16B          64    156250 [ -2e+19, 2e+19]         27         12         17         12         22         22
flt_16B         128     78125 [ -2e+19, 2e+19]         31          9         16          9         25         16
flt_16B         256     39062 [ -2e+19, 2e+19]         35          8         15          7         29          7
flt_16B         512     19531 [ -2e+19, 2e+19]         39          7         13          7         32          7
flt_16B        1024      9765 [ -2e+19, 2e+19]         43          7         13          7         36          7
flt_16B        2048      4882 [ -2e+19, 2e+19]         48          7         14          7         39          8
flt_16B        4096      2441 [ -2e+19, 2e+19]         53          7         15          7         43          7
flt_16B        8192      1220 [ -2e+19, 2e+19]         57          7         16          7         46          7
flt_16B       16384       610 [ -2e+19, 2e+19]         62          8         17          8         50          8
flt_16B       32768       305 [ -2e+19, 2e+19]         67          8         26          8         53          7
flt_16B       65536       152 [ -2e+19, 2e+19]         72          8         30          8         56          8
flt_16B      131072        76 [ -2e+19, 2e+19]         76          8         29          8         60          8
flt_16B      262144        38 [ -2e+19, 2e+19]         80          8         27          8         63          8
flt_16B      524288        19 [ -2e+19, 2e+19]         85          8         25          9         66          9
flt_16B     1048576         9 [ -2e+19, 2e+19]         90          9         24          9         69          9
flt_16B     2097152         4 [ -2e+19, 2e+19]         94          9         23          9         72         10
flt_16B     4194304         2 [ -2e+19, 2e+19]         99         10         24          9         75         10
flt_16B     8388608         1 [ -2e+19, 2e+19]        104          9         22          9         79         10

type         length   samples            range   net-sort    lsd-rdx    msd-rdx   cav-sort   std-sort   ska-sort
flt_32B           8   1250000 [ -2e+19, 2e+19]         13         61         10          8          8          8
flt_32B          16    625000 [ -2e+19, 2e+19]         17         38         11         10         11         11
flt_32B          32    312500 [ -2e+19, 2e+19]         32         25         13         12         18         18
flt_32B          64    156250 [ -2e+19, 2e+19]         36         13         18         13         22         23
flt_32B         128     78125 [ -2e+19, 2e+19]         39         10         18         10         26         18
flt_32B         256     39062 [ -2e+19, 2e+19]         43          9         17          9         29          9
flt_32B         512     19531 [ -2e+19, 2e+19]         47          9         15          9         33          9
flt_32B        1024      9765 [ -2e+19, 2e+19]         52         10         15         10         36         10
flt_32B        2048      4882 [ -2e+19, 2e+19]         57         12         16         12         40         11
flt_32B        4096      2441 [ -2e+19, 2e+19]         61         11         17         11         43         11
flt_32B        8192      1220 [ -2e+19, 2e+19]         66         12         18         12         47         11
flt_32B       16384       610 [ -2e+19, 2e+19]         71         12         19         12         50         12
flt_32B       32768       305 [ -2e+19, 2e+19]         76         13         28         13         54         13
flt_32B       65536       152 [ -2e+19, 2e+19]         81         13         32         13         57         13
flt_32B      131072        76 [ -2e+19, 2e+19]         86         13         32         13         60         13
flt_32B      262144        38 [ -2e+19, 2e+19]         90         13         30         13         63         13
flt_32B      524288        19 [ -2e+19, 2e+19]         98         15         29         15         67         15
flt_32B     1048576         9 [ -2e+19, 2e+19]        102         16         28         16         70         15
flt_32B     2097152         4 [ -2e+19, 2e+19]        107         16         27         16         74         16
flt_32B     4194304         2 [ -2e+19, 2e+19]        112         16         28         16         77         16
flt_32B     8388608         1 [ -2e+19, 2e+19]        118         16         26         16         81         16

type         length   samples            range   net-sort    lsd-rdx    msd-rdx   cav-sort   std-sort   ska-sort
flt_64B           8   1250000 [ -2e+19, 2e+19]         14         62         11          9          9          9
flt_64B          16    625000 [ -2e+19, 2e+19]         21         39         12         11         13         12
flt_64B          32    312500 [ -2e+19, 2e+19]         36         28         14         13         21         21
flt_64B          64    156250 [ -2e+19, 2e+19]         39         16         22         16         26         25
flt_64B         128     78125 [ -2e+19, 2e+19]         43         14         22         13         29         21
flt_64B         256     39062 [ -2e+19, 2e+19]         47         13         20         13         32         13
flt_64B         512     19531 [ -2e+19, 2e+19]         53         16         19         16         36         16
flt_64B        1024      9765 [ -2e+19, 2e+19]         57         17         20         17         40         17
flt_64B        2048      4882 [ -2e+19, 2e+19]         62         18         21         18         43         18
flt_64B        4096      2441 [ -2e+19, 2e+19]         67         17         22         17         47         18
flt_64B        8192      1220 [ -2e+19, 2e+19]         72         18         23         18         51         18
flt_64B       16384       610 [ -2e+19, 2e+19]         78         20         26         20         55         20
flt_64B       32768       305 [ -2e+19, 2e+19]         84         21         35         21         58         21
flt_64B       65536       152 [ -2e+19, 2e+19]         89         20         39         20         62         21
flt_64B      131072        76 [ -2e+19, 2e+19]         95         21         40         21         66         21
flt_64B      262144        38 [ -2e+19, 2e+19]        107         24         40         24         70         24
flt_64B      524288        19 [ -2e+19, 2e+19]        116         26         39         26         74         25
flt_64B     1048576         9 [ -2e+19, 2e+19]        123         26         38         26         78         25
flt_64B     2097152         4 [ -2e+19, 2e+19]        129         26         38         26         84         25
flt_64B     4194304         2 [ -2e+19, 2e+19]        134         26         39         26         87         25
flt_64B     8388608         1 [ -2e+19, 2e+19]        139         26         37         26         92         25

type         length   samples            range   net-sort    lsd-rdx    msd-rdx   cav-sort   std-sort   ska-sort
dbl_16B           8   1250000 [-1e+154,1e+154]         10        135         10          8          8          8
dbl_16B          16    625000 [-1e+154,1e+154]         15         83         10          9         10         11
dbl_16B          32    312500 [-1e+154,1e+154]         24         81         12         11         17         17
dbl_16B          64    156250 [-1e+154,1e+154]         27         28         14         14         21         21
dbl_16B         128     78125 [-1e+154,1e+154]         31         18         19         18         25         28
dbl_16B         256     39062 [-1e+154,1e+154]         35         15         15         15         28         30
dbl_16B         512     19531 [-1e+154,1e+154]         39         14         15         14         31         26
dbl_16B        1024      9765 [-1e+154,1e+154]         44         13         15         13         35         24
dbl_16B        2048      4882 [-1e+154,1e+154]         48         14         16         14         39         23
dbl_16B        4096      2441 [-1e+154,1e+154]         54         15         18         15         42         24
dbl_16B        8192      1220 [-1e+154,1e+154]         57         15         25         15         45         26
dbl_16B       16384       610 [-1e+154,1e+154]         64         19         28         19         49         25
dbl_16B       32768       305 [-1e+154,1e+154]         67         15         25         15         52         23
dbl_16B       65536       152 [-1e+154,1e+154]         72         16         23         16         55         22
dbl_16B      131072        76 [-1e+154,1e+154]         76         16         21         16         58         21
dbl_16B      262144        38 [-1e+154,1e+154]         84         23         21         16         61         22
dbl_16B      524288        19 [-1e+154,1e+154]         85         16         21         16         65         24
dbl_16B     1048576         9 [-1e+154,1e+154]         90         18         23         18         68         27
dbl_16B     2097152         4 [-1e+154,1e+154]         94         18         23         18         70         29
dbl_16B     4194304         2 [-1e+154,1e+154]         99         19         23         19         73         27
dbl_16B     8388608         1 [-1e+154,1e+154]        105         19         24         19         76         26

type         length   samples            range   net-sort    lsd-rdx    msd-rdx   cav-sort   std-sort   ska-sort
dbl_32B           8   1250000 [-1e+154,1e+154]         13        140         10          8          8          8
dbl_32B          16    625000 [-1e+154,1e+154]         21         85         11         10         11         11
dbl_32B          32    312500 [-1e+154,1e+154]         33         79         13         12         18         18
dbl_32B          64    156250 [-1e+154,1e+154]         37         29         17         16         23         23
dbl_32B         128     78125 [-1e+154,1e+154]         42         20         22         25         26         29
dbl_32B         256     39062 [-1e+154,1e+154]         45         17         17         17         29         30
dbl_32B         512     19531 [-1e+154,1e+154]         49         16         17         16         33         26
dbl_32B        1024      9765 [-1e+154,1e+154]         54         19         17         19         36         25
dbl_32B        2048      4882 [-1e+154,1e+154]         58         22         19         22         40         25
dbl_32B        4096      2441 [-1e+154,1e+154]         63         21         21         21         43         27
dbl_32B        8192      1220 [-1e+154,1e+154]         68         22         28         22         47         28
dbl_32B       16384       610 [-1e+154,1e+154]         73         22         29         21         50         26
dbl_32B       32768       305 [-1e+154,1e+154]         79         24         27         24         54         25
dbl_32B       65536       152 [-1e+154,1e+154]         83         24         26         24         57         24
dbl_32B      131072        76 [-1e+154,1e+154]         87         24         25         24         60         24
dbl_32B      262144        38 [-1e+154,1e+154]         92         24         24         24         63         25
dbl_32B      524288        19 [-1e+154,1e+154]         99         28         28         27         67         28
dbl_32B     1048576         9 [-1e+154,1e+154]        104         29         29         29         70         31
dbl_32B     2097152         4 [-1e+154,1e+154]        108         30         28         28         73         34
dbl_32B     4194304         2 [-1e+154,1e+154]        113         30         27         27         77         33
dbl_32B     8388608         1 [-1e+154,1e+154]        120         30         29         29         80         31

type         length   samples            range   net-sort    lsd-rdx    msd-rdx   cav-sort   std-sort   ska-sort
dbl_64B           8   1250000 [-1e+154,1e+154]         14        142         11          9          9          9
dbl_64B          16    625000 [-1e+154,1e+154]         22         88         12         11         12         13
dbl_64B          32    312500 [-1e+154,1e+154]         36         72         14         13         21         21
dbl_64B          64    156250 [-1e+154,1e+154]         40         30         19         18         26         25
dbl_64B         128     78125 [-1e+154,1e+154]         44         24         26         30         29         32
dbl_64B         256     39062 [-1e+154,1e+154]         48         20         21         20         32         34
dbl_64B         512     19531 [-1e+154,1e+154]         53         28         22         28         36         29
dbl_64B        1024      9765 [-1e+154,1e+154]         57         31         22         31         39         28
dbl_64B        2048      4882 [-1e+154,1e+154]         62         32         23         32         43         29
dbl_64B        4096      2441 [-1e+154,1e+154]         67         31         26         31         47         31
dbl_64B        8192      1220 [-1e+154,1e+154]         72         33         33         33         50         32
dbl_64B       16384       610 [-1e+154,1e+154]         79         36         36         35         54         30
dbl_64B       32768       305 [-1e+154,1e+154]         83         38         35         35         58         30
dbl_64B       65536       152 [-1e+154,1e+154]         89         38         34         34         61         31
dbl_64B      131072        76 [-1e+154,1e+154]         95         39         33         33         66         31
dbl_64B      262144        38 [-1e+154,1e+154]        106         45         35         35         69         32
dbl_64B      524288        19 [-1e+154,1e+154]        116         48         38         38         73         37
dbl_64B     1048576         9 [-1e+154,1e+154]        124         49         41         41         78         42
dbl_64B     2097152         4 [-1e+154,1e+154]        131         49         40         40         82         44
dbl_64B     4194304         2 [-1e+154,1e+154]        135         49         39         39         86         45
dbl_64B     8388608         1 [-1e+154,1e+154]        146         49         40         40         92         44

type         length   samples            range   net-sort    lsd-rdx    msd-rdx   cav-sort   std-sort   ska-sort
i32_ind           8   1250000 [ -5e+04, 5e+04]         12         63         12         10         12         12
i32_ind          16    625000 [ -5e+04, 5e+04]         20         43         13         12         16         17
i32_ind          32    312500 [ -5e+04, 5e+04]         30         31         15         15         20         21
i32_ind          64    156250 [ -5e+04, 5e+04]         35         22         17         17         23         24
i32_ind         128     78125 [ -5e+04, 5e+04]         37         17         37         17         27         21
i32_ind         256     39062 [ -5e+04, 5e+04]         42         14         32         14         31         11
i32_ind         512     19531 [ -5e+04, 5e+04]         46         11         26         11         36         10
i32_ind        1024      9765 [ -5e+04, 5e+04]         49          9         21          9         39          8
i32_ind        2048      4882 [ -5e+04, 5e+04]         54          8         19          8         43          7
i32_ind        4096      2441 [ -5e+04, 5e+04]         59          8         18          7         47          7
i32_ind        8192      1220 [ -5e+04, 5e+04]         64          8         19          7         51          7
i32_ind       16384       610 [ -5e+04, 5e+04]         70          8         17          8         55          7
i32_ind       32768       305 [ -5e+04, 5e+04]         75          8         10          8         59          7
i32_ind       65536       152 [ -5e+04, 5e+04]         80          8          9          8         63          7
i32_ind      131072        76 [ -5e+04, 5e+04]         86          8          9          8         67          7
i32_ind      262144        38 [ -5e+04, 5e+04]         91          8          9          8         69          8
i32_ind      524288        19 [ -5e+04, 5e+04]         95          9         10          9         73          9
i32_ind     1048576         9 [ -5e+04, 5e+04]        101          9         12         10         74          9
i32_ind     2097152         4 [ -5e+04, 5e+04]        104          9         10          9         75         10
i32_ind     4194304         2 [ -5e+04, 5e+04]        106         10         11         10         76         10
i32_ind     8388608         1 [ -5e+04, 5e+04]        111         17         14         17         80         17

type         length   samples            range   net-sort    lsd-rdx    msd-rdx   cav-sort   std-sort   ska-sort
i64_ind           8   1250000 [ -3e+09, 3e+09]         12        144         12         10         12         12
i64_ind          16    625000 [ -3e+09, 3e+09]         19         96         13         12         16         17
i64_ind          32    312500 [ -3e+09, 3e+09]         30         92         15         15         21         21
i64_ind          64    156250 [ -3e+09, 3e+09]         34         51         18         18         23         23
i64_ind         128     78125 [ -3e+09, 3e+09]         37         37         19         19         27         31
i64_ind         256     39062 [ -3e+09, 3e+09]         41         29         39         29         30         38
i64_ind         512     19531 [ -3e+09, 3e+09]         45         23         30         23         33         35
i64_ind        1024      9765 [ -3e+09, 3e+09]         49         19         28         19         37         29
i64_ind        2048      4882 [ -3e+09, 3e+09]         54         18         25         18         41         25
i64_ind        4096      2441 [ -3e+09, 3e+09]         59         18         25         18         44         25
i64_ind        8192      1220 [ -3e+09, 3e+09]         64         18         27         18         48         29
i64_ind       16384       610 [ -3e+09, 3e+09]         70         18         29         18         53         32
i64_ind       32768       305 [ -3e+09, 3e+09]         75         18         29         18         56         35
i64_ind       65536       152 [ -3e+09, 3e+09]         80         18         24         18         60         28
i64_ind      131072        76 [ -3e+09, 3e+09]         86         20         25         20         65         26
i64_ind      262144        38 [ -3e+09, 3e+09]         91         21         26         21         70         28
i64_ind      524288        19 [ -3e+09, 3e+09]         97         22         28         22         75         30
i64_ind     1048576         9 [ -3e+09, 3e+09]        105         25         30         27         81         31
i64_ind     2097152         4 [ -3e+09, 3e+09]        112         27         33         33         85         34
i64_ind     4194304         2 [ -3e+09, 3e+09]        123         53         39         40         98         42
i64_ind     8388608         1 [ -3e+09, 3e+09]        129         61         47         47        108         49

type         length   samples            range   net-sort    lsd-rdx    msd-rdx   cav-sort   std-sort   ska-sort
flt_ind           8   1250000 [ -2e+19, 2e+19]         16         73         13         12         14         14
flt_ind          16    625000 [ -2e+19, 2e+19]         22         51         16         15         19         19
flt_ind          32    312500 [ -2e+19, 2e+19]         39         38         19         19         26         27
flt_ind          64    156250 [ -2e+19, 2e+19]         43         24         21         21         31         31
flt_ind         128     78125 [ -2e+19, 2e+19]         48         18         21         18         36         26
flt_ind         256     39062 [ -2e+19, 2e+19]         53         14         19         14         41         15
flt_ind         512     19531 [ -2e+19, 2e+19]         59         11         16         11         46         12
flt_ind        1024      9765 [ -2e+19, 2e+19]         64          9         16          9         51          9
flt_ind        2048      4882 [ -2e+19, 2e+19]         70          8         18          8         56          8
flt_ind        4096      2441 [ -2e+19, 2e+19]         76          8         19          8         61          7
flt_ind        8192      1220 [ -2e+19, 2e+19]         83          8         21          8         66          8
flt_ind       16384       610 [ -2e+19, 2e+19]         89          9         25          9         72          9
flt_ind       32768       305 [ -2e+19, 2e+19]         96          9         34          9         78          9
flt_ind       65536       152 [ -2e+19, 2e+19]        102          9         36          9         84          9
flt_ind      131072        76 [ -2e+19, 2e+19]        109          9         35          9         88          9
flt_ind      262144        38 [ -2e+19, 2e+19]        116         10         32         10         93         10
flt_ind      524288        19 [ -2e+19, 2e+19]        123         11         30         11        101         10
flt_ind     1048576         9 [ -2e+19, 2e+19]        130         12         29         11        108         11
flt_ind     2097152         4 [ -2e+19, 2e+19]        138         12         29         12        114         11
flt_ind     4194304         2 [ -2e+19, 2e+19]        148         14         32         14        121         13
flt_ind     8388608         1 [ -2e+19, 2e+19]        154         23         31         23        133         20

type         length   samples            range   net-sort    lsd-rdx    msd-rdx   cav-sort   std-sort   ska-sort
dbl_ind           8   1250000 [-1e+154,1e+154]         16        161         13         12         14         14
dbl_ind          16    625000 [-1e+154,1e+154]         22        111         16         15         20         19
dbl_ind          32    312500 [-1e+154,1e+154]         38        109         19         19         26         25
dbl_ind          64    156250 [-1e+154,1e+154]         42         58         25         25         31         30
dbl_ind         128     78125 [-1e+154,1e+154]         47         38         29         29         36         39
dbl_ind         256     39062 [-1e+154,1e+154]         52         29         20         29         41         40
dbl_ind         512     19531 [-1e+154,1e+154]         57         22         20         22         46         34
dbl_ind        1024      9765 [-1e+154,1e+154]         62         19         20         19         49         33
dbl_ind        2048      4882 [-1e+154,1e+154]         68         18         22         18         54         31
dbl_ind        4096      2441 [-1e+154,1e+154]         75         18         26         18         59         32
dbl_ind        8192      1220 [-1e+154,1e+154]         81         18         30         18         65         34
dbl_ind       16384       610 [-1e+154,1e+154]         87         18         30         18         70         31
dbl_ind       32768       305 [-1e+154,1e+154]         93         18         28         18         75         30
dbl_ind       65536       152 [-1e+154,1e+154]        100         19         26         19         80         30
dbl_ind      131072        76 [-1e+154,1e+154]        107         21         25         21         86         29
dbl_ind      262144        38 [-1e+154,1e+154]        114         23         26         24         94         31
dbl_ind      524288        19 [-1e+154,1e+154]        121         25         29         25        101         35
dbl_ind     1048576         9 [-1e+154,1e+154]        128         27         35         29        106         39
dbl_ind     2097152         4 [-1e+154,1e+154]        139         34         33         32        112         42
dbl_ind     4194304         2 [-1e+154,1e+154]        152         63         36         36        130         45
dbl_ind     8388608         1 [-1e+154,1e+154]        156         70         39         39        138         48
```
