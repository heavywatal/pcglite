## pcglite

[![build](https://github.com/heavywatal/pcglite/actions/workflows/build.yaml/badge.svg)](https://github.com/heavywatal/pcglite/actions/workflows/build.yaml)

pcglite is a lightweight subset of [pcg-cpp](https://github.com/imneme/pcg-cpp),
the original C++ implementation of [PCG random number generator](https://www.pcg-random.org/).


### Features

- A template class `pcg_engine<UIntType>` supports `uint32_t` and `uint64_t`.
- Two aliases for specialized types: `pcglite::pcg32` and `pcglite::pcg64`.
- Compliant to `UniformRandomBitGenerator` requirements;
  usable with various distributions in `<random>` as a drop-in replacement of the standard generators such as `std::mt19937`.
- Single header.


### Dependency

- C++17 compiler
- `pcg64` requires `__uint128_t`.
  It is not in the C++ standard, but is available in most environments with Clang and GCC.


### Installation

Modern CMake can populate this package into your project with [FetchContent](https://cmake.org/cmake/help/latest/module/FetchContent.html) module.

Alternatively, you can get the source code from GitHub, and install it into your system with CMake:
```sh
git clone https://github.com/heavywatal/pcglite.git
cd pcglite/
cmake -S . -B build -DCMAKE_INSTALL_PREFIX=${HOME}/local
cmake --build build
cmake --install build
```

The easiest way for system-wide installation is to use [Homebrew](https://brew.sh/):
```sh
brew install heavywatal/tap/pcglite
```

Header files are installed to `${CMAKE_INSTALL_PREFIX}/include/pcglite/`.
This library can be imported from other CMake projects:
```cmake
find_package(pcglite)
target_link_libraries(${YOUR_TARGET} PRIVATE pcglite::pcglite)
```

Of course you can manually put `pcglite.hpp` anywhere you like.


### Usage

See `test/example.cpp`.
