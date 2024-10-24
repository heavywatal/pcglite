#include <pcglite/pcglite.hpp>

#include <iostream>
#include <random>

int main() {
    std::random_device seeder;
    pcglite::pcg32 rng(seeder());
    std::uniform_int_distribution<int> uniform(1, 6);
    for (int i = 0; i < 8; ++i) {
        std::cout << uniform(rng) << std::endl;
    }
    return 0;
}
