#include <bits/stdc++.h>
using namespace std;
using ll = long long;

pair<ll, ll> crt(const vector<pair<ll, ll>>& equations) {
    auto extgcd = [](auto&& self, ll a, ll b, ll& x, ll& y) -> ll {
        if(b == 0) {
            x = (a < 0 ? -1 : 1);
            y = 0;
            return abs(a);
        }
        const ll d = self(self, b, a % b, y, x);
        y -= (a / b) * x;
        return d;
    };
    ll r = 0, m = 1;
    for(auto [nr, nm] : equations) {
        assert(1 <= nm && "crt: 1 <= mod");
        nr %= nm;
        if(nr < 0) nr += nm;
        ll x, y;
        const ll g = extgcd(extgcd, m, nm, x, y);
        const ll diff = nr - r;
        if(diff % g != 0) return {0, -1};
        const ll mod = nm / g;
        ll k = static_cast<ll>(static_cast<__int128_t>(diff / g) * x % mod);
        if(k < 0) k += mod;
        const __int128_t next_r = static_cast<__int128_t>(r) + static_cast<__int128_t>(m) * k;
        const __int128_t next_m = static_cast<__int128_t>(m / g) * nm;
        assert(next_m <= LLONG_MAX && "crt: lcm <= LLONG_MAX");
        r = static_cast<ll>(next_r % next_m);
        m = static_cast<ll>(next_m);
    }
    return {r, m};
}
