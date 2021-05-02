# mpfib - Fast Doubling Fibonacci Primes Arbitrary Precision Calculator

# with Docker container for all environments
# c++ '20

This is a high-digit Fibonacci Prime calculator in C++.    My son and I spent about 30 minutes building a python program to get F(1000000) and then I spent about two weeks of spare time getting the same result in C++. 😅

The 'BigInt' class is an arbitrary precision math class that performs radix 256 math.

Records:

| Index | Notes |
| :---: | :----: |
| F(10M) | 2089878 digits |
| F(3340367) | probable prime! |

This is all rather interesesting, down right exciting, if this is the sort of thing that floats your boat!

'bigmp::BigInt'

| operation | description |  approach |
| :-----: | :------: | :------: |
| '+=' | addition | quick add from RADIX 256 single digit|
| '+=' | addition | arbitrary precision |
| '+' | addition | base type quick add |
| '+' | arbitrary precision |
| '*=' | multiplication | quick multiply from RADIX 256 single digit |
| '*' | multiplication | quick multiply from RADIX 256 single digit |
| '*' | multiplication | arbitrary precision based on Karatsuba approach |
| '==' | equality | arbitrary precisiion |
| '/' | division | quick division of a RADIX 256 digit |
| '%' | modulus | quick mod from a RADIX 256 digit | 

