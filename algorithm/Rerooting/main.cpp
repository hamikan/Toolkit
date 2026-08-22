#include <bits/stdc++.h>
using namespace std;

struct DP {
    struct S {
        long long acc;
        int cnt;
    };
    struct V {
        long long w;
        int color;
    };
    struct E {
        long long w;
    };
    static S op(const S& a, const S& b) {
        return {
            a.acc + b.acc,
            a.cnt + b.cnt
        };
    }
    static S e() {
        return {
            0,
            0
        };
    }
    static S add_root(const S& x, const V& v) {
        return {
            x.acc + v.w * (x.cnt + 1),
            x.cnt + 1
        };
    }
    static S through(const S& x, const E& e, const V& u, const V& v) {
        const long long cost = e.w * (u.color != v.color);
        return {
            x.acc + cost * x.cnt,
            x.cnt
        };
    }
};

template<typename DP>
struct Rerooting {
private:
    using S = typename DP::S;
    using V = typename DP::V;
    using E = typename DP::E;

    int N_;
    vector<vector<pair<int, E>>> graph_;
    vector<V> val_;
    vector<vector<S>> dp_;
    vector<S> up_;
    vector<S> down_;
    vector<S> ans_;
    bool built_;

public:
    explicit Rerooting(int N) : Rerooting(vector<V>(N, V{})) {}
    explicit Rerooting(int N, const V& x) : Rerooting(vector<V>(N, x)) {}
    explicit Rerooting(const vector<V>& v) : N_(static_cast<int>(v.size())), graph_(N_), val_(v), dp_(N_), up_(N_, DP::e()), down_(N_, DP::e()), ans_(N_, DP::e()), built_(false) {}

    void add_edge(int a, int b, const E& ab = E{}) { add_edge(a, b, ab, ab); }
    void add_edge(int a, int b, const E& ab, const E& ba) {
        assert(0 <= a && "Rerooting::add_edge: 0 <= a");
        assert(a < N_ && "Rerooting::add_edge: a < N_");
        assert(0 <= b && "Rerooting::add_edge: 0 <= b");
        assert(b < N_ && "Rerooting::add_edge: b < N_");
        assert(a != b && "Rerooting::add_edge: a != b");
        graph_[a].emplace_back(b, ab);
        graph_[b].emplace_back(a, ba);
        built_ = false;
    }

    void build(int root = 0) {
        assert(0 <= root && "Rerooting::build: 0 <= root");
        assert(root < N_ && "Rerooting::build: root < N_");
        vector<int> par(N_, -1), order, dfs;
        order.reserve(N_);
        dfs.reserve(N_);
        dfs.push_back(root);
        par[root] = -2;
        while(!dfs.empty()) {
            int v = dfs.back(); dfs.pop_back();
            order.push_back(v);
            for(const auto &[to, _]: graph_[v]) {
                if(par[to] != -1) continue;
                par[to] = v;
                dfs.push_back(to);
            }
        }
        assert(static_cast<int>(order.size()) == N_ && "Rerooting::build: order.size() == N_");
        int len = 0;
        for(int t=N_-1; t>=0; t--) {
            int v = order[t];
            int deg = static_cast<int>(graph_[v].size());
            len = max(len, deg);
            dp_[v].resize(deg, DP::e());
            S sum = DP::e();
            for(int i=0; i<deg; i++) {
                const auto &[u, ew] = graph_[v][i];
                if(u == par[v]) continue;
                dp_[v][i] = DP::through(down_[u], ew, val_[v], val_[u]);
                sum = DP::op(sum, dp_[v][i]);
            }
            down_[v] = DP::add_root(sum, val_[v]);
        }
        vector<S> pref(len + 1, DP::e()), suff(len + 1, DP::e());
        for(int t=0; t<N_; t++) {
            int v = order[t];
            int deg = static_cast<int>(graph_[v].size());
            for(int i=0; i<deg; i++) {
                const auto& [u, ew] = graph_[v][i];
                if(u == par[v]) {
                    dp_[v][i] = DP::through(up_[v], ew, val_[v], val_[u]);
                    break;
                }
            }
            pref[0] = DP::e();
            for(int i=0; i<deg; i++) pref[i+1] = DP::op(pref[i], dp_[v][i]);
            suff[deg] = DP::e();
            for(int i=deg-1; i>=0; i--) suff[i] = DP::op(dp_[v][i], suff[i+1]);
            ans_[v] = DP::add_root(pref[deg], val_[v]);
            for(int i=0; i<deg; i++) {
                int u = graph_[v][i].first;
                if(u == par[v]) continue;
                S ex = DP::op(pref[i], suff[i+1]);
                up_[u] = DP::add_root(ex, val_[v]);
            }
        }
        built_ = true;
    }

    int size() const {
        return N_;
    }

    S get(int p) const {
        assert(built_ && "Rerooting::get: built_");
        assert(0 <= p && "Rerooting::get: 0 <= p");
        assert(p < N_ && "Rerooting::get: p < N_");
        return ans_[p];
    }

    void set(int p, const V& x) {
        assert(0 <= p && "Rerooting::set: 0 <= p");
        assert(p < N_ && "Rerooting::set: p < N_");
        val_[p] = x;
        built_ = false;
    }
};
