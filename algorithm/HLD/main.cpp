#include <bits/stdc++.h>
using namespace std;

struct HLD {
private:
    int N_;
    vector<vector<int>> graph_;
    vector<int> siz_;
    vector<int> dep_;
    vector<int> par_;
    vector<int> head_;
    vector<int> in_;
    vector<int> out_;
    vector<int> rev_;
    bool built_;

public:
    explicit HLD(int N) : N_(N), graph_(N), siz_(N), dep_(N), par_(N), head_(N), in_(N), out_(N), rev_(N), built_(false) {}
    explicit HLD(const vector<vector<int>>& graph, int root = 0) : N_(static_cast<int>(graph.size())), graph_(graph), siz_(N_), dep_(N_), par_(N_), head_(N_), in_(N_), out_(N_), rev_(N_), built_(false) {
        build(root);
    }

    void add_edge(int u, int v) {
        assert(0 <= u && "HLD::add_edge: 0 <= u");
        assert(u < N_ && "HLD::add_edge: u < N_");
        assert(0 <= v && "HLD::add_edge: 0 <= v");
        assert(v < N_ && "HLD::add_edge: v < N_");
        graph_[u].push_back(v);
        graph_[v].push_back(u);
        built_ = false;
    }

    void build(int root = 0) {
        assert(0 <= root && "HLD::build: 0 <= root");
        assert(root < N_ && "HLD::build: root < N_");
        fill(par_.begin(), par_.end(), -1);
        fill(dep_.begin(), dep_.end(), 0);
        vector<int> order, dfs;
        order.reserve(N_);
        dfs.reserve(N_);
        dfs.push_back(root);
        while(!dfs.empty()) {
            int v = dfs.back(); dfs.pop_back();
            order.push_back(v);
            for(int to : graph_[v]) {
                if(to == par_[v]) continue;
                par_[to] = v;
                dep_[to] = dep_[v] + 1;
                dfs.push_back(to);
            }
        }
        assert(static_cast<int>(order.size()) == N_ && "HLD::build: order.size() == N_");
        for(int t=N_-1; t>=0; t--) {
            int v = order[t];
            siz_[v] = 1;
            if(!graph_[v].empty() && graph_[v][0] == par_[v]) swap(graph_[v][0], graph_[v].back());
            for(int& to : graph_[v]) {
                if(to == par_[v]) continue;
                siz_[v] += siz_[to];
                if(siz_[graph_[v][0]] < siz_[to]) swap(graph_[v][0], to);
            }
        }
        int t = 0;
        dfs.clear();
        dfs.push_back(root);
        head_[root] = root;
        while(!dfs.empty()) {
            int v = dfs.back(); dfs.pop_back();
            in_[v] = t++;
            rev_[in_[v]] = v;
            for(int i=static_cast<int>(graph_[v].size())-1; i>=0; i--) {
                int to = graph_[v][i];
                if(to == par_[v]) continue;
                head_[to] = (to == graph_[v][0] ? head_[v] : to);
                dfs.push_back(to);
            }
        }
        for(int v : order) out_[v] = in_[v] + siz_[v];
        built_ = true;
    }

    int size() const {
        return N_;
    }

    int depth(int v) const {
        assert(built_ && "HLD::depth: built_");
        assert(0 <= v && "HLD::depth: 0 <= v");
        assert(v < N_ && "HLD::depth: v < N_");
        return dep_[v];
    }

    int parent(int v) const {
        assert(built_ && "HLD::parent: built_");
        assert(0 <= v && "HLD::parent: 0 <= v");
        assert(v < N_ && "HLD::parent: v < N_");
        return par_[v];
    }

    int subtree_size(int v) const {
        assert(built_ && "HLD::subtree_size: built_");
        assert(0 <= v && "HLD::subtree_size: 0 <= v");
        assert(v < N_ && "HLD::subtree_size: v < N_");
        return siz_[v];
    }

    int ancestor(int v, int k) const {
        assert(built_ && "HLD::ancestor: built_");
        assert(0 <= v && "HLD::ancestor: 0 <= v");
        assert(v < N_ && "HLD::ancestor: v < N_");
        assert(0 <= k && "HLD::ancestor: 0 <= k");
        if(dep_[v] < k) return -1;
        while(true) {
            int d = in_[v] - in_[head_[v]];
            if(k <= d) return rev_[in_[v] - k];
            k -= d + 1;
            v = par_[head_[v]];
        }
    }

    int lca(int u, int v) const {
        assert(built_ && "HLD::lca: built_");
        assert(0 <= u && "HLD::lca: 0 <= u");
        assert(u < N_ && "HLD::lca: u < N_");
        assert(0 <= v && "HLD::lca: 0 <= v");
        assert(v < N_ && "HLD::lca: v < N_");
        while(true) {
            if(in_[u] > in_[v]) swap(u, v);
            if(head_[u] == head_[v]) return u;
            v = par_[head_[v]];
        }
    }

    int lca(int u, int v, int root) const {
        assert(built_ && "HLD::lca: built_");
        assert(0 <= u && "HLD::lca: 0 <= u");
        assert(u < N_ && "HLD::lca: u < N_");
        assert(0 <= v && "HLD::lca: 0 <= v");
        assert(v < N_ && "HLD::lca: v < N_");
        assert(0 <= root && "HLD::lca: 0 <= root");
        assert(root < N_ && "HLD::lca: root < N_");
        int a = lca(u, v);
        int b = lca(u, root);
        int c = lca(v, root);
        if(a == b) return c;
        if(a == c) return b;
        return a;
    }

    bool is_ancestor(int u, int v) const {
        assert(built_ && "HLD::is_ancestor: built_");
        assert(0 <= u && "HLD::is_ancestor: 0 <= u");
        assert(u < N_ && "HLD::is_ancestor: u < N_");
        assert(0 <= v && "HLD::is_ancestor: 0 <= v");
        assert(v < N_ && "HLD::is_ancestor: v < N_");
        return in_[u] <= in_[v] && in_[v] < out_[u];
    }

    int jump(int u, int v, int k) const {
        assert(built_ && "HLD::jump: built_");
        assert(0 <= u && "HLD::jump: 0 <= u");
        assert(u < N_ && "HLD::jump: u < N_");
        assert(0 <= v && "HLD::jump: 0 <= v");
        assert(v < N_ && "HLD::jump: v < N_");
        assert(0 <= k && "HLD::jump: 0 <= k");
        int l = lca(u, v);
        int du = dep_[u] - dep_[l];
        int dv = dep_[v] - dep_[l];
        if(du + dv < k) return -1;
        if(k <= du) return ancestor(u, k);
        return ancestor(v, du + dv - k);
    }

    vector<int> path(int u, int v) const {
        assert(built_ && "HLD::path: built_");
        assert(0 <= u && "HLD::path: 0 <= u");
        assert(u < N_ && "HLD::path: u < N_");
        assert(0 <= v && "HLD::path: 0 <= v");
        assert(v < N_ && "HLD::path: v < N_");
        int l = lca(u, v);
        int du = dep_[u] - dep_[l];
        int dv = dep_[v] - dep_[l];
        vector<int> left, right;
        left.reserve(du + dv + 1);
        right.reserve(dv);
        while(u != l) {
            left.push_back(u);
            u = par_[u];
        }
        left.push_back(l);
        while(v != l) {
            right.push_back(v);
            v = par_[v];
        }
        reverse(right.begin(), right.end());
        left.insert(left.end(), right.begin(), right.end());
        return left;
    }

    bool is_on_path(int u, int v, int a) const {
        assert(built_ && "HLD::is_on_path: built_");
        assert(0 <= u && "HLD::is_on_path: 0 <= u");
        assert(u < N_ && "HLD::is_on_path: u < N_");
        assert(0 <= v && "HLD::is_on_path: 0 <= v");
        assert(v < N_ && "HLD::is_on_path: v < N_");
        assert(0 <= a && "HLD::is_on_path: 0 <= a");
        assert(a < N_ && "HLD::is_on_path: a < N_");
        int l = lca(u, v);
        return is_ancestor(l, a) && (is_ancestor(a, u) || is_ancestor(a, v));
    }

    int get_idx(int v) const {
        assert(built_ && "HLD::get_idx: built_");
        assert(0 <= v && "HLD::get_idx: 0 <= v");
        assert(v < N_ && "HLD::get_idx: v < N_");
        return in_[v];
    }

    int get_vertex(int i) const {
        assert(built_ && "HLD::get_vertex: built_");
        assert(0 <= i && "HLD::get_vertex: 0 <= i");
        assert(i < N_ && "HLD::get_vertex: i < N_");
        return rev_[i];
    }

    int get_edge_idx(int u, int v) const {
        assert(built_ && "HLD::get_edge_idx: built_");
        assert(0 <= u && "HLD::get_edge_idx: 0 <= u");
        assert(u < N_ && "HLD::get_edge_idx: u < N_");
        assert(0 <= v && "HLD::get_edge_idx: 0 <= v");
        assert(v < N_ && "HLD::get_edge_idx: v < N_");
        assert((par_[u] == v || par_[v] == u) && "HLD::get_edge_idx: par_[u] == v || par_[v] == u");
        if(dep_[u] > dep_[v]) return in_[u];
        return in_[v];
    }

    int get_dist(int u, int v) const {
        assert(built_ && "HLD::get_dist: built_");
        assert(0 <= u && "HLD::get_dist: 0 <= u");
        assert(u < N_ && "HLD::get_dist: u < N_");
        assert(0 <= v && "HLD::get_dist: 0 <= v");
        assert(v < N_ && "HLD::get_dist: v < N_");
        return dep_[u] + dep_[v] - 2 * dep_[lca(u, v)];
    }

    template<typename F>
    void for_each(int u, int v, const F& f, bool edge_query = false) const {
        assert(built_ && "HLD::for_each: built_");
        assert(0 <= u && "HLD::for_each: 0 <= u");
        assert(u < N_ && "HLD::for_each: u < N_");
        assert(0 <= v && "HLD::for_each: 0 <= v");
        assert(v < N_ && "HLD::for_each: v < N_");
        while(true) {
            if(in_[u] > in_[v]) swap(u, v);
            if(head_[u] == head_[v]) {
                f(in_[u] + edge_query, in_[v] + 1);
                break;
            }
            f(in_[head_[v]], in_[v] + 1);
            v = par_[head_[v]];
        }
    }

    template<typename T, typename OP, typename F>
    T query(int u, int v, const T& e, const OP& op, const F& f, bool edge_query = false) const {
        return query(u, v, e, op, f, f, edge_query);
    }
    template<typename T, typename OP, typename UP, typename DOWN>
    T query(int u, int v, const T& e, const OP& op, const UP& up, const DOWN& down, bool edge_query = false) const {
        assert(built_ && "HLD::query: built_");
        assert(0 <= u && "HLD::query: 0 <= u");
        assert(u < N_ && "HLD::query: u < N_");
        assert(0 <= v && "HLD::query: 0 <= v");
        assert(v < N_ && "HLD::query: v < N_");
        int top = lca(u, v);
        T res_up = e;
        T res_down = e;
        while(true) {
            if(head_[u] == head_[top]) {
                res_up = op(res_up, up(in_[top] + edge_query, in_[u] + 1));
                break;
            }
            res_up = op(res_up, up(in_[head_[u]], in_[u] + 1));
            u = par_[head_[u]];
        }
        while(v != top) {
            if(head_[v] == head_[top]) {
                res_down = op(down(in_[top] + 1, in_[v] + 1), res_down);
                break;
            }
            res_down = op(down(in_[head_[v]], in_[v] + 1), res_down);
            v = par_[head_[v]];
        }
        return op(res_up, res_down);
    }

    pair<int, int> subtree_range(int v, bool edge_query = false) const {
        assert(built_ && "HLD::subtree_range: built_");
        assert(0 <= v && "HLD::subtree_range: 0 <= v");
        assert(v < N_ && "HLD::subtree_range: v < N_");
        return {in_[v] + edge_query, out_[v]};
    }

    pair<vector<int>, vector<int>> auxiliary_tree(vector<int> vs) const {
        assert(built_ && "HLD::auxiliary_tree: built_");
        for(int v : vs) {
            assert(0 <= v && "HLD::auxiliary_tree: 0 <= v");
            assert(v < N_ && "HLD::auxiliary_tree: v < N_");
        }
        if(vs.empty()) return {{}, {}};
        auto comp = [&](int u, int v) {
            return in_[u] < in_[v];
        };
        sort(vs.begin(), vs.end(), comp);
        int M = static_cast<int>(vs.size());
        vs.reserve(2 * M);
        for(int i=M-1; i>0; i--) vs.push_back(lca(vs[i], vs[i-1]));
        sort(vs.begin(), vs.end(), comp);
        vs.erase(unique(vs.begin(), vs.end()), vs.end());
        M = static_cast<int>(vs.size());
        vector<int> parent_idx(M, -1), st;
        st.reserve(M);
        st.push_back(0);
        for(int i=1; i<M; i++) {
            while(true) {
                int u = vs[st.back()], v = vs[i];
                if(out_[u] >= out_[v]) break;
                st.pop_back();
            }
            parent_idx[i] = st.back();
            st.push_back(i);
        }
        return {vs, parent_idx};
    }
};
