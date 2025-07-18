#include <vector>
#include <compare>
#include <cstdint>
#include <iostream>
#include <cstdio>

struct bigint {
  std::vector<uint64_t> digits; 
  int sign; // 1 = positive, 0 = zero, -1 = negative

  bigint();

  bigint (const std::vector<uint64_t>&, int);

  bigint(int64_t value);

  bigint (uint64_t value, int sign);
 
  void remove_leading_zeros (); 

  uint64_t& operator[] (size_t idx);

  const uint64_t& operator[] (size_t idx) const;

  size_t size() const;

  bigint& operator<<=(const uint32_t n);
  
  bigint& operator>>=(const uint32_t n);
  
  bigint& operator+=(const bigint& rhs);

  bigint& operator-=(const bigint& rhs); 

  bigint& operator*=(const bigint& rhs);

};

bool operator==(const bigint& lhs, const bigint& rhs);

bool operator!=(const bigint& lhs, const bigint& rhs); 

std::strong_ordering operator<=>(const bigint& lhs, const bigint& rhs); 

bigint operator>>(const bigint& lhs, const uint32_t n);

bigint operator<<(const bigint& lhs, const uint32_t n); 

bigint operator-(const bigint& rhs);

bigint operator+(const bigint& lhs, const bigint& rhs);

bigint operator-(const bigint& lhs, const bigint& rhs); 

bigint operator*(const bigint& lhs, const bigint& rhs);

std::ostream& operator<<(std::ostream& os, const bigint& rhs);

bigint mul(uint64_t lhs, uint64_t rhs, int result_sign);

void split(const bigint& x, size_t n, bigint& x_lo, bigint& x_hi); 