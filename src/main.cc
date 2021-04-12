#include <cstdlib>

#include "bigmp.h"

namespace {
auto fib(int n) {
    bigmp::BigInt<unsigned, unsigned char> n_1(1UL), n_2(2UL);
    if(n == 1) {
        return n_1;
    } else if(n == 2) {
        return n_2;
    } else {
        for(int i=3; i<n; ++i) {
            auto n_0 = n_1 + n_2;
            n_1 = std::move(n_2);
            n_2 = std::move(n_0);
        }
        return n_1 + n_2;
    }
}
}

int main(int argc, char* argv[]) {

    for(int i=1; i<argc; ++i) {
        const auto n = std::atol(argv[i]);
        auto f = fib(n);
        std::cout << f << std::endl;
    }

    return EXIT_SUCCESS;
}
