#include <bits/stdc++.h>
using namespace std;
using ll = long long;
using P = pair<int, ll>;

int N;
vector<vector<P>> graph;

vector<ll> bellman_ford(const vector<int>& starts) {
    vector<ll> dist(N, LLONG_MAX);
    for(int start : starts) {
        assert(0 <= start && "bellman_ford: 0 <= start");
        assert(start < N && "bellman_ford: start < N");
        dist[start] = 0;
    }
    for(int i=0; i<N-1; i++) {
        bool updated = false;
        for(int now=0; now<N; now++) {
            if(dist[now] == LLONG_MAX) continue;
            for(auto [to, w] : graph[now]) {
                if(dist[to] <= dist[now] + w) continue;
                dist[to] = dist[now] + w;
                updated = true;
            }
        }
        if(!updated) break;
    }
    vector<bool> negative(N, false);
    queue<int> q;
    for(int now=0; now<N; now++) {
        if(dist[now] == LLONG_MAX) continue;
        for(auto [to, w] : graph[now]) {
            if(dist[to] <= dist[now] + w || negative[to]) continue;
            negative[to] = true;
            q.push(to);
        }
    }
    while(!q.empty()) {
        int now = q.front();
        q.pop();
        for(const P& edge : graph[now]) {
            int to = edge.first;
            if(negative[to]) continue;
            negative[to] = true;
            q.push(to);
        }
    }
    for(int i=0; i<N; i++) if(negative[i]) dist[i] = LLONG_MIN;
    return dist;
}

vector<ll> bellman_ford(int start) {
    return bellman_ford(vector<int>{start});
}
