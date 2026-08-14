#include <bits/stdc++.h>
using namespace std;

struct SCC {
private:
    int N_;
    int scc_count_;
    vector<vector<int>> graph_;
    vector<vector<int>> rgraph_;
    vector<int> order_;
    vector<int> id_;
    vector<bool> used_;
    bool built_;

public:
    explicit SCC(int N) : N_(N), scc_count_(0), graph_(N), rgraph_(N), id_(N, -1), used_(N, false), built_(false) {}

    void add_edge(int u, int v) {
        assert(0 <= u && "SCC::add_edge: 0 <= u");
        assert(u < N_ && "SCC::add_edge: u < N_");
        assert(0 <= v && "SCC::add_edge: 0 <= v");
        assert(v < N_ && "SCC::add_edge: v < N_");
        graph_[u].push_back(v);
        rgraph_[v].push_back(u);
        built_ = false;
    }

    void build() {
        if(built_) return;
        scc_count_ = 0;
        order_.clear();
        order_.reserve(N_);
        fill(id_.begin(), id_.end(), -1);
        fill(used_.begin(), used_.end(), false);
        vector<pair<int, int>> dfs;
        dfs.reserve(N_);
        for(int s=0; s<N_; s++) {
            if(used_[s]) continue;
            used_[s] = true;
            dfs.emplace_back(s, 0);
            while(!dfs.empty()) {
                auto& [v, i] = dfs.back();
                if(i == static_cast<int>(graph_[v].size())) {
                    order_.push_back(v);
                    dfs.pop_back();
                    continue;
                }
                int u = graph_[v][i++];
                if(used_[u]) continue;
                used_[u] = true;
                dfs.emplace_back(u, 0);
            }
        }
        reverse(order_.begin(), order_.end());
        vector<int> rdfs;
        rdfs.reserve(N_);
        for(int s : order_) {
            if(id_[s] != -1) continue;
            id_[s] = scc_count_;
            rdfs.push_back(s);
            while(!rdfs.empty()) {
                int v = rdfs.back();
                rdfs.pop_back();
                for(int u : rgraph_[v]) {
                    if(id_[u] != -1) continue;
                    id_[u] = scc_count_;
                    rdfs.push_back(u);
                }
            }
            scc_count_++;
        }
        built_ = true;
    }

    int size() const {
        return N_;
    }

    int scc_count() const {
        assert(built_ && "SCC::scc_count: built_");
        return scc_count_;
    }

    int get_id(int u) const {
        assert(built_ && "SCC::get_id: built_");
        assert(0 <= u && "SCC::get_id: 0 <= u");
        assert(u < N_ && "SCC::get_id: u < N_");
        return id_[u];
    }

    bool same(int u, int v) const {
        assert(built_ && "SCC::same: built_");
        assert(0 <= u && "SCC::same: 0 <= u");
        assert(u < N_ && "SCC::same: u < N_");
        assert(0 <= v && "SCC::same: 0 <= v");
        assert(v < N_ && "SCC::same: v < N_");
        return id_[u] == id_[v];
    }

    const vector<int>& operator[](int v) const {
        assert(0 <= v && "SCC::operator[]: 0 <= v");
        assert(v < N_ && "SCC::operator[]: v < N_");
        return graph_[v];
    }

    vector<vector<int>> get_groups() const {
        assert(built_ && "SCC::get_groups: built_");
        vector<vector<int>> groups(scc_count_);
        for(int i=0; i<N_; i++) groups[id_[i]].push_back(i);
        return groups;
    }

    vector<vector<int>> get_dag() const {
        assert(built_ && "SCC::get_dag: built_");
        vector<vector<int>> dag(scc_count_);
        for(int v=0; v<N_; v++) {
            for(int u : graph_[v]) {
                if(id_[v] == id_[u]) continue;
                dag[id_[v]].push_back(id_[u]);
            }
        }
        for(int i=0; i<scc_count_; i++) {
            sort(dag[i].begin(), dag[i].end());
            dag[i].erase(unique(dag[i].begin(), dag[i].end()), dag[i].end());
        }
        return dag;
    }
};
