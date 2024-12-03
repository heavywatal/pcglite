#include <pcglite/pcglite.hpp>
#include <pcg/pcg_random.hpp>

#include <iostream>
#include <random>
#include <type_traits>

template <class UIntType>
struct PCG {
    using lite = pcglite::permuted_congruential_engine<UIntType>;
    using itype = typename lite::state_type;
    using cpp = std::conditional_t<
      std::is_same_v<UIntType, uint32_t>,
      pcg_detail::setseq_base<UIntType, itype, pcg_detail::xsh_rr_mixin>,
      pcg_detail::setseq_base<UIntType, itype, pcg_detail::xsl_rr_mixin>
    >;
};

template <class T> inline
void test_static() {
    static_assert(PCG<T>::lite::min() == PCG<T>::cpp::min());
    static_assert(PCG<T>::lite::max() == PCG<T>::cpp::max());
    static_assert(PCG<T>::lite::multiplier == pcg_detail::default_multiplier<typename PCG<T>::lite::state_type>::multiplier());
}

template <class Lite, class Cpp>
int test_eq(Lite& origin, Cpp& upstream, unsigned n = 3u) {
    int ret = 0;
    std::cout << origin << std::endl;
    std::cout << upstream << std::endl;
    for (unsigned i=0u; i<n; ++i) {
        const auto expected = upstream();
        const auto observed = origin();
        if (observed != expected) {
            std::cout << observed << " != " << expected << "\n";
            ret = 1;
        }
    }
    return ret;
}

template <class T> inline
int ctor0() {
    typename PCG<T>::lite origin;
    typename PCG<T>::cpp upstream;
    return test_eq(origin, upstream);
}

template <class T> inline
int ctor1() {
    const auto seed{42u};
    typename PCG<T>::lite origin{seed};
    typename PCG<T>::cpp upstream{seed};
    return test_eq(origin, upstream);
}

template <class T> inline
int ctor2() {
    const auto seed{42u}, stream{54u};
    typename PCG<T>::lite origin{seed, stream};
    typename PCG<T>::cpp upstream{seed, stream};
    return test_eq(origin, upstream);
}

template <class T> inline
int seed1() {
    const auto seed{42u};
    typename PCG<T>::lite origin;
    typename PCG<T>::cpp upstream;
    origin.seed(42u);
    upstream.seed(42u);
    return test_eq(origin, upstream);
}

template <class T> inline
int seed2() {
    const auto seed{42u}, stream{54u};
    typename PCG<T>::lite origin;
    typename PCG<T>::cpp upstream;
    origin.seed(seed, stream);
    upstream.seed(seed, stream);
    return test_eq(origin, upstream);
}

template <class T> inline
int seedseq() {
    std::seed_seq sseq{42u, 54u};
    typename PCG<T>::lite origin;
    typename PCG<T>::cpp upstream;
    origin.seed(sseq);
    upstream.seed(sseq);
    return test_eq(origin, upstream);
}

int main() {
    test_static<uint32_t>();
    test_static<uint64_t>();
    int ret = 0;
    ret |= ctor0<uint32_t>();
    ret |= ctor0<uint64_t>();
    ret |= ctor1<uint32_t>();
    ret |= ctor1<uint64_t>();
    ret |= ctor2<uint32_t>();
    ret |= ctor2<uint64_t>();
    ret |= seed1<uint32_t>();
    ret |= seed1<uint64_t>();
    ret |= seed2<uint32_t>();
    ret |= seed2<uint64_t>();
    ret |= seedseq<uint32_t>();
    ret |= seedseq<uint64_t>();
    return ret;
}
