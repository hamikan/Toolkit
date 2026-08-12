#include <bits/stdc++.h>
using namespace std;
using ll = long long;

int digit(ll x) {
    int ret = 1;
    if(10000000000000000LL <= x) { ret += 16; x /= 10000000000000000LL; }
    if(100000000 <= x) { ret += 8; x /= 100000000; }
    if(10000 <= x) { ret += 4; x /= 10000; }
    if(100 <= x) { ret += 2; x /= 100; }
    if(10 <= x) ret++;
    return ret;
}
