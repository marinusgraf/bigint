#include <iostream>
#include <vector>
#include <cstdio>
#include <cstdint>

struct bigint {
  std::vector<uint64_t> digits; 
  int sign; // 1 = positive, 0 = zero, -1 = negative

  bigint() {}

  bigint (const std::vector<uint64_t>& digits, int sign) : digits(digits), sign(sign) {}

  bigint (uint64_t value, int sign) {
    if (value == 0 || sign == 0) {
      sign = 0;
      digits = std::vector<uint64_t>();
    } else {
      this->sign = sign;
      digits = std::vector<uint64_t>(1, value);
    }
  }

  void remove_leading_zeros () {
    while (digits.size() > 0 and digits[digits.size() - 1] == 0) {
      digits.pop_back();
    }
    if (size() == 0)
      sign = 0;
  }

  uint64_t& operator[] (size_t idx) {
    return this->digits[idx];
  } 

  const uint64_t& operator[] (size_t idx) const {
    return this->digits[idx];
  }

  size_t size() const {
    return this->digits.size();
  }

  friend bool operator<(const bigint& l, const bigint& r) {
    if (l.sign == 0) 
      return r.sign == 1;
    if (r.sign == 0) 
      return l.sign == -1;
    if (l.sign * r.sign == -1) //l,r have opposite sign
      return l.sign == -1; 
    
    if (l.digits.size() != r.digits.size()) {

      if (l.sign == -1 && r.sign == -1) //both negative
        return !(l.digits.size() < r.digits.size());
      
      if (l.sign == 1 && r.sign == 1) //both positive
        return l.digits.size() < r.digits.size();
    }

    if (l.digits.size() == r.digits.size()) {
      for (size_t i = l.digits.size() - 1; i != -1; --i) {
        if (l[i] != r[i] && l.sign == -1) { //both negative
          return !(l[i] < r[i]);
        }
        if (l[i] != r[i] && l.sign == 1) { //both positive
          return l[i] < r[i];
        }
      }
    } 
    return false; //r==l 
  }
  
  friend bool operator>(const bigint& l, const bigint& r) {
    return r < l;
  }
  
  friend bool operator<=(const bigint& l, const bigint& r) {
    return !(l > r);
  }
  
  friend bool operator>=(const bigint& l, const bigint& r) {
    return !(l < r);
  }

  friend bool operator==(const bigint& l, const bigint& r) {
    return (l <= r) && (r <= l);
  }
  
  friend bool operator==(const bigint& l, const uint64_t& r) {
    if (l.size() == 0) 
      return r == 0 and l.sign == 0;
    if (r == 0) 
      return l.size() == 0 and l.sign == 0; 
    if (l.size() == 1) 
      return l[0] == r;
    if (l[0] != r) {
      return false;
    }
    for (size_t i = 1; i < l.size(); ++i) {
      if (l[i] != 0) {
        return false;
      }
    }
    return true;
  }

  friend bool operator!=(const bigint& l, const bigint& r) {
    return !(l == r); 
  }

  bigint& operator<<=(uint32_t n) {
    if (n == 0 || *this == 0) 
      return *this;
    
    size_t nb_64_shifts = n / 64;
    size_t nb_bit_shifts = n % 64;
    size_t new_bigint_size = this->size() + nb_64_shifts;
    bigint result(std::vector<uint64_t>(new_bigint_size + 1, 0), this->sign);
    for (size_t i = 0; i < size(); ++i) {
      result[i + nb_64_shifts] = (*this)[i];
    }
    if (nb_bit_shifts > 0) {
      uint64_t top_bits_mask = (uint64_t) -1 << (64 - nb_bit_shifts);
      for (size_t i = result.size() - 1; i > nb_64_shifts; --i) {
        result[i] |= (result[i - 1] & top_bits_mask) >> (64 - nb_bit_shifts);
        result[i - 1] <<= nb_bit_shifts;
     }
    }
  remove_leading_zeros();
  *this = result;
  return *this;
  }
  
  friend bigint operator<<(bigint& l, uint32_t n) {
    l <<= n;
    return l;
  }

  bigint& operator>>=(uint32_t n) {
    if (n == 0 or *this == 0) 
      return *this;
    size_t nb_64_shifts = n / 64;
    if (nb_64_shifts >= this->size()) {
      *this = bigint(0, 0);
      return *this;
    }

    size_t nb_bit_shifts = n % 64;
    size_t new_bigint_size = this->size() - nb_64_shifts; 
    bigint result(std::vector<uint64_t>(new_bigint_size, 0), this->sign);
    
    for (size_t i = 0; i < result.size(); ++i) {
      result[i] = (*this)[i + nb_64_shifts];
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
    *this = result;
    return *this;
  }
  friend bigint operator>>(bigint& l, uint32_t n) {
    l <<= n;
    return l;
  }

  bigint& operator+=(bigint& r) {
    if (sign == 0)
      return r;
    if (r.sign == 0)
      return *this;
    if (sign == 1 and r.sign == -1)
      return *this -= r;
    if (sign == -1 and r.sign == 1)
      return r -= *this;
    size_t max_size = std::max(size(), r.size());
    uint64_t carry = 0;
    size_t i;
    bigint result(std::vector<uint64_t>(max_size + 1, 0), sign);
    for (i = 0; i < max_size; ++i) {
      if (i >= size()) {
        result[i] = r[i] + carry;
        carry = result[i] < r[i] ? 1 : 0;
      } else if (i >= r.size()) {
        result[i] = (*this)[i] + carry;
        carry = result[i] < (*this)[i] ? 1 : 0;
      } else { 
        result[i] = (*this)[i] + r[i] + carry;
        carry = result[i] < r[i] ? 1 : 0; //set carry if overflow    
      }
    }
    if (carry == 1) {
      result[i] = 1;
    } else {
      result.digits.pop_back();
    }
    *this = result;
    return *this; 
  }

  friend bigint operator+(bigint& l, bigint& r) {} 

  bigint& operator-=(bigint& r) {}

  friend bigint operator-(bigint& l, bigint &r) {}

};


const bigint ZERO(std::vector<uint64_t>(), 0);

const bigint ONE(std::vector<uint64_t>(1, 1), 1);
