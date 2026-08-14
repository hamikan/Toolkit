#include <bits/stdc++.h>
using namespace std;

struct Monoid {
    struct S {
        long long val;

        friend ostream& operator<<(ostream& os, const S& x) {
            return os << "{" << x.val << "}";
        }
    };
    static S op(const S& a, const S& b) {
        return {
            a.val + b.val
        };
    }
    static S e() {
        return {
            0
        };
    }
};

template<typename Monoid>
struct PersistentSegTree {
private:
    using S = typename Monoid::S;

    struct Node {
        S data;
        int left;
        int right;
    };

    int N_;
    int siz_;
    int log_;
    vector<Node> nodes_;
    vector<int> roots_;

    int make_node(const S& x, int l = -1, int r = -1) {
        nodes_.emplace_back(x, l, r);
        return static_cast<int>(nodes_.size()) - 1;
    }

    int clone_node(int t) {
        nodes_.push_back(nodes_[t]);
        return static_cast<int>(nodes_.size()) - 1;
    }

    int build_dfs(const vector<S>& v, int l, int r) {
        if(r - l == 1) return make_node(l < N_ ? v[l] : Monoid::e());
        int m = (l + r) >> 1;
        int lc = build_dfs(v, l, m);
        int rc = build_dfs(v, m, r);
        return make_node(Monoid::op(nodes_[lc].data, nodes_[rc].data), lc, rc);
    }

    int set_dfs(int t, int l, int r, int p, const S& x) {
        if(r - l == 1) return make_node(x);
        int m = (l + r) >> 1;
        int lc = nodes_[t].left, rc = nodes_[t].right;
        if(p < m) lc = set_dfs(lc, l, m, p, x);
        else rc = set_dfs(rc, m, r, p, x);
        return make_node(Monoid::op(nodes_[lc].data, nodes_[rc].data), lc, rc);
    }

    S get_dfs(int t, int l, int r, int p) const {
        if(r - l == 1) return nodes_[t].data;
        int m = (l + r) >> 1;
        if(p < m) return get_dfs(nodes_[t].left, l, m, p);
        return get_dfs(nodes_[t].right, m, r, p);
    }

    S query_dfs(int t, int l, int r, int ql, int qr) const {
        if(qr <= l || r <= ql) return Monoid::e();
        if(ql <= l && r <= qr) return nodes_[t].data;
        int m = (l + r) >> 1;
        return Monoid::op(query_dfs(nodes_[t].left, l, m, ql, qr), query_dfs(nodes_[t].right, m, r, ql, qr));
    }

    int copy_dfs(int t, int s, int l, int r, int ql, int qr) {
        if(qr <= l || r <= ql) return t;
        if(ql <= l && r <= qr) return s;
        int nt = clone_node(t);
        int m = (l + r) >> 1;
        nodes_[nt].left = copy_dfs(nodes_[t].left, nodes_[s].left, l, m, ql, qr);
        nodes_[nt].right = copy_dfs(nodes_[t].right, nodes_[s].right, m, r, ql, qr);
        nodes_[nt].data = Monoid::op(nodes_[nodes_[nt].left].data, nodes_[nodes_[nt].right].data);
        return nt;
    }

    template<typename G>
    int max_right_dfs(int t, int l, int r, int ql, G& ok, S& acc) const {
        if(r <= ql) return r;
        if(ql <= l) {
            S next = Monoid::op(acc, nodes_[t].data);
            if(ok(next)) {
                acc = move(next);
                return r;
            }
            if(r - l == 1) return l;
        }
        int m = (l + r) >> 1;
        int ret = max_right_dfs(nodes_[t].left, l, m, ql, ok, acc);
        if(ret < m) return ret;
        return max_right_dfs(nodes_[t].right, m, r, ql, ok, acc);
    }

    template<typename G>
    int min_left_dfs(int t, int l, int r, int qr, G& ok, S& acc) const {
        if(qr <= l) return l;
        if(r <= qr) {
            S next = Monoid::op(nodes_[t].data, acc);
            if(ok(next)) {
                acc = move(next);
                return l;
            }
            if(r - l == 1) return r;
        }
        int m = (l + r) >> 1;
        int ret = min_left_dfs(nodes_[t].right, m, r, qr, ok, acc);
        if(m < ret) return ret;
        return min_left_dfs(nodes_[t].left, l, m, qr, ok, acc);
    }

public:
    explicit PersistentSegTree(int N) : PersistentSegTree(vector<S>(N, Monoid::e())) {}
    explicit PersistentSegTree(int N, const S& x) : PersistentSegTree(vector<S>(N, x)) {}
    explicit PersistentSegTree(const vector<S>& v) : N_(static_cast<int>(v.size())), siz_(1), log_(0) {
        while(siz_ < N_) siz_ <<= 1, log_++;
        nodes_.reserve(2 * siz_ - 1);
        roots_.push_back(build_dfs(v, 0, siz_));
    }

    int size() const {
        return N_;
    }

    int version_count() const {
        return static_cast<int>(roots_.size());
    }

    int latest_version() const {
        return version_count() - 1;
    }

    void reserve_nodes(int q) {
        assert(0 <= q && "PersistentSegTree::reserve_nodes: 0 <= q");
        nodes_.reserve(nodes_.size() + q * (log_ + 1));
    }

    int set(int p, const S& x) { return set(latest_version(), p, x); }
    int set(int t, int p, const S& x) {
        assert(0 <= t && "PersistentSegTree::set: 0 <= t");
        assert(t < version_count() && "PersistentSegTree::set: t < version_count()");
        assert(0 <= p && "PersistentSegTree::set: 0 <= p");
        assert(p < N_ && "PersistentSegTree::set: p < N_");
        roots_.push_back(set_dfs(roots_[t], 0, siz_, p, x));
        return latest_version();
    }

    S get(int p) const { return get(latest_version(), p); }
    S get(int t, int p) const {
        assert(0 <= t && "PersistentSegTree::get: 0 <= t");
        assert(t < version_count() && "PersistentSegTree::get: t < version_count()");
        assert(0 <= p && "PersistentSegTree::get: 0 <= p");
        assert(p < N_ && "PersistentSegTree::get: p < N_");
        return get_dfs(roots_[t], 0, siz_, p);
    }

    S query(int l, int r) const { return query(latest_version(), l, r); }
    S query(int t, int l, int r) const {
        assert(0 <= t && "PersistentSegTree::query: 0 <= t");
        assert(t < version_count() && "PersistentSegTree::query: t < version_count()");
        assert(0 <= l && "PersistentSegTree::query: 0 <= l");
        assert(l <= r && "PersistentSegTree::query: l <= r");
        assert(r <= N_ && "PersistentSegTree::query: r <= N_");
        return query_dfs(roots_[t], 0, siz_, l, r);
    }

    S all_query() const { return all_query(latest_version()); }
    S all_query(int t) const {
        assert(0 <= t && "PersistentSegTree::all_query: 0 <= t");
        assert(t < version_count() && "PersistentSegTree::all_query: t < version_count()");
        return nodes_[roots_[t]].data;
    }

    int copy(int src, int l, int r) { return copy(latest_version(), src, l, r); }
    int copy(int base, int src, int l, int r) {
        assert(0 <= base && "PersistentSegTree::copy: 0 <= base");
        assert(base < version_count() && "PersistentSegTree::copy: base < version_count()");
        assert(0 <= src && "PersistentSegTree::copy: 0 <= src");
        assert(src < version_count() && "PersistentSegTree::copy: src < version_count()");
        assert(0 <= l && "PersistentSegTree::copy: 0 <= l");
        assert(l <= r && "PersistentSegTree::copy: l <= r");
        assert(r <= N_ && "PersistentSegTree::copy: r <= N_");
        roots_.push_back(copy_dfs(roots_[base], roots_[src], 0, siz_, l, r));
        return latest_version();
    }

    int max_right(int l, auto ok) const { return max_right(latest_version(), l, ok); }
    int max_right(int t, int l, auto ok) const {
        assert(0 <= t && "PersistentSegTree::max_right: 0 <= t");
        assert(t < version_count() && "PersistentSegTree::max_right: t < version_count()");
        assert(0 <= l && "PersistentSegTree::max_right: 0 <= l");
        assert(l <= N_ && "PersistentSegTree::max_right: l <= N_");
        assert(ok(Monoid::e()) && "PersistentSegTree::max_right: ok(Monoid::e())");
        if(l == N_) return N_;
        S acc = Monoid::e();
        return min(max_right_dfs(roots_[t], 0, siz_, l, ok, acc), N_);
    }

    int min_left(int r, auto ok) const { return min_left(latest_version(), r, ok); }
    int min_left(int t, int r, auto ok) const {
        assert(0 <= t && "PersistentSegTree::min_left: 0 <= t");
        assert(t < version_count() && "PersistentSegTree::min_left: t < version_count()");
        assert(0 <= r && "PersistentSegTree::min_left: 0 <= r");
        assert(r <= N_ && "PersistentSegTree::min_left: r <= N_");
        assert(ok(Monoid::e()) && "PersistentSegTree::min_left: ok(Monoid::e())");
        if(r == 0) return 0;
        S acc = Monoid::e();
        return min_left_dfs(roots_[t], 0, siz_, r, ok, acc);
    }

    void debug() const { debug(latest_version()); }
    void debug(int t) const {
        assert(0 <= t && "PersistentSegTree::debug: 0 <= t");
        assert(t < version_count() && "PersistentSegTree::debug: t < version_count()");
        vector<int> level = {roots_[t]};
        while(!level.empty()) {
            vector<int> next;
            for(int i=0; i<static_cast<int>(level.size()); i++) {
                int node = level[i];
                cout << nodes_[node].data << " \n"[i + 1 == static_cast<int>(level.size())];
                if(nodes_[node].left != -1) {
                    next.push_back(nodes_[node].left);
                    next.push_back(nodes_[node].right);
                }
            }
            level = move(next);
        }
    }
};
