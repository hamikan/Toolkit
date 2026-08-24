#include <bits/stdc++.h>
using namespace std;

struct TwoSat {
private:
    int N_;
    vector<vector<int>> graph_;
    vector<vector<int>> rgraph_;
    vector<bool> answer_;
    bool built_;
    bool has_solution_;

    vector<int> scc() const {
        const int n = 2 * N_;
        vector<bool> used(n, false);
        vector<int> order;
        order.reserve(n);
        vector<pair<int, int>> dfs;
        dfs.reserve(n);
        for(int s=0; s<n; s++) {
            if(used[s]) continue;
            used[s] = true;
            dfs.emplace_back(s, 0);
            while(!dfs.empty()) {
                auto& [v, i] = dfs.back();
                if(i == static_cast<int>(graph_[v].size())) {
                    order.push_back(v);
                    dfs.pop_back();
                    continue;
                }
                int u = graph_[v][i++];
                if(used[u]) continue;
                used[u] = true;
                dfs.emplace_back(u, 0);
            }
        }
        reverse(order.begin(), order.end());
        vector<int> id(n, -1);
        vector<int> rdfs;
        rdfs.reserve(n);
        int scc_count = 0;
        for(int s : order) {
            if(id[s] != -1) continue;
            id[s] = scc_count;
            rdfs.push_back(s);
            while(!rdfs.empty()) {
                int v = rdfs.back();
                rdfs.pop_back();
                for(int u : rgraph_[v]) {
                    if(id[u] != -1) continue;
                    id[u] = scc_count;
                    rdfs.push_back(u);
                }
            }
            scc_count++;
        }
        return id;
    }

    void add_edge(int u, int v) {
        graph_[u].push_back(v);
        rgraph_[v].push_back(u);
        built_ = false;
    }

public:
    explicit TwoSat(int N) : N_(N), graph_(2 * N), rgraph_(2 * N), answer_(N), built_(false), has_solution_(false) {}

    int size() const {
        return N_;
    }

    void add_clause(int x, bool f, int y, bool g) {
        assert(0 <= x && "TwoSat::add_clause: 0 <= x");
        assert(x < N_ && "TwoSat::add_clause: x < N_");
        assert(0 <= y && "TwoSat::add_clause: 0 <= y");
        assert(y < N_ && "TwoSat::add_clause: y < N_");
        int u = x + (f ? 0 : N_);
        int nu = x + (f ? N_ : 0);
        int v = y + (g ? 0 : N_);
        int nv = y + (g ? N_ : 0);
        add_edge(nu, v);
        add_edge(nv, u);
    }

    void add_same(int x, int y) {
        assert(0 <= x && "TwoSat::add_same: 0 <= x");
        assert(x < N_ && "TwoSat::add_same: x < N_");
        assert(0 <= y && "TwoSat::add_same: 0 <= y");
        assert(y < N_ && "TwoSat::add_same: y < N_");
        add_edge(x, y);
        add_edge(y, x);
        add_edge(x + N_, y + N_);
        add_edge(y + N_, x + N_);
    }

    void add_different(int x, int y) {
        assert(0 <= x && "TwoSat::add_different: 0 <= x");
        assert(x < N_ && "TwoSat::add_different: x < N_");
        assert(0 <= y && "TwoSat::add_different: 0 <= y");
        assert(y < N_ && "TwoSat::add_different: y < N_");
        add_edge(x, y + N_);
        add_edge(y + N_, x);
        add_edge(x + N_, y);
        add_edge(y, x + N_);
    }

    void set(int x, bool f) {
        assert(0 <= x && "TwoSat::set: 0 <= x");
        assert(x < N_ && "TwoSat::set: x < N_");
        int u = x + (f ? 0 : N_);
        int nu = x + (f ? N_ : 0);
        add_edge(nu, u);
    }

    bool satisfiable() {
        if(built_) return has_solution_;
        const vector<int> id = scc();
        built_ = true;
        has_solution_ = false;
        for(int i=0; i<N_; i++) if(id[i] == id[i+N_]) return false;
        has_solution_ = true;
        for(int i=0; i<N_; i++) answer_[i] = id[i] > id[i+N_];
        return true;
    }

    bool get(int x) const {
        assert(built_ && "TwoSat::get: built_");
        assert(has_solution_ && "TwoSat::get: has_solution_");
        assert(0 <= x && "TwoSat::get: 0 <= x");
        assert(x < N_ && "TwoSat::get: x < N_");
        return answer_[x];
    }
};
