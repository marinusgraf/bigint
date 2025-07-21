#include <vector>
#include <compare>
#include <cstdint>
#include <iostream>
#include <cstdio>
#include <cassert>
#include <iomanip>

#define radix (uint8_t)100

struct bigint {
  std::vector<uint8_t> digits;
  int8_t sign; // 1 = positive, 0 = zero, -1 = negative

  bigint();

  bigint (const std::vector<uint8_t>&, int sign);

  bigint(int64_t value);

  bigint (uint64_t value, int sign);
 
  void remove_leading_zeros (); 

  uint8_t& operator[] (size_t idx);

  const uint8_t& operator[] (size_t idx) const;

  size_t size() const;

  bigint& operator<<=(size_t n);
  
  bigint& operator>>=(size_t n);
  
  bigint& operator+=(const bigint& rhs);

  bigint& operator-=(const bigint& rhs); 

  bigint& operator*=(const bigint& rhs);

  bigint& operator/=(const bigint& rhs);

  bigint& operator%=(const bigint& rhs);

};

bool operator==(const bigint& lhs, const bigint& rhs);

bool operator!=(const bigint& lhs, const bigint& rhs); 

std::strong_ordering operator<=>(const bigint& lhs, const bigint& rhs); 

bigint operator>>(const bigint& lhs, size_t n);

bigint operator<<(const bigint& lhs, size_t n); 

bigint operator-(const bigint& rhs);

bigint operator+(const bigint& lhs, const bigint& rhs);

bigint operator-(const bigint& lhs, const bigint& rhs); 

bigint operator*(const bigint& lhs, const bigint& rhs);

bigint operator/(const bigint& lhs, const bigint& rhs);

std::ostream& operator<<(std::ostream& os, const bigint& rhs);

void split(const bigint& x, size_t n, bigint& x_lo, bigint& x_hi); 

uint16_t div(bigint lhs, bigint rhs, bigint& quotient, bigint& remainder);

void short_div(bigint& lhs, bigint& rhs, bigint& quotient, bigint& remainder);
