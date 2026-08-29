#include <bits/stdc++.h>
using namespace std;
using ll = long long;

int N;
vector<tuple<int, int, ll>> edges;

pair<ll, vector<tuple<int, int, ll>>> kruskal() {
    vector<tuple<int, int, ll>> sorted = edges;
    sort(sorted.begin(), sorted.end(), [](const auto& a, const auto& b) {
        return get<2>(a) < get<2>(b);
    });
    vector<int> par(N), siz(N, 1);
    iota(par.begin(), par.end(), 0);
    auto root = [&](auto&& self, int v) -> int {
        if(par[v] == v) return v;
        return par[v] = self(self, par[v]);
    };
    ll cost = 0;
    vector<tuple<int, int, ll>> used;
    for(auto [u, v, w] : sorted) {
        int ru = root(root, u);
        int rv = root(root, v);
        if(ru == rv) continue;
        if(siz[ru] < siz[rv]) swap(ru, rv);
        par[rv] = ru;
        siz[ru] += siz[rv];
        cost += w;
        used.emplace_back(u, v, w);
    }
    return {cost, used};
}
