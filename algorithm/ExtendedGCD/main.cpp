#include <bits/stdc++.h>
using namespace std;
using ll = long long;

ll extgcd(ll a, ll b, ll &x, ll &y) {
    if(b == 0) {
        x = (a < 0 ? -1 : 1);
        y = 0;
        return abs(a);
    }
    ll d = extgcd(b, a % b, y, x);
    y -= (a / b) * x;
    return d;
}
