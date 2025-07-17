#include "bigint.h"

bigint ZERO(0, 0);

bigint ONE(1, 1);

bigint::bigint() {}

bigint::bigint (const std::vector<uint64_t>& digits, int sign) : digits(digits), sign(sign) {}

bigint::bigint (uint64_t value, int sign) {
  if (value == 0 || sign == 0) {
    this->sign = 0;
    this->digits = std::vector<uint64_t>();
  } else {
    this->sign = sign;
    this->digits = std::vector<uint64_t>(1, value);
  }
}

void bigint::remove_leading_zeros () {
  while (digits.size() > 0 and digits[digits.size() - 1] == 0) {
    digits.pop_back();
  }
  if (size() == 0)
    sign = 0;
}

uint64_t& bigint::operator[] (size_t idx) {
  return this->digits[idx];
} 

const uint64_t& bigint::operator[] (size_t idx) const {
  return this->digits[idx];
}

size_t bigint::size() const {
  return this->digits.size();
}

bigint& bigint::operator<<=(const uint32_t n) {
  bigint& lhs = *this;
  if (n == 0 || lhs.sign == 0) 
    return lhs;
  
  size_t nb_64_shifts = n / 64;
  size_t nb_bit_shifts = n % 64;
  size_t new_bigint_size = lhs.size() + nb_64_shifts;
  bigint result{std::vector<uint64_t>{new_bigint_size + 1, 0}, 0};
  for (size_t i = 0; i < size(); ++i) {
    result[i + nb_64_shifts] = lhs[i];
  }
  if (nb_bit_shifts > 0) {
    uint64_t top_bits_mask = (uint64_t) -1 << (64 - nb_bit_shifts);
    for (size_t i = result.size() - 1; i > nb_64_shifts; --i) {
      result[i] |= (result[i - 1] & top_bits_mask) >> (64 - nb_bit_shifts);
      result[i - 1] <<= nb_bit_shifts;
    }
  }
  remove_leading_zeros();
  lhs = result;
  return lhs;
}


bigint& bigint::operator>>=(const uint32_t n) {
  bigint& lhs = *this;
  if (n == 0 or lhs.sign == 0) 
    return lhs;
  size_t nb_64_shifts = n / 64;
  if (nb_64_shifts >= lhs.size()) {
    lhs = bigint(0, 0);
    return lhs;
  }

  size_t nb_bit_shifts = n % 64;
  size_t new_bigint_size = lhs.size() - nb_64_shifts; 
  bigint result(std::vector<uint64_t>(new_bigint_size, 0), lhs.sign);
  
  for (size_t i = 0; i < result.size(); ++i) {
    result[i] = lhs[i + nb_64_shifts];
  }
  if (nb_bit_shifts > 0) {
    uint64_t bottom_bits_mask = (uint64_t) -1 >> (64 - nb_bit_shifts);
    size_t i = 0;
    for (; i < result.size() - 1; ++i) {
      result[i] >>= nb_bit_shifts;
      result[i] |= (result[i + 1] & bottom_bits_mask) << (64 - nb_bit_shifts);
    }
    result[i] >>= nb_bit_shifts;
  }
  remove_leading_zeros();
  lhs = result;
  return lhs;
}

bigint& bigint::operator+=(const bigint& rhs) {
  bigint& lhs = *this;
  if (lhs.sign == 0) {
    *this = rhs;
    return *this;
  }
  if (rhs.sign == 0)
    return *this;
  if (lhs == -rhs) {
    *this = ZERO;
    return *this;
  }
  if (lhs.sign != rhs.sign) 
    return lhs -= rhs;
  
  size_t max_size = std::max(lhs.size(), rhs.size());
  uint64_t carry = 0;
  size_t i;
  bigint result(std::vector<uint64_t>(max_size + 1, 0), sign);
  for (i = 0; i < max_size; ++i) {
    if (i >= lhs.size()) {
      result[i] = rhs[i] + carry;
      carry = result[i] < rhs[i] ? 1 : 0;
    } else if (i >= rhs.size()) {
      result[i] = lhs[i] + carry;
      carry = result[i] < lhs[i] ? 1 : 0;
    } else { 
      result[i] = lhs[i] + rhs[i] + carry;
      carry = result[i] < rhs[i] ? 1 : 0; //set carry if overflow    
    }
  }
  if (carry == 1) {
    result[i] = 1;
  } else {
    result.remove_leading_zeros();
  }
  lhs = result;
  return lhs; 
}

bigint& bigint::operator-=(const bigint& rhs) {
  bigint& lhs = *this;
  if (lhs.sign == 0) {
    *this = -rhs;
    return *this; 
  }
  if (rhs.sign == 0)
    return *this;
  if (lhs.sign != rhs.sign)
    return lhs += -rhs;
  if (lhs == rhs) {
    *this = ZERO;
    return *this; 
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
  bigint result(std::vector<uint64_t>(minuend.size() + 1), minuend.sign); 
  size_t idx;
  uint64_t borrow = 0;
  for (idx = 0; idx < subtrahend.size(); ++idx) {
    result[idx] = minuend[idx] - subtrahend[idx] - borrow;
    if (minuend[idx] < subtrahend[idx] or minuend[idx] < subtrahend[idx] + borrow) {
      borrow = 1;
    } else {
      borrow = 0;
    }
  }
  for (; idx < minuend.size(); ++idx) {
    result[idx] = minuend[idx] - borrow;
    if (minuend[idx] < borrow) {
      borrow = 1;
    } else {
      borrow = 0;
    }
  }
  if (lhs_greater_rhs and both_operands_positive) 
    *this = result;
  if (lhs_greater_rhs and both_operands_negative) 
    *this = -result;
  if (!lhs_greater_rhs and both_operands_positive)
    *this = -result;
  if (!lhs_greater_rhs and both_operands_negative)
    *this = result;
  remove_leading_zeros();
  return *this; 
}

bigint& bigint::operator*=(const bigint& rhs) {
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

  for (size_t i = lhs.size() - 1; i != -1; --i) {
    if (lhs[i] == rhs[i]) continue;
    if (both_operands_positive)
      return lhs[i] <=> rhs[i];
    if (both_operands_negative)
      return rhs[i] <=> lhs[i];
  }
  throw -1;
}

bigint operator>>(const bigint& lhs, const uint32_t n) {
  bigint result{lhs};
  result >>= n;
  return result;
} 

bigint operator<<(const bigint& lhs, const uint32_t n) {
  bigint result{lhs};
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

bigint operator-(bigint& lhs, bigint &rhs) {
  bigint result = lhs;
  result -= rhs;
  return result;
}

std::ostream& operator<<(std::ostream& os, const bigint& rhs)
{
    if (rhs.sign == 0) {
      os << 0 << std::endl;
      return os;
    }
    os << std::hex;
    if (rhs.sign == -1) 
      os << "-";
    for (size_t idx = rhs.size() - 1; idx >= 1; --idx) {
      os << rhs[idx] << "|";
    }
    os << rhs[0] << std::endl;
    return os;
}
