"""Pythonインタープリタで使う計算補助関数を提供する。"""

from math import comb as C
from math import factorial as fac
from math import gcd, lcm, log10, log2, perm, prod


def base2(x):
    return bin(x)[2:]


def H(n, r):
    return C(n + r - 1, r)


def P(n, r):
    return perm(n, r)


def modpow(a, b, m):
    ret = 1
    while b > 0:
        if b % 2 == 1:
            ret = ret * a % m
        a = a * a % m
        b //= 2
    return ret


def division(a, b, m):
    return a * modpow(b, m - 2, m) % m


def dig(x):
    return len(str(abs(x)))


def plist(N):
    if N < 2:
        return []
    is_prime = [True] * (N + 1)
    is_prime[0] = is_prime[1] = False
    primes = [2]
    for i in range(3, N + 1, 2):
        if is_prime[i]:
            primes.append(i)
            for j in range(i * i, N + 1, i * 2):
                is_prime[j] = False
    return primes


def pcnt(N):
    return len(plist(N))
