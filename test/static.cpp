#include <pcglite/pcglite.hpp>

#include <limits>
#include <type_traits>

namespace {

template <class T> inline
void test_static() {
    static_assert(std::is_trivially_copyable_v<T>);
    static_assert(std::is_standard_layout_v<T>);
    static_assert(std::is_nothrow_default_constructible_v<T>);
    static_assert(std::is_nothrow_copy_constructible_v<T>);
    static_assert(std::is_nothrow_move_constructible_v<T>);
    static_assert(std::is_nothrow_destructible_v<T>);
    static_assert(T::min() == typename T::result_type{});
    static_assert(T::max() == std::numeric_limits<typename T::result_type>::max());
    // static_assert(std::is_swappable_v<T>);
}

}

int main() {
    test_static<pcglite::pcg32>();
    test_static<pcglite::pcg64>();
    return 0;
}
