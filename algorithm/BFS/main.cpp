#include <bits/stdc++.h>
using namespace std;

template<typename T = int>
vector<T> bfs(const vector<vector<int>>& graph, const vector<int>& starts, const T INF = INT_MAX) {
    const int N = static_cast<int>(graph.size());
    queue<int> q;
    vector<T> dist(N, INF);
    for(int start : starts) {
        assert(0 <= start && "bfs: 0 <= start");
        assert(start < N && "bfs: start < N");
        dist[start] = 0;
        q.push(start);
    }
    while(!q.empty()) {
        int now = q.front();
        q.pop();
        for(int to : graph[now]) {
            if(dist[to] != INF) continue;
            dist[to] = dist[now] + 1;
            q.push(to);
        }
    }
    return dist;
}

template<typename T = int>
vector<T> bfs(const vector<vector<int>>& graph, int start, const T INF = INT_MAX) {
    return bfs<T>(graph, vector<int>{start}, INF);
}
