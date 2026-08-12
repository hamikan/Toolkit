#include <bits/stdc++.h>
using namespace std;
using ll = long long;
using P = pair<ll, ll>;

int N;
vector<vector<P>> graph;

vector<ll> dijkstra(const vector<int>& starts) {
    priority_queue<P, vector<P>, greater<P>> pq;
    vector<ll> dist(N, LLONG_MAX);
    for(int start : starts) {
        assert(0 <= start && "dijkstra: 0 <= start");
        assert(start < N && "dijkstra: start < N");
        if(dist[start] == 0) continue;
        dist[start] = 0;
        pq.emplace(0, start);
    }
    while(!pq.empty()) {
        auto [cost, now] = pq.top();
        pq.pop();
        if(dist[now] < cost) continue;
        for(auto [to, w] : graph[now]) {
            if(dist[to] <= cost + w) continue;
            dist[to] = cost + w;
            pq.emplace(dist[to], to);
        }
    }
    for(ll& t : dist) t = (t == LLONG_MAX ? -1 : t);
    return dist;
}

vector<ll> dijkstra(int start) {
    return dijkstra(vector<int>{start});
}
