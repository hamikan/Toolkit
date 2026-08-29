#include <bits/stdc++.h>
using namespace std;
using ll = long long;
using P = pair<int, ll>;

int N;
vector<vector<P>> graph;

vector<vector<ll>> warshall_floyd() {
    vector<vector<ll>> dist(N, vector<ll>(N, LLONG_MAX));
    for(int i=0; i<N; i++) dist[i][i] = 0;
    for(int i=0; i<N; i++) for(auto [to, w] : graph[i]) dist[i][to] = min(dist[i][to], w);
    for(int k=0; k<N; k++) for(int i=0; i<N; i++) {
        if(dist[i][k] == LLONG_MAX) continue;
        for(int j=0; j<N; j++) {
            if(dist[k][j] == LLONG_MAX) continue;
            dist[i][j] = min(dist[i][j], dist[i][k] + dist[k][j]);
        }
    }
    return dist;
}
