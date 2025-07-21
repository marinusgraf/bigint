#include "bigint.h"

bigint ZERO(0, 0);

bigint ONE(1, 1);

bigint::bigint() {}

bigint::bigint (const std::vector<uint8_t>& digits, int sign) : sign(sign) {
  for (size_t idx = 0; idx < digits.size(); ++idx) {
    assert(digits[idx] < radix);
  }
  this->digits = digits;
  remove_leading_zeros();
}

bigint::bigint(int64_t value) {
  if (value == 0) {
    *this = ZERO;
    return;
  } else if (value < 0) {
    value = -value;
    sign = -1;
  } else {
    sign = 1;
  }
  *this = bigint(value, sign);
}

bigint::bigint (uint64_t value, int sign) {
  if (value == 0 || sign == 0) {
    *this = ZERO;
  } else {
    sign = sign;
    digits = std::vector<uint8_t>(20, 0);
    for (size_t idx = 0; value != 0; ++idx) {
      digits[idx] = value % radix;
      value /= radix; 
    }
    *this = bigint(digits, sign);
    remove_leading_zeros();
  }
}

void bigint::remove_leading_zeros () {
  while (digits.size() > 0 and digits[digits.size() - 1] == 0) {
    digits.pop_back();
  }
  if (size() == 0)
    sign = 0;
}

uint8_t& bigint::operator[] (size_t idx) {
  return this->digits[idx];
} 

const uint8_t& bigint::operator[] (size_t idx) const {
  return this->digits[idx];
}

size_t bigint::size() const {
  return this->digits.size();
}

bigint& bigint::operator<<=(size_t n) {
  bigint& lhs = *this;
  if (n == 0 || lhs.sign == 0) 
    return lhs;
  
  std::vector<uint8_t> result(lhs.size() + n, 0);
  for (size_t i = 0; i < size(); ++i) {
    result[i + n] = lhs[i];
  }
  lhs = bigint(result, lhs.sign); 
  return lhs;
}

bigint& bigint::operator>>=(size_t n) {
  bigint& lhs = *this;
  if (n == 0 or lhs.sign == 0) 
    return lhs;
  if (n >= lhs.size()) {
    lhs = bigint(0, 0);
    return lhs;
  }
  std::vector<uint8_t> result(lhs.size() - n, 0);
  for (size_t i = 0; i < result.size(); ++i) {
    result[i] = lhs[i + n];
  }
  lhs = bigint(result, lhs.sign);
  return lhs;
}

bigint& bigint::operator+=(const bigint& rhs) {
  bigint& lhs = *this;
  if (lhs.sign == 0) {
    lhs = rhs;
    return lhs;
  }
  if (rhs.sign == 0)
    return lhs;
  if (lhs == -rhs) {
    lhs = ZERO;
    return lhs;
  }
  if (lhs.sign == 1 and rhs.sign == -1) {
    lhs = lhs - (-rhs);
    return lhs;
  }
  if (lhs.sign == -1 and rhs.sign == 1) {
    lhs = rhs - (-lhs);
    return lhs;
  }

  
  size_t max_size = std::max(lhs.size(), rhs.size());
  size_t min_size = std::min(lhs.size(), rhs.size());
  uint8_t carry = 0;
  size_t i;
  std::vector<uint8_t> result(max_size + 1, 0);

  for (i = 0; i < min_size; ++i) {
    result[i] = (lhs[i] + rhs[i] + carry) % radix;
    carry = lhs[i] + rhs[i] + carry >= radix ? 1 : 0;
  }
  lhs = lhs.size() >= rhs.size() ? lhs : rhs;
  for (; i < max_size; ++i) {
    result[i] = (lhs[i] + carry) % radix;
    carry = lhs[i] + carry >= radix ? 1 : 0;
  }
  if (carry == 1) {
    result[i] = 1;
  } 
  lhs = bigint(result, lhs.sign);
  return lhs;
}

bigint& bigint::operator-=(const bigint& rhs) {
  bigint& lhs = *this;
  if (lhs.sign == 0) {
    lhs = -rhs;
    return lhs; 
  }
  if (rhs.sign == 0)
    return lhs;
  if (lhs.sign != rhs.sign)
    return lhs += -rhs;
  if (lhs == rhs) {
    lhs = ZERO;
    return lhs; 
  }
  bigint minuend, subtrahend; 
  bool both_operands_positive = lhs.sign == 1 and rhs.sign == 1;
  bool both_operands_negative = lhs.sign == -1 and rhs.sign == -1;
  bool lhs_greater_rhs = lhs > rhs;
  if (lhs_greater_rhs) {
    minuend = lhs;
    subtrahend = rhs;
  } else {
    minuend = rhs;
    subtrahend = lhs;
  }
  std::vector<uint8_t> result(minuend.size(), 0); 
  size_t idx;
  uint8_t borrow = 0;
  for (idx = 0; idx < subtrahend.size(); ++idx) {
    if (minuend[idx] < subtrahend[idx] + borrow) {
      result[idx] = (minuend[idx] + radix) - (subtrahend[idx] + borrow);
      borrow = 1;
    } else {
      result[idx] = minuend[idx] - (subtrahend[idx] + borrow);
      borrow = 0;
    }
  }
  for (; idx < minuend.size(); ++idx) {
    if (minuend[idx] < borrow) {
      result[idx] = (minuend[idx] + radix) - borrow;
      borrow = 1;
    } else {
      result[idx] = minuend[idx] - borrow;
      borrow = 0;
    }
    assert(result[idx] < radix);
  }
  uint8_t result_sign = minuend.sign;
  if (lhs_greater_rhs and both_operands_negative) 
    result_sign = -sign;
  if (!lhs_greater_rhs and both_operands_positive)
    result_sign = -sign;
  lhs = bigint(result, result_sign);
  return lhs;
}

bigint& bigint::operator*=(const bigint& rhs) {
  bigint& lhs = *this;
  if (lhs.sign == 0 or rhs.sign == 0) {
    lhs = ZERO;
    return lhs;
  }
  if (lhs == ONE) {
    lhs = rhs;
    return lhs;
  }
  if (lhs == -ONE) {
    lhs = -rhs;
    return lhs;
  }
  if (rhs == ONE) {
    return lhs;
  }
  if (rhs == -ONE) {
    lhs = -lhs;
    return lhs;
  }
  if (lhs.size() == 1 and rhs.size() == 1) {
    lhs = bigint((uint16_t)rhs[0] * (uint64_t)lhs[0], rhs.sign * lhs.sign);
    return lhs;
  }
  size_t max_operand_size = std::max(lhs.size(), rhs.size());
  size_t next_even_number = max_operand_size % 2 == 0 ? max_operand_size : max_operand_size + 1;
  bigint lhs_lo, lhs_hi, rhs_lo, rhs_hi;
  split(lhs, next_even_number, lhs_lo, lhs_hi); 
  split(rhs, next_even_number, rhs_lo, rhs_hi); 
  bigint lhs_hi_rhs_hi = lhs_hi * rhs_hi;
  bigint lhs_lo_rhs_lo = lhs_lo * rhs_lo;
  bigint lhs_rhs_mixed = ((lhs_hi + lhs_lo) * (rhs_hi + rhs_lo)) - (lhs_hi_rhs_hi + lhs_lo_rhs_lo);
  lhs_hi_rhs_hi <<= next_even_number;
  lhs_rhs_mixed <<= (next_even_number >> 1); 
  lhs = lhs_hi_rhs_hi + lhs_rhs_mixed + lhs_lo_rhs_lo;
  return lhs;
}

void split(const bigint& x, size_t n, bigint& x_lo, bigint& x_hi) {
  size_t k = n / 2;
  if (x.size() > k) {
    std::vector<uint8_t> x_lo_digits(k, 0);
    std::vector<uint8_t> x_hi_digits(x.size() - k);
    for (size_t idx = 0; idx < k; ++idx) {
      x_lo_digits[idx] = x[idx];
      assert(x_lo_digits[idx] < radix);
    }
    for (size_t idx = 0; idx + k < x.size(); ++idx) {
      x_hi_digits[idx] = x[idx + k]; 
      assert(x_hi_digits[idx] < radix);
    }
    x_lo = bigint(x_lo_digits, x.sign);
    x_hi = bigint(x_hi_digits, x.sign);
  } else {
    x_lo = x;
    x_hi = ZERO;
    x_lo.remove_leading_zeros();
  }
}

bigint& bigint::operator/=(const bigint& rhs) {
  bigint& lhs = *this;
  bigint quotient, remainder;
  div(lhs, rhs, quotient, remainder);
  lhs = quotient;
  return lhs;
}

bigint& bigint::operator%=(const bigint& rhs) {
  bigint& lhs = *this;
  bigint quotient, remainder;
  uint16_t normalization_factor = div(lhs, rhs, quotient, remainder);
  remainder /= normalization_factor;
  lhs = remainder;
  return lhs;
}

uint16_t div(bigint lhs, bigint rhs, bigint& quotient, bigint& remainder) {
  if (lhs.sign == 0) {
    quotient = ZERO;
    remainder = ZERO;
    return 1;
  }
  if (lhs < rhs) {
    quotient = ZERO;
    remainder = rhs;
    return 1;
  }
  if (lhs == rhs) {
    quotient = ONE;
    remainder = ZERO;
    return 1;
  }
  int8_t sign = lhs.sign * rhs.sign;
  if (rhs.size() == 1 and lhs.size() == 1) {
   quotient = bigint(lhs[0] / rhs[0], sign); 
   remainder = bigint(lhs[0] %  rhs[0], sign);
   return 1;
  }
  if (rhs.size() == 1) {
    short_div(lhs, rhs, quotient, remainder);
    return 1;
  }
  uint16_t normalization_factor = 1;
  size_t lhs_size_pre_norm = lhs.size();
  if (rhs[rhs.size() - 1] < (radix >> 1)) {
    normalization_factor = radix / (rhs[rhs.size() - 1] + 1);
    rhs *= normalization_factor;
    lhs *= normalization_factor;
  }
  if (normalization_factor == 1 or (lhs.size() == rhs.size()))
    lhs.digits.push_back(0);
  size_t n = rhs.size();
  size_t m = lhs.size() - rhs.size() - 1;
  uint16_t divisor = rhs[n-1];
  uint16_t dividend;
  uint8_t q, r;
  std::vector<uint8_t> result(m + 1, 0);
  for (size_t j = m; j != -1; --j) {
    dividend = ((uint16_t)lhs[n + j] * radix) + (uint16_t)lhs[n + j - 1];
    q = dividend / divisor;
    r = dividend % divisor;
    while (q >= radix or ((q * rhs[n-2]) > ((radix * r) + lhs[n + j - 2]))) {
       q -= 1;
       r += rhs[n-1];
    }
    lhs -= (q * rhs) << j; 
    if (lhs.sign == -1) {
      --q;
      lhs += rhs;
    }
    result[j] = q;
    if (lhs == ZERO) break;
  }
  quotient = bigint(result, sign);
  remainder = lhs;
  return normalization_factor;
}

void short_div(bigint& lhs, bigint& rhs, bigint& quotient, bigint& remainder) {
  assert(rhs.size() == 1);
  uint8_t divisor = rhs[0];
  size_t n = lhs.size();
  std::vector<uint8_t> result(n, 0);
  uint64_t r = 0;
  uint16_t cur;
  for (size_t j = n - 1; j != -1; --j) {
    cur = (lhs[j] + (r * radix)); 
    result[j] = cur / divisor;
    r = cur % divisor; 
  }
  quotient = bigint(result, lhs.sign * rhs.sign);
  remainder = bigint(r, lhs.sign * rhs.sign);
}

bool operator==(const bigint& lhs, const bigint& rhs) {
  if (lhs.sign != rhs.sign)
    return false;
  if (lhs.size() != rhs.size()) 
    return false;
  for (size_t idx = 0; idx < lhs.size(); ++idx) {
    if (lhs[idx] != rhs[idx])
      return false;
  }
  return true;
}

bool operator!=(const bigint& lhs, const bigint& rhs) {
  return !(lhs == rhs);
}

std::strong_ordering operator<=>(const bigint& lhs, const bigint& rhs) {
  if (lhs.sign == 0 and rhs.sign == 0) 
    return std::strong_ordering::equivalent;
  if (lhs.sign != rhs.sign) 
    return lhs.sign <=> rhs.sign;
  
  bool both_operands_positive = lhs.sign == 1 and rhs.sign == 1;
  bool both_operands_negative = lhs.sign == -1 and rhs.sign == -1;

  if (lhs.size() != rhs.size()) {
    if (both_operands_positive)
      return lhs.size() <=> rhs.size();
    if (both_operands_negative)
      return rhs.size() <=> rhs.size();
  }
  if (lhs.size() == 1 and rhs.size() == 1)
      return lhs[0] <=> rhs[0];
  
  for (size_t i = lhs.size() - 1; i != -1; --i) {
    if (lhs[i] == rhs[i]) continue;
    if (both_operands_positive)
      return lhs[i] <=> rhs[i];
    if (both_operands_negative)
      return rhs[i] <=> lhs[i];
  }
  return std::strong_ordering::equivalent;
}

bigint operator>>(const bigint& lhs, size_t n) {
  bigint result = lhs;
  result >>= n;
  return result;
} 

bigint operator<<(const bigint& lhs, size_t n) {
  bigint result = lhs;
  result <<= n;
  return result;;
}

bigint operator+(const bigint& lhs, const bigint& rhs) {
  bigint result = lhs;
  result += rhs;
  return result;
}

bigint operator-(const bigint& rhs) {
  bigint result = rhs;
  result.sign *= -1;
  return result;
}

bigint operator-(const bigint& lhs, const bigint &rhs) {
  bigint result = lhs;
  result -= rhs;
  return result;
}

bigint operator*(const bigint& lhs, const bigint& rhs) {
  bigint result = lhs;
  result *= rhs;
  return result;
}

bigint operator/(const bigint& lhs, const bigint& rhs) {
  bigint result = lhs;
  result /= rhs;
  return result;
}

std::ostream& operator<<(std::ostream& os, const bigint& rhs)
{
    if (rhs.sign == 0) {
      os << 0 << std::endl;
      return os;
    }
    if (rhs.sign == -1) 
      os << "-";
    assert(rhs.size() >= 1);
    os << unsigned(rhs[rhs.size() - 1]);
    for (size_t idx = rhs.size() - 2; idx != -1; --idx) {
      os << std::format("{:02}", unsigned(rhs[idx]));
    }
    return os;
}
