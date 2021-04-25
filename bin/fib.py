import sys,math

sys.setrecursionlimit(1000)

#fast doubling algorithm

TOPN = 50000
MAXN = 500000
prime_tab = []

def is_prime(m):
    for d in range(3, int(math.sqrt(m)), 2):    
        if i % d == 0:
            return False
    return True

prime_tab.append(2)
for i in range(3, TOPN, 2):
    if (is_prime(i)):
        prime_tab.append(i)
        
def fib(n):
    if n == 0:
        return (0, 1)
    else:
        a, b = fib(n // 2)
        c = a * (b * 2 - a)
        d = a * a + b * b
        if n % 2 == 0:
            return (c, d)
        else:
            return (d, c + d)

def is_probably_prime(m):
    for d in prime_tab:
        if m % d == 0:
            return False
    for d in range(TOPN+1, MAXN, 2):
        if is_prime(d) and m % d == 0:
            return False
    return True

if len(sys.argv) > 1:
    check_prime = False
    if sys.argv[1] == '-prime':
        check_prime = True
        args = sys.argv[2:]
    else:
        args = sys.argv[1:]
    for n in args:
        R = fib(int(n))[0]
        if check_prime:
            status = 'is probably prime' if is_probably_prime(R) else 'is not prime!'
            valstr = str(R)
            print(valstr + ' ' + status + ' with %d digits' % len(valstr))
        else:
            print(R)
