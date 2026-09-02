#include <bits/stdc++.h>
using namespace std;
using ll = long long;

mt19937_64 rng(chrono::steady_clock::now().time_since_epoch().count());

template<typename T>
T rnd(T l, T r) {
    assert(l <= r && "rnd: l <= r");
    return uniform_int_distribution<T>(l, r)(rng);
}

string rndstr(int N, const string& chars = "abcdefghijklmnopqrstuvwxyz") {
    assert(0 <= N && "rndstr: 0 <= N");
    assert(!chars.empty() && "rndstr: !chars.empty()");
    string ret(N, ' ');
    for(char& c : ret) c = chars[rnd(0, static_cast<int>(chars.size()) - 1)];
    return ret;
}

template<typename T>
vector<T> rndvec(int N, T l, T r) {
    assert(0 <= N && "rndvec: 0 <= N");
    assert(l <= r && "rndvec: l <= r");
    vector<T> ret(N);
    for(T& x : ret) x = rnd(l, r);
    return ret;
}

vector<int> rndperm(int N, int base = 1) {
    assert(0 <= N && "rndperm: 0 <= N");
    vector<int> ret(N);
    iota(ret.begin(), ret.end(), base);
    shuffle(ret.begin(), ret.end(), rng);
    return ret;
}

template<typename T>
vector<T> rndunique(int N, T l, T r) {
    assert(0 <= N && "rndunique: 0 <= N");
    assert(l <= r && "rndunique: l <= r");
    const __int128_t count = static_cast<__int128_t>(r) - static_cast<__int128_t>(l) + 1;
    assert(N <= count && "rndunique: N <= r - l + 1");
    vector<T> ret;
    ret.reserve(N);
    if(count <= static_cast<__int128_t>(N) * 2) {
        for(T x=l;; x++) {
            ret.push_back(x);
            if(x == r) break;
        }
        shuffle(ret.begin(), ret.end(), rng);
        ret.resize(N);
        return ret;
    }
    unordered_set<T> used;
    used.reserve(static_cast<size_t>(N) * 2);
    while(static_cast<int>(ret.size()) < N) {
        const T x = rnd(l, r);
        if(used.insert(x).second) ret.push_back(x);
    }
    return ret;
}

vector<string> rndgrid(int H, int W, const string& chars = ".#", const string& special = "") {
    assert(0 <= H && "rndgrid: 0 <= H");
    assert(0 <= W && "rndgrid: 0 <= W");
    assert(!chars.empty() && "rndgrid: !chars.empty()");
    assert(static_cast<int>(special.size()) <= H * W && "rndgrid: special.size() <= H * W");
    vector<string> ret(H);
    for(string& row : ret) row = rndstr(W, chars);
    if(!special.empty()) {
        const vector<int> pos = rndunique(static_cast<int>(special.size()), 0, H * W - 1);
        for(int i=0; i<static_cast<int>(special.size()); i++) ret[pos[i]/W][pos[i]%W] = special[i];
    }
    return ret;
}

template<typename T>
vector<vector<T>> rndgrid(int H, int W, T l, T r) {
    assert(0 <= H && "rndgrid: 0 <= H");
    assert(0 <= W && "rndgrid: 0 <= W");
    assert(l <= r && "rndgrid: l <= r");
    vector<vector<T>> ret(H);
    for(auto& row : ret) row = rndvec<T>(W, l, r);
    return ret;
}

vector<string> rndconnectedgrid(int H, int W, int K, char empty = '.', char wall = '#', const string& special = "") {
    assert(0 <= H && "rndconnectedgrid: 0 <= H");
    assert(0 <= W && "rndconnectedgrid: 0 <= W");
    assert(1 <= K && "rndconnectedgrid: 1 <= K");
    assert(K <= H * W && "rndconnectedgrid: K <= H * W");
    assert(static_cast<int>(special.size()) <= K && "rndconnectedgrid: special.size() <= K");
    assert(empty != wall && "rndconnectedgrid: empty != wall");
    for(char c : special) assert(c != wall && "rndconnectedgrid: special[i] != wall");
    vector<string> ret(H, string(W, empty));
    int open = H * W;
    const pair<int, int> dir[4] = {{1, 0}, {0, 1}, {-1, 0}, {0, -1}};
    while(open > K) {
        const int V = H * W;
        vector<int> tin(V, -1), low(V);
        vector<bool> articulation(V);
        int timer = 0;
        auto dfs = [&](auto&& self, int v, int parent) -> void {
            tin[v] = low[v] = timer++;
            const int r = v / W, c = v % W;
            int child = 0;
            for(auto [dr, dc] : dir) {
                const int nr = r + dr, nc = c + dc;
                if(nr < 0 || H <= nr || nc < 0 || W <= nc) continue;
                if(ret[nr][nc] == wall) continue;
                const int to = nr * W + nc;
                if(to == parent) continue;
                if(tin[to] != -1) {
                    low[v] = min(low[v], tin[to]);
                    continue;
                }
                self(self, to, v);
                low[v] = min(low[v], low[to]);
                if(parent != -1 && tin[v] <= low[to]) articulation[v] = true;
                child++;
            }
            if(parent == -1 && 2 <= child) articulation[v] = true;
        };
        for(int v=0; v<V; v++) {
            const int r = v / W, c = v % W;
            if(ret[r][c] != wall && tin[v] == -1) dfs(dfs, v, -1);
        }
        vector<int> candidate;
        candidate.reserve(open);
        for(int v=0; v<V; v++) {
            const int r = v / W;
            const int c = v % W;
            if(ret[r][c] != wall && !articulation[v]) candidate.push_back(v);
        }
        const int p = candidate[rnd(0, static_cast<int>(candidate.size()) - 1)];
        ret[p/W][p%W] = wall;
        open--;
    }
    if(!special.empty()) {
        vector<int> pos;
        pos.reserve(K);
        for(int r=0; r<H; r++) {
            for(int c=0; c<W; c++) {
                if(ret[r][c] != wall) pos.push_back(r * W + c);
            }
        }
        const vector<int> idx = rndunique(static_cast<int>(special.size()), 0, K - 1);
        for(int i=0; i<static_cast<int>(special.size()); i++) {
            const int p = pos[idx[i]];
            ret[p/W][p%W] = special[i];
        }
    }
    return ret;
}

vector<pair<int, int>> rndtree(int N, int base = 1) {
    assert(0 <= N && "rndtree: 0 <= N");
    if(N <= 1) return {};
    const vector<int> prufer = rndvec<int>(N - 2, 0, N - 1);
    vector<int> degree(N, 1);
    for(int v : prufer) degree[v]++;
    priority_queue<int, vector<int>, greater<int>> leaf;
    for(int v=0; v<N; v++) if(degree[v] == 1) leaf.push(v);
    vector<pair<int, int>> ret;
    ret.reserve(N - 1);
    for(int v : prufer) {
        const int u = leaf.top();
        leaf.pop();
        ret.emplace_back(u + base, v + base);
        degree[u]--;
        degree[v]--;
        if(degree[v] == 1) leaf.push(v);
    }
    int u = leaf.top();
    leaf.pop();
    int v = leaf.top();
    ret.emplace_back(u + base, v + base);
    for(auto& [a, b] : ret) if(rnd(0, 1)) swap(a, b);
    shuffle(ret.begin(), ret.end(), rng);
    return ret;
}

vector<pair<int, int>> rndgraph(int N, int M, bool connected = true, bool directed = false, int base = 1) {
    assert(0 <= N && "rndgraph: 0 <= N");
    assert(0 <= M && "rndgraph: 0 <= M");
    const ll max_edges = directed ? ll(N) * (N - 1) : ll(N) * (N - 1) / 2;
    assert(M <= max_edges && "rndgraph: M <= max_edges");
    assert((!connected || max(0, N - 1) <= M) && "rndgraph: !connected || N - 1 <= M");
    vector<pair<int, int>> ret;
    ret.reserve(M);
    unordered_set<unsigned long long> used;
    used.reserve(static_cast<size_t>(M) * 2);
    auto add_edge = [&](int a, int b) {
        if(!directed && b < a) swap(a, b);
        const auto key = static_cast<unsigned long long>(a) * static_cast<unsigned long long>(N) + static_cast<unsigned long long>(b);
        if(!used.insert(key).second) return;
        ret.emplace_back(a + base, b + base);
    };
    if(connected) for(auto [a, b] : rndtree(N, 0)) add_edge(a, b);
    if(ll(M) * 2 > max_edges) {
        vector<pair<int, int>> candidate;
        candidate.reserve(static_cast<size_t>(max_edges - static_cast<ll>(used.size())));
        for(int a=0; a<N; a++) {
            for(int b=0; b<N; b++) {
                if(a == b || (!directed && b < a)) continue;
                const auto key = static_cast<unsigned long long>(a) * static_cast<unsigned long long>(N) + static_cast<unsigned long long>(b);
                if(!used.contains(key)) candidate.emplace_back(a, b);
            }
        }
        shuffle(candidate.begin(), candidate.end(), rng);
        const int need = M - static_cast<int>(ret.size());
        for(int i=0; i<need; i++) ret.emplace_back(candidate[i].first + base, candidate[i].second + base);
    } else {
        while(static_cast<int>(ret.size()) < M) {
            int a = rnd(0, N - 1), b = rnd(0, N - 1);
            if(a == b) continue;
            add_edge(a, b);
        }
    }
    if(!directed) for(auto& [a, b] : ret) if(rnd(0, 1)) swap(a, b);
    shuffle(ret.begin(), ret.end(), rng);
    return ret;
}

template<typename T>
void out(const vector<T>& v) {
    for(int i=0; i<static_cast<int>(v.size()); i++) cout << v[i] << " \n"[i == static_cast<int>(v.size()) - 1];
}

template<typename T, typename U>
void out(const vector<pair<T, U>>& v) {
    for(const auto& [a, b] : v) cout << a << ' ' << b << '\n';
}

void out(const vector<string>& grid) {
    for(const string& row : grid) cout << row << '\n';
}

template<typename T>
void out(const vector<vector<T>>& grid) {
    for(const auto& row : grid) out(row);
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    const int N = rnd(1, 10);
    const int M = rnd(N - 1, N * (N - 1) / 2);
    cout << N << " " << M << "\n";
    out(rndgraph(N, M));
    return 0;
}
