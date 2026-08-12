#include <bits/stdc++.h>
using namespace std;
using ll = long long;

ll modpow(ll a, ll b, ll m) {
    ll ret = 1 % m;
    a %= m;
    if(a < 0) a += m;
    while(b) {
        if(b & 1) ret = static_cast<ll>(__int128_t(ret) * a % m);
        a = static_cast<ll>(__int128_t(a) * a % m);
        b >>= 1;
    }
    return ret;
}
