#include <pcglite/pcglite.hpp>

#include <iostream>
#include <sstream>
#include <random>

inline int test_uint128_stream() {
    using pcglite::operator<<;
    using pcglite::operator>>;
    __uint128_t x{42u}, z{};
    auto y = pcglite::detail::constexpr_uint128(42u, 54u);
    std::stringstream sst;
    sst << x;
    sst >> z;
    std::cout << x << std::endl;
    std::cout << z << std::endl;
    if (x != z) return 1;
    sst.clear();
    sst << y;
    sst >> z;
    std::cout << y << std::endl;
    std::cout << z << std::endl;
    if (y != z) return 1;
    return 0;
}

template <class URBG> inline
int test_rng_stream(URBG rng) {
    URBG copy(rng);
    if (rng != copy) return 1;
    std::cout << rng << std::endl;
    rng.operator()();
    std::cout << rng << std::endl;
    if (rng == copy) return 1;
    std::stringstream sst;
    sst << copy;
    sst >> rng;
    std::cout << rng << std::endl;
    if (rng != copy) return 1;
    return 0;
}

int main() {
    int ret = 0;
    ret |= test_uint128_stream();
    ret |= test_rng_stream(std::minstd_rand{});
    ret |= test_rng_stream(pcglite::pcg32{});
    ret |= test_rng_stream(pcglite::pcg64{});
    return ret;
}
