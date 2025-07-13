#include "bigint.cpp"

int main() {
  bigint a(-1, 1);
  a >>= 64; 
  std::cout << std::hex << a[2] << "|" << a[1] << "|" << a[0] << std::endl;
}