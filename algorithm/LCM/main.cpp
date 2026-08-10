#include <bits/stdc++.h>
using namespace std;
using ll = long long;

ll gcd(ll a, ll b) {
    if(b != 0) a = gcd(b, a % b);
    return abs(a);
}

ll lcm(ll a, ll b) {
    if(a == 0 || b == 0) return 0;
    return abs(a / gcd(a, b) * b);
}
