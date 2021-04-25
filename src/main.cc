#include <cstdlib>
#include <utility>

#include "bigmp.h"

namespace {

using BigInt = bigmp::BigInt<unsigned int, unsigned char>;

const BigInt n0(0UL);
const BigInt n1(1UL);
const BigInt n2(1UL);
const BigInt n3(2UL);

// a big pair
std::pair<BigInt, BigInt> fib(int n) {
    switch(n) {
    case 0:
        return std::make_pair(n0, n1);
    case 1:
        return std::make_pair(n1, n2);            
    case 2:
        return std::make_pair(n2, n3);        
    default:
        auto ab = fib(n / 2);
        auto c = ab.first * ((ab.second * 2) - ab.first);
        auto d = (ab.first * ab.first) + (ab.second * ab.second);
        if (n % 2 == 0)
            return std::make_pair(c, d);
        else
            return std::make_pair(d, c + d);
    }
}
}

int main(int argc, char* argv[]) {

    for(int i=1; i<argc; ++i) {
        const auto n = std::atol(argv[i]);
        auto fn = fib(n);
        std::cout << fn.first << std::endl;
    }

    return EXIT_SUCCESS;
}
