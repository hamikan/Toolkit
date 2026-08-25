#include <bits/stdc++.h>
using namespace std;

template<typename T>
struct Dinic {
private:
    struct Edge {
        int to;
        int rev;
        T cap;
    };

    int N_;
    vector<pair<int, int>> pos_;
    vector<vector<Edge>> graph_;
    vector<int> level_;
    vector<int> iter_;

    void bfs(int s) {
        fill(level_.begin(), level_.end(), -1);
        queue<int> q;
        level_[s] = 0;
        q.push(s);
        while(!q.empty()) {
            int v = q.front(); q.pop();
            for(const Edge& e : graph_[v]) {
                if(T{} < e.cap && level_[e.to] < 0) {
                    level_[e.to] = level_[v] + 1;
                    q.push(e.to);
                }
            }
        }
    }

    T dfs(int v, int t, T f) {
        if(v == t) return f;
        for(int& i=iter_[v]; i<static_cast<int>(graph_[v].size()); i++) {
            Edge& e = graph_[v][i];
            if(T{} < e.cap && level_[e.to] == level_[v] + 1) {
                T d = dfs(e.to, t, min(f, e.cap));
                if(T{} < d) {
                    e.cap -= d;
                    graph_[e.to][e.rev].cap += d;
                    return d;
                }
            }
        }
        return T{};
    }

public:
    struct EdgeInfo {
        int from;
        int to;
        T cap;
        T flow;
    };

    explicit Dinic(int N) : N_(N), graph_(N), level_(N), iter_(N) {}

    int size() const {
        return N_;
    }

    int add_edge(int from, int to, T cap) {
        assert(0 <= from && "Dinic::add_edge: 0 <= from");
        assert(from < N_ && "Dinic::add_edge: from < N_");
        assert(0 <= to && "Dinic::add_edge: 0 <= to");
        assert(to < N_ && "Dinic::add_edge: to < N_");
        assert(T{} <= cap && "Dinic::add_edge: T{} <= cap");
        int id = static_cast<int>(pos_.size());
        int from_id = static_cast<int>(graph_[from].size());
        int to_id = static_cast<int>(graph_[to].size());
        if(from == to) to_id++;
        pos_.emplace_back(from, from_id);
        graph_[from].emplace_back(to, to_id, cap);
        graph_[to].emplace_back(from, from_id, T{});
        return id;
    }

    EdgeInfo get_edge(int i) const {
        assert(0 <= i && "Dinic::get_edge: 0 <= i");
        assert(i < static_cast<int>(pos_.size()) && "Dinic::get_edge: i < pos_.size()");
        auto [from, id] = pos_[i];
        const Edge& e = graph_[from][id];
        const Edge& re = graph_[e.to][e.rev];
        return {
            from,
            e.to,
            e.cap + re.cap,
            re.cap
        };
    }

    vector<EdgeInfo> edges() const {
        vector<EdgeInfo> ret;
        ret.reserve(pos_.size());
        for(int i=0; i<static_cast<int>(pos_.size()); i++) ret.push_back(get_edge(i));
        return ret;
    }

    void change_edge(int i, T cap, T flow) {
        assert(0 <= i && "Dinic::change_edge: 0 <= i");
        assert(i < static_cast<int>(pos_.size()) && "Dinic::change_edge: i < pos_.size()");
        assert(T{} <= flow && "Dinic::change_edge: T{} <= flow");
        assert(flow <= cap && "Dinic::change_edge: flow <= cap");
        auto [from, id] = pos_[i];
        Edge& e = graph_[from][id];
        Edge& re = graph_[e.to][e.rev];
        e.cap = cap - flow;
        re.cap = flow;
    }

    T max_flow(int s, int t, T limit = numeric_limits<T>::max()) {
        assert(0 <= s && "Dinic::max_flow: 0 <= s");
        assert(s < N_ && "Dinic::max_flow: s < N_");
        assert(0 <= t && "Dinic::max_flow: 0 <= t");
        assert(t < N_ && "Dinic::max_flow: t < N_");
        assert(s != t && "Dinic::max_flow: s != t");
        assert(T{} <= limit && "Dinic::max_flow: T{} <= limit");
        T flow = T{};
        while(flow < limit) {
            bfs(s);
            if(level_[t] < 0) break;
            fill(iter_.begin(), iter_.end(), 0);
            T f;
            while(T{} < (f = dfs(s, t, limit - flow))) flow += f;
        }
        return flow;
    }

    vector<bool> min_cut(int s) const {
        assert(0 <= s && "Dinic::min_cut: 0 <= s");
        assert(s < N_ && "Dinic::min_cut: s < N_");
        vector<bool> ret(N_, false);
        queue<int> q;
        ret[s] = true;
        q.push(s);
        while(!q.empty()) {
            int v = q.front(); q.pop();
            for(const Edge& e : graph_[v]) {
                if(T{} < e.cap && !ret[e.to]) {
                    ret[e.to] = true;
                    q.push(e.to);
                }
            }
        }
        return ret;
    }
};
