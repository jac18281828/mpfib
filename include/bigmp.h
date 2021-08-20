#pragma once

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <ostream>
#include <vector>

namespace bigmp {

template <typename container_t = unsigned long, typename base_t = unsigned char> class BigInt {
public:
  using basevec_t = std::vector<base_t>;

private:
  constexpr static char IXZ = '0';
  constexpr static container_t BASE_MASK = 0xffU;
  constexpr static container_t BASE_SATURATION = BASE_MASK + 1U;
  constexpr static container_t BASE_SHIFT = 8;

  BigInt(basevec_t::const_iterator begin, basevec_t::const_iterator end) : m_basevec(begin, end) { trim(); }

public:
  explicit BigInt(const unsigned long r) { (void)set(r); }

  explicit BigInt(char const* cstr) : BigInt(std::string(cstr)) {}

  BigInt(std::string const& init_str) : BigInt() {
    auto add_each = [&](const auto d) {
      if ((d != '.') && (d != ',')) {
        *this *= 10;
        *this += (d - IXZ);
      }
    };
    std::for_each(std::begin(init_str), std::end(init_str), add_each);
  }

  BigInt() = default;

  BigInt(BigInt const& other) { m_basevec = other.m_basevec; }

  BigInt(BigInt&& other) { m_basevec = std::move(other.m_basevec); }

  BigInt& operator=(BigInt const& other) {
    m_basevec = other.m_basevec;
    return *this;
  }

  BigInt& operator=(BigInt&& other) {
    m_basevec = std::move(other.m_basevec);
    return *this;
  }

  BigInt& operator=(const unsigned long r) { return set(r); }

  BigInt& operator+=(const base_t d) {
    if (m_basevec.empty())
      m_basevec.push_back(0);
    auto iter = std::rbegin(m_basevec);
    container_t ireg = *iter + d;
    *iter = lobyte(ireg);
    iter++;

    while (iter != std::rend(m_basevec)) {
      ireg = *iter + hibyte(ireg);
      const auto loval = lobyte(ireg);
      *iter = loval;
      ++iter;
    }

    const auto hival = hibyte(ireg);
    if (hival != 0) {
      m_basevec.insert(std::begin(m_basevec), hival);
    }

    return *this;
  }

  BigInt operator+(BigInt const& other) const {
    BigInt result;
    auto& resvec = result.m_basevec;
    resvec.reserve(std::max(other.size(), size()) + 1);

    auto iter = std::rbegin(m_basevec);
    auto oter = std::rbegin(other.m_basevec);

    container_t ireg = 0;

    while (iter != std::rend(m_basevec) && oter != std::rend(other.m_basevec)) {
      ireg = *iter + *oter + hibyte(ireg);
      resvec.push_back(lobyte(ireg));
      ++iter;
      ++oter;
    }

    while (iter != std::rend(m_basevec)) {
      ireg = *iter + hibyte(ireg);
      resvec.push_back(lobyte(ireg));
      ++iter;
    }

    while (oter != std::rend(other.m_basevec)) {
      ireg = *oter + hibyte(ireg);
      resvec.push_back(lobyte(ireg));
      ++oter;
    }

    // dont push an empty value to the front if you dont have to
    const auto hival = hibyte(ireg);
    if (hival != 0) {
      resvec.push_back(hival);
    }

    std::reverse(std::begin(resvec), std::end(resvec));

    return result;
  }

  BigInt operator+(const base_t d) const { return *this + BigInt(d); }

  // non negative subtraction!
  BigInt operator-(BigInt const& other) const {
    BigInt result;
    auto& resvec = result.m_basevec;
    const auto p = std::min(other.size(), size());
    resvec.reserve(std::max(other.size(), size()));

    container_t ireg = BASE_SATURATION;

    auto iter = std::rbegin(m_basevec);
    auto oter = std::rbegin(other.m_basevec);

    while (iter != std::rend(m_basevec) && oter != std::rend(other.m_basevec)) {
      ireg = BASE_MASK + *iter - *oter + hibyte(ireg);
      resvec.insert(std::begin(resvec), lobyte(ireg));
      ++iter;
      ++oter;
    }

    // keep on rolling!
    while (iter != std::rend(m_basevec)) {
      ireg = BASE_MASK + *iter + hibyte(ireg);
      resvec.insert(std::begin(resvec), lobyte(ireg));
      ++iter;
    }

    // check results
    const auto sign = hibyte(ireg);

    if (sign == 0UL || oter != std::rend(other.m_basevec)) {
      throw std::runtime_error("negative result");
    }

    result.trim();
    return result;
  }

  BigInt& operator*=(const base_t iv) {
    if (m_basevec.empty())
      m_basevec.push_back(0);
    container_t ireg = 0;
    auto riter = std::rbegin(m_basevec);
    while (riter != std::rend(m_basevec)) {
      ireg = *riter * iv + hibyte(ireg);
      const auto loval = lobyte(ireg);
      *riter = loval;
      ++riter;
    }
    const auto hival = hibyte(ireg);
    if (hival != 0) {
      m_basevec.insert(std::begin(m_basevec), hival);
    }
    // else zero times iv still equal zero
    return *this;
  }

  BigInt operator*(const base_t iv) const {
    container_t ireg = 0;
    BigInt result;
    auto& resvec = result.m_basevec;
    resvec.reserve(m_basevec.size() + 1);
    auto riter = std::rbegin(m_basevec);
    while (riter != std::rend(m_basevec)) {
      ireg = *riter * iv + hibyte(ireg);
      resvec.insert(std::begin(resvec), lobyte(ireg));
      ++riter;
    }

    const auto hival = hibyte(ireg);
    if (hival != 0) {
      resvec.insert(std::begin(resvec), hibyte(ireg));
    }
    result.trim();
    return result;
  }

  // karatsuba recursive - middle school math
  BigInt operator*(BigInt const& other) const {
    const auto length = std::max(size(), other.size());
    if (length == 1) {
      const container_t tailval = m_basevec[0] * other.m_basevec[0];
      return BigInt(tailval);
    }

    auto left = m_basevec;
    while (left.size() < length) {
      left.insert(std::begin(left), base_t(0));
    }
    const auto lmid = left.size() / 2;
    const auto l0 = BigInt(std::cbegin(left), std::cbegin(left) + lmid);
    const auto l1 = BigInt(std::cbegin(left) + lmid, std::cend(left));
    auto right = other.m_basevec;
    while (right.size() < length) {
      right.insert(std::begin(right), base_t(0));
    }
    const auto rmid = right.size() / 2;
    const auto r0 = BigInt(std::cbegin(right), std::cbegin(right) + rmid);
    const auto r1 = BigInt(std::cbegin(right) + rmid, std::cend(right));

    auto p0 = l0 * r0;
    const auto p1 = l1 * r1;
    const auto p2 = (l0 + l1) * (r0 + r1);
    auto p3 = p2 - (p0 + p1);

    for (auto i = 0; i < 2 * (length - length / 2); i++)
      p0.m_basevec.push_back(0);
    for (auto i = 0; i < length - length / 2; i++)
      p3.m_basevec.push_back(0);

    auto result = (p0 + p1) + p3;
    result.trim();
    return result;
  }

  bool operator==(BigInt const& other) const {
    auto iter = std::cbegin(m_basevec);
    auto oter = std::cbegin(other.m_basevec);
    while (iter != std::cend(m_basevec) && oter != std::cend(other.m_basevec)) {
      if (*iter != *oter) {
        if (*iter == 0) {
          ++iter;
        } else if (*oter == 0) {
          ++oter;
        } else {
          return false;
        }
      }
      ++iter;
      ++oter;
    }
    while (iter != std::cend(m_basevec)) {
      if (*iter != 0)
        return false;
      ++iter;
    }
    while (oter != std::cend(other.m_basevec)) {
      if (*oter != 0)
        return false;
      ++oter;
    }
    return true;
  }

  bool operator==(const base_t value) const {
    auto iter = std::rbegin(m_basevec);
    if (iter != std::rend(m_basevec)) {
      if (value == *iter) {
        ++iter;
      } else {
        return false;
      }
      while (iter != std::rend(m_basevec)) {
        if (*iter != 0)
          return false;
        ++iter;
      }
      return true;
    }
    return false;
  }

  BigInt& operator/=(const base_t div) {
    container_t j, remainder{0};
    auto iter = std::begin(m_basevec);
    while (iter != std::end(m_basevec)) {
      j = BASE_SATURATION * remainder + *iter;
      *iter = static_cast<base_t>(j / div);
      remainder = j % div;
      ++iter;
    }
    return *this;
  }

  base_t operator%(const container_t modulus) const {
    if (empty())
      return 0;
    container_t j, remainder{0};
    auto iter = std::begin(m_basevec);
    while (iter != std::end(m_basevec)) {
      j = BASE_SATURATION * remainder + *iter;
      remainder = j % modulus;
      ++iter;
    }
    return remainder;
  }

  bool is_zero() const { return !is_nonzero(); }

  bool is_nonzero() const {
    for (const auto d : m_basevec) {
      if (d != 0)
        return true;
    }
    return false;
  }

  base_t operator[](const int n) const { return m_basevec[n]; }

  size_t size() const { return m_basevec.size(); }

  bool empty() const { return m_basevec.size() == 0; }

  void lsh() { m_basevec.push_back(0); }

  base_t rsh() {
    if (!m_basevec.empty()) {
      auto right = m_basevec.back();
      m_basevec.pop_back();
      return right;
    }
    return 0;
  }

  void trim() {
    auto iter = std::begin(m_basevec);
    while (iter != std::end(m_basevec) && *iter == 0 && size() > 1) {
      iter = m_basevec.erase(iter);
    }
  }

  template <typename container, typename base>
  friend std::ostream& operator<<(std::ostream& os, BigInt<container, base> const& bigin);

private:
  basevec_t m_basevec;

  void push_front(const base_t val) {
    auto front = std::cbegin(m_basevec);
    m_basevec.insert(front, val);
  }

  BigInt& set(unsigned long r) {
    m_basevec.clear();
    while (r > 0) {
      const auto bottom = r & BASE_MASK;
      m_basevec.insert(std::cbegin(m_basevec), bottom);
      r >>= BASE_SHIFT;
    }
    return *this;
  }

  template <typename T> int sgn(T val) { return (T(0) < val) - (val < T(0)); }

  constexpr static container_t lobyte(container_t x) {
    const auto val = x & BASE_MASK;
    return val;
  }

  constexpr static container_t hibyte(container_t x) {
    const auto val = (x >> BASE_SHIFT) & BASE_MASK;
    return val;
  }
};

template <typename container_t, typename base_t>
std::ostream& operator<<(std::ostream& out, BigInt<container_t, base_t> const& bigin) {
  if (bigin.is_zero()) {
    out << '0';
  } else {
    const auto IAZ = '0';
    std::vector<char> cvec;
    auto copy = bigin;
    while (!copy.is_zero()) {
      const auto d = copy % 10;
      cvec.push_back(static_cast<char>(IAZ + d));
      copy /= 10;
    }
    auto out_d = [&out](const auto d) { out << d; };
    // reverse it
    std::for_each(cvec.rbegin(), cvec.rend(), out_d);
    cvec.clear();
  }

  return out;
}

} // namespace bigmp
