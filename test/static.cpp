#include <pcglite/pcglite.hpp>

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
    // static_assert(std::is_swappable_v<T>);
}

}

int main() {
    test_static<pcglite::pcg32>();
    test_static<pcglite::pcg64>();
    return 0;
}
