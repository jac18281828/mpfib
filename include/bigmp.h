#pragma once

#include <iostream>
#include <ostream>
#include <vector>
#include <algorithm>
#include <cstdint>

#include "nr.h"

namespace bigmp {

    template<typename container_t=uint32_t, typename base_t=uint8_t>
    class BigInt {
        constexpr static char IXZ = '0';
        constexpr static container_t MASK = 0xffU;
        constexpr static container_t SHIFT = 8;
    public:

        explicit BigInt(const unsigned long r) {
            (void)set(r);
        }

        explicit BigInt(char const* cstr) : BigInt(std::string(cstr)) {}

        BigInt(std::string const& init_str) : BigInt() {
            auto add_each = [&](const auto d) { 
                if((d != '.') && (d != ',')) {
                    *this *= 10;
                    *this += (d - IXZ);
                }
            };
            std::for_each(std::begin(init_str), std::end(init_str), add_each);
        }

        BigInt() = default;

        BigInt(BigInt const& other) {
            m_basevec = other.m_basevec;
        }

        BigInt(BigInt&& other) {
            m_basevec = std::move(other.m_basevec);
        }

        BigInt& operator=(BigInt const& other) {
            m_basevec = other.m_basevec;
            return *this;
        }

        BigInt& operator=(BigInt&& other) {
            m_basevec = std::move(other.m_basevec);
            return *this;
        }

        BigInt& operator=(const unsigned long r) {
            return set(r);
        }


        BigInt& operator+=(const base_t d) { 
            if(m_basevec.empty()) m_basevec.push_back(0);
            auto iter = std::rbegin(m_basevec);
            container_t ireg = *iter + d;
            *iter = lobyte(ireg);
            iter++;

            while(iter != std::rend(m_basevec)) {
                ireg = *iter + hibyte(ireg);
                const auto loval = lobyte(ireg);
                *iter = loval;
                ++iter;
            }

            const auto hival = hibyte(ireg);
            if(hival != 0) {
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

            while(iter != std::rend(m_basevec) && oter != std::rend(other.m_basevec)) {
                ireg = *iter + *oter + hibyte(ireg);
                resvec.push_back(lobyte(ireg));
                ++iter; ++oter;
            }

            while(iter != std::rend(m_basevec)) {
                ireg = *iter + hibyte(ireg);
                resvec.push_back(lobyte(ireg));
                ++iter;
            }
        
            while(oter != std::rend(other.m_basevec)) {
                ireg = *oter + hibyte(ireg);
                resvec.push_back(lobyte(ireg));
                ++oter;
            }

            // dont push an empty value to the front if you dont have to
            const auto hival = hibyte(ireg);
            if(hival != 0) {
                resvec.push_back(hival);
            }

            std::reverse(std::begin(resvec), std::end(resvec));

            return result;
        }

        BigInt operator+(const base_t d) const {
            return *this + BigInt(d);
        }

        BigInt& operator*=(const base_t iv) {
            if(m_basevec.empty()) m_basevec.push_back(0);
            container_t ireg = 0;
            auto riter = std::rbegin(m_basevec);
            while(riter != std::rend(m_basevec)) {
                ireg = *riter * iv + hibyte(ireg);
                const auto loval = lobyte(ireg);
                *riter = loval;
                ++riter;
            }
            const auto hival = hibyte(ireg);
            if(hival != 0) {
                m_basevec.insert(std::begin(m_basevec), hival);
            }
            // else zero times iv still equal zero
            return *this;
        }

        BigInt operator*(const base_t iv) const {
            container_t ireg = 0;
            BigInt result;
            auto& resvec = result.m_basevec;
            resvec.reserve(m_basevec.size()+1);
            auto riter = std::rbegin(m_basevec);
            while(riter != std::rend(m_basevec)) {
                ireg = *riter * iv + hibyte(ireg);
                resvec.insert(std::begin(resvec), lobyte(ireg));
                ++riter;
            }

            const auto hival = hibyte(ireg);
            if(hival != 0) {
                resvec.insert(std::begin(resvec), hibyte(ireg));
            }
            return result;
        }

        BigInt operator*(BigInt const& other) const {
            BigInt result;
            auto& resvec = result.m_basevec;

            constexpr double RX=256.0;
            int j, nn=1;
            double cy, t = 0.0;

            const int n = m_basevec.size();
            const int m = other.m_basevec.size();
            const int p = n + m - 1;
            resvec.reserve(p);
            const auto n_max = std::max(m,n);
            while(nn < n_max) nn <<= 1;
            nn <<= 1;
            std::vector<double> a(nn, 0.0);
            for(j=0; j<n; j++) a[j] = m_basevec[j];
            nr::realft(a, 1);
            std::vector<double> b(nn, 0.0);
            for(j=0; j<n; j++) b[j] = other.m_basevec[j];
            nr::realft(b, 1);
            b[0] *= a[0];
            b[1] *= a[1];
            for(j=2; j<nn; j+=2) {
                t=b[j];
                b[j] = t*a[j]-b[j+1]*a[j+1];
                b[j+1]=t*a[j+1]+b[j+1]*a[j];
            }
            nr::realft(b, -1);
            cy=0.0;
            for(j=nn-1;j>=0;--j) {
                t=b[j]/(nn >> 1)+cy+0.5;
                cy=static_cast<unsigned long>(t/RX);
                b[j]=t-cy*RX;
            }
            if(cy >= RX) 
                throw std::runtime_error("Cannot happen in mul");
            for(j=0;j<p;j++) result.m_basevec.push_back(0);
            result.m_basevec[0] = static_cast<base_t>(cy);
            const auto min_nmp = std::min(n+m,p);
            for(j=1;j<min_nmp;++j) {
                result.m_basevec[j] = static_cast<base_t>(b[j-1]);
            }

            return result;
        }

        bool operator==(BigInt const& other) const {
            auto iter = std::cbegin(m_basevec);
            auto oter = std::cbegin(other.m_basevec);
            while(iter != std::cend(m_basevec) && oter != std::cend(other.m_basevec)) {
                if(*iter != *oter) {
                    if(*iter == 0) {
                        ++iter;
                    } else if(*oter == 0) {
                        ++oter;
                    } else {
                        return false;
                    }
                }
                ++iter; ++oter;
            }
            while(iter != std::cend(m_basevec)) {
                if(*iter != 0) return false;
                ++iter;
            }
            while(oter != std::cend(other.m_basevec)) {
                if(*oter != 0) return false;
                ++oter;
            }
            return true;
        }

        bool operator==(const base_t value) const {
            auto iter = std::rbegin(m_basevec);
            if(iter != std::rend(m_basevec)) {
                if(value == *iter) {
                    ++iter;
                } else {
                    return false;
                }
                while(iter != std::rend(m_basevec)) {
                    if(*iter != 0) return false;
                    ++iter;
                }
                return true;
            }
            return false;
        }

        BigInt &operator/=(const container_t div) {
            container_t j, remainder{0};
            auto iter = std::begin(m_basevec);
            while(iter != std::end(m_basevec)) {
                j = 256U * remainder + *iter;
                *iter = static_cast<base_t>(j / div);
                remainder = j % div;
                ++iter;
            }
            return *this;
        }

        base_t operator%(const base_t modulus) const {
            if(m_basevec.empty()) return 0;
            container_t j, remainder{0};
            auto iter = std::begin(m_basevec);
            while(iter != std::end(m_basevec)) {            
                j = 256U * remainder + *iter;
                remainder = j % modulus;
                ++iter;
            }
            return remainder;
        }

        bool is_zero() const {
            return !is_nonzero();
        }

        bool is_nonzero() const {
            for(const auto d : m_basevec) {
                if(d != 0) return true;
            }
            return false;
        }

        base_t operator[](const int n) const {
            return m_basevec[n];
        }

        size_t size() const {
            return m_basevec.size();
        }

        bool empty() const {
            return m_basevec.empty();
        }

        void lsh() {
            m_basevec.push_back(0);
        }

        base_t rsh() {
            if(!m_basevec.empty()) {
                auto right = m_basevec.back();
                m_basevec.pop_back();
                return right;
            }
            return 0;
        }

        void trim() {
            while(!m_basevec.empty() && m_basevec.front == 0) m_basevec.erase(0);
        }

        template <typename container, typename base>
        friend std::ostream& operator<<(std::ostream& os, BigInt<container, base> const& bigin);

    private:
        std::vector<base_t> m_basevec;

        void push_front(const base_t val) {
            auto front = std::cbegin(m_basevec);
            m_basevec.insert(front, val);
            
        }

        BigInt& set(unsigned long r) {
            m_basevec.clear();
            while(r > 0) {
                const auto bottom = r & MASK;
                m_basevec.insert(std::cbegin(m_basevec), bottom);
                r >>= SHIFT;
            }
            return *this;
        }

        template <typename T> int sgn(T val) {
            return (T(0) < val) - (val < T(0));
        }


        constexpr static container_t lobyte(container_t x) {
            const auto val = x & MASK;
            return val;
        }
    
        constexpr static container_t hibyte(container_t x) {
            const auto val = (x >> SHIFT) & MASK;
            return val;
        }

    };

    template <typename container_t, typename base_t>
    std::ostream& operator<<(std::ostream& out, BigInt<container_t, base_t> const& bigin) {
        if(bigin.is_zero()) {
            out << '0';
        } else {
            const auto IAZ = '0';
            std::vector<char> cvec;
            auto copy = bigin;
            while(!copy.is_zero()) {
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

}

