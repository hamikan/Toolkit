#include <bits/stdc++.h>
using namespace std;
using ll = long long;
using P = pair<int, ll>;

int N;
vector<vector<P>> graph;

vector<ll> bfs01(const vector<int>& starts) {
    deque<int> q;
    vector<ll> dist(N, LLONG_MAX);
    for(int start : starts) {
        assert(0 <= start && "bfs01: 0 <= start");
        assert(start < N && "bfs01: start < N");
        if(dist[start] == 0) continue;
        dist[start] = 0;
        q.push_back(start);
    }
    while(!q.empty()) {
        int now = q.front();
        q.pop_front();
        for(auto [to, w] : graph[now]) {
            assert((w == 0 || w == 1) && "bfs01: w == 0 || w == 1");
            if(dist[to] <= dist[now] + w) continue;
            dist[to] = dist[now] + w;
            if(w == 0) q.push_front(to);
            else q.push_back(to);
        }
    }
    for(ll& d : dist) if(d == LLONG_MAX) d = -1;
    return dist;
}

vector<ll> bfs01(int start) {
    return bfs01(vector<int>{start});
}
