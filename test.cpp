#include "bigint.h"

int main() {
    bigint a({0, 0, 1}, 1);
    bigint b({1, (uint64_t)-1}, 1);
    std::cout << a - b;
 }