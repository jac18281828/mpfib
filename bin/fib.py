import sys

sys.setrecursionlimit(1000)

save_value = {}

def fib_rec(n):
    if n <= 2:
        return n
    else:
        if n in save_value:
            return save_value[n]
        else:
            result = fib_rec(n-1) + fib_rec(n-2)
            if n < 2048:
                save_value[n] = result
        return result


def fib(n):
  fiblist = [0]
  for r in range(1, n):
    if r < 10:
        fiblist.append(fib_rec(r))
    else:
        fiblist.append(fiblist[-1] + fiblist[-2])
  return fiblist[-1]+fiblist[-2]

if len(sys.argv) > 1:
    for n in sys.argv[1:]:
        print(fib(int(n)))
