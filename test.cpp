#include "bigint.cpp"

int main() {
    bigint a(-1, 1);
    bigint b(1, 1);
    std::cout << std::hex << a[1] << "|" << a[0] << std::endl;
    a += b;
    std::cout << std::hex << a[1] << "|" << a[0] << std::endl;
}