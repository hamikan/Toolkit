#include <bits/stdc++.h>
using namespace std;

struct LCA {
private:
    int N_;
    int log_;
    vector<vector<int>> graph_;
    vector<vector<int>> par_;
    vector<int> dep_;
    bool built_;

public:
    explicit LCA(int N) : N_(N), graph_(N), built_(false) {}
    explicit LCA(const vector<vector<int>>& graph, int root = 0) : N_(static_cast<int>(graph.size())), graph_(graph), built_(false) {
        build(root);
    }

    void add_edge(int u, int v) {
        assert(0 <= u && "LCA::add_edge: 0 <= u");
        assert(u < N_ && "LCA::add_edge: u < N_");
        assert(0 <= v && "LCA::add_edge: 0 <= v");
        assert(v < N_ && "LCA::add_edge: v < N_");
        graph_[u].push_back(v);
        graph_[v].push_back(u);
        built_ = false;
    }

    void build(int root = 0) {
        assert(0 <= root && "LCA::build: 0 <= root");
        assert(root < N_ && "LCA::build: root < N_");
        log_ = 1;
        while((1 << log_) < N_) log_++;
        par_.assign(log_, vector<int>(N_, -1));
        dep_.assign(N_, 0);
        stack<pair<int, int>> dfs;
        dfs.emplace(root, -1);
        while(!dfs.empty()) {
            auto [v, p] = dfs.top();
            dfs.pop();
            for(int to : graph_[v]) {
                if(to == p) continue;
                par_[0][to] = v;
                dep_[to] = dep_[v] + 1;
                dfs.emplace(to, v);
            }
        }
        for(int k=0; k+1<log_; k++) {
            for(int v=0; v<N_; v++) {
                if(par_[k][v] < 0) par_[k+1][v] = -1;
                else par_[k+1][v] = par_[k][par_[k][v]];
            }
        }
        built_ = true;
    }

    int size() const {
        return N_;
    }

    int depth(int v) const {
        assert(built_ && "LCA::depth: built_");
        assert(0 <= v && "LCA::depth: 0 <= v");
        assert(v < N_ && "LCA::depth: v < N_");
        return dep_[v];
    }

    int parent(int v) const {
        assert(built_ && "LCA::parent: built_");
        assert(0 <= v && "LCA::parent: 0 <= v");
        assert(v < N_ && "LCA::parent: v < N_");
        return par_[0][v];
    }

    int query(int u, int v) const {
        assert(built_ && "LCA::query: built_");
        assert(0 <= u && "LCA::query: 0 <= u");
        assert(u < N_ && "LCA::query: u < N_");
        assert(0 <= v && "LCA::query: 0 <= v");
        assert(v < N_ && "LCA::query: v < N_");
        if(dep_[u] < dep_[v]) swap(u, v);
        for(int k=0; k<log_; k++) {
            if(((dep_[u] - dep_[v]) >> k) & 1) {
                u = par_[k][u];
            }
        }
        if(u == v) return u;
        for(int k=log_-1; k>=0; k--) {
            if(par_[k][u] != par_[k][v]) {
                u = par_[k][u];
                v = par_[k][v];
            }
        }
        return  par_[0][u];
    }

    int query(int u, int v, int root) const {
        assert(built_ && "LCA::query: built_");
        assert(0 <= u && "LCA::query: 0 <= u");
        assert(u < N_ && "LCA::query: u < N_");
        assert(0 <= v && "LCA::query: 0 <= v");
        assert(v < N_ && "LCA::query: v < N_");
        assert(0 <= root && "LCA::query: 0 <= root");
        assert(root < N_ && "LCA::query: root < N_");
        int a = query(u, v);
        int b = query(u, root);
        int c = query(v, root);
        if(a == b) return c;
        if(a == c) return b;
        return a;
    }

    int get_dist(int u, int v) const {
        assert(built_ && "LCA::get_dist: built_");
        assert(0 <= u && "LCA::get_dist: 0 <= u");
        assert(u < N_ && "LCA::get_dist: u < N_");
        assert(0 <= v && "LCA::get_dist: 0 <= v");
        assert(v < N_ && "LCA::get_dist: v < N_");
        return dep_[u] + dep_[v] - 2 * dep_[query(u, v)];
    }

    int ancestor(int v, int k) const {
        assert(built_ && "LCA::ancestor: built_");
        assert(0 <= v && "LCA::ancestor: 0 <= v");
        assert(v < N_ && "LCA::ancestor: v < N_");
        assert(0 <= k && "LCA::ancestor: 0 <= k");
        if(dep_[v] < k) return -1;
        for(int i=0; i<log_; i++) if((k >> i) & 1) v = par_[i][v];
        return v;
    }

    int jump(int u, int v, int k) const {
        assert(built_ && "LCA::jump: built_");
        assert(0 <= u && "LCA::jump: 0 <= u");
        assert(u < N_ && "LCA::jump: u < N_");
        assert(0 <= v && "LCA::jump: 0 <= v");
        assert(v < N_ && "LCA::jump: v < N_");
        assert(0 <= k && "LCA::jump: 0 <= k");
        int l = query(u, v);
        int du = dep_[u] - dep_[l];
        int dv = dep_[v] - dep_[l];
        if(du + dv < k) return -1;
        if(k <= du) return ancestor(u, k);
        return ancestor(v, du + dv - k);
    }

    vector<int> path(int u, int v) const {
        assert(built_ && "LCA::path: built_");
        assert(0 <= u && "LCA::path: 0 <= u");
        assert(u < N_ && "LCA::path: u < N_");
        assert(0 <= v && "LCA::path: 0 <= v");
        assert(v < N_ && "LCA::path: v < N_");
        int l = query(u, v);
        int du = dep_[u] - dep_[l];
        int dv = dep_[v] - dep_[l];
        vector<int> left, right;
        left.reserve(du + dv + 1);
        right.reserve(dv);
        while(u != l) {
            left.push_back(u);
            u = par_[0][u];
        }
        left.push_back(l);
        while(v != l) {
            right.push_back(v);
            v = par_[0][v];
        }
        reverse(right.begin(), right.end());
        left.insert(left.end(), right.begin(), right.end());
        return left;
    }

    bool is_on_path(int u, int v, int a) const {
        assert(built_ && "LCA::is_on_path: built_");
        assert(0 <= u && "LCA::is_on_path: 0 <= u");
        assert(u < N_ && "LCA::is_on_path: u < N_");
        assert(0 <= v && "LCA::is_on_path: 0 <= v");
        assert(v < N_ && "LCA::is_on_path: v < N_");
        assert(0 <= a && "LCA::is_on_path: 0 <= a");
        assert(a < N_ && "LCA::is_on_path: a < N_");
        return get_dist(u, a) + get_dist(a, v) == get_dist(u, v);
    }

    bool is_ancestor(int u, int v) const {
        assert(built_ && "LCA::is_ancestor: built_");
        assert(0 <= u && "LCA::is_ancestor: 0 <= u");
        assert(u < N_ && "LCA::is_ancestor: u < N_");
        assert(0 <= v && "LCA::is_ancestor: 0 <= v");
        assert(v < N_ && "LCA::is_ancestor: v < N_");
        return query(u, v) == u;
    }
};
