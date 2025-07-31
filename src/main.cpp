#include "bigint.h"

int main() { //calculates 1000!
  bigint a = 1;
  for (uint64_t k = 2; k <= 1000; k++) {
    a *= k;
  }
  std::cout << a << "\n";
  return 0;
}

