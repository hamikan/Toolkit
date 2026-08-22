#include <bits/stdc++.h>
using namespace std;

struct Monoid {
    struct S {
        long long val;
        int size;

        friend ostream& operator<<(ostream& os, const S& x) {
            return os << "{" << x.val << ", " << x.size << "}";
        }
    };
    static S op(const S& a, const S& b) {
        return {
            a.val + b.val,
            a.size + b.size
        };
    }
    static S e() {
        return {
            0,
            0
        };
    }
    struct F {
        long long data;

        friend ostream& operator<<(ostream& os, const F& f) {
            return os << "{" << f.data << "}";
        }
    };
    static S mapping(const F& f, const S& x) {
        return {
            x.val + f.data * x.size,
            x.size
        };
    }
    static F composition(const F& f, const F& g) {
        return {
            f.data + g.data
        };
    }
    static F id() {
        return {
            0
        };
    }
};

template<typename Monoid>
struct PersistentLazySegTree {
private:
    using S = typename Monoid::S;
    using F = typename Monoid::F;

    struct Node {
        S data;
        F lazy;
        int left;
        int right;
    };

    int N_;
    int siz_;
    int log_;
    vector<Node> nodes_;
    vector<int> roots_;

    int make_node(const S& x, const F& f = Monoid::id(), int l = -1, int r = -1) {
        nodes_.emplace_back(x, f, l, r);
        return static_cast<int>(nodes_.size()) - 1;
    }

    int clone_node(int t) {
        nodes_.push_back(nodes_[t]);
        return static_cast<int>(nodes_.size()) - 1;
    }

    void all_apply(int t, const F& f) {
        nodes_[t].data = Monoid::mapping(f, nodes_[t].data);
        nodes_[t].lazy = Monoid::composition(f, nodes_[t].lazy);
    }

    void push(int t) {
        nodes_[t].left = clone_node(nodes_[t].left);
        nodes_[t].right = clone_node(nodes_[t].right);
        all_apply(nodes_[t].left, nodes_[t].lazy);
        all_apply(nodes_[t].right, nodes_[t].lazy);
        nodes_[t].lazy = Monoid::id();
    }

    int build_dfs(const vector<S>& v, int l, int r) {
        if(r - l == 1) return make_node(l < N_ ? v[l] : Monoid::e());
        int m = (l + r) >> 1;
        int lc = build_dfs(v, l, m);
        int rc = build_dfs(v, m, r);
        return make_node(Monoid::op(nodes_[lc].data, nodes_[rc].data), Monoid::id(), lc, rc);
    }

    int set_dfs(int t, int l, int r, int p, const S& x) {
        int nt = clone_node(t);
        if(r - l == 1) {
            nodes_[nt].data = x;
            nodes_[nt].lazy = Monoid::id();
            return nt;
        }
        push(nt);
        int m = (l + r) >> 1;
        if(p < m) nodes_[nt].left = set_dfs(nodes_[nt].left, l, m, p, x);
        else nodes_[nt].right = set_dfs(nodes_[nt].right, m, r, p, x);
        nodes_[nt].data = Monoid::op(nodes_[nodes_[nt].left].data, nodes_[nodes_[nt].right].data);
        return nt;
    }

    S get_dfs(int t, int l, int r, int p, const F& acc) const {
        if(r - l == 1) return Monoid::mapping(acc, nodes_[t].data);
        F next = Monoid::composition(acc, nodes_[t].lazy);
        int m = (l + r) >> 1;
        if(p < m) return get_dfs(nodes_[t].left, l, m, p, next);
        return get_dfs(nodes_[t].right, m, r, p, next);
    }

    S query_dfs(int t, int l, int r, int ql, int qr, const F& acc) const {
        if(qr <= l || r <= ql) return Monoid::e();
        if(ql <= l && r <= qr) return Monoid::mapping(acc, nodes_[t].data);
        F next = Monoid::composition(acc, nodes_[t].lazy);
        int m = (l + r) >> 1;
        return Monoid::op(query_dfs(nodes_[t].left, l, m, ql, qr, next), query_dfs(nodes_[t].right, m, r, ql, qr, next));
    }

    int apply_dfs(int t, int l, int r, int ql, int qr, const F& f) {
        if(qr <= l || r <= ql) return t;
        int nt = clone_node(t);
        if(ql <= l && r <= qr) {
            all_apply(nt, f);
            return nt;
        }
        push(nt);
        int m = (l + r) >> 1;
        nodes_[nt].left = apply_dfs(nodes_[nt].left, l, m, ql, qr, f);
        nodes_[nt].right = apply_dfs(nodes_[nt].right, m, r, ql, qr, f);
        nodes_[nt].data = Monoid::op(nodes_[nodes_[nt].left].data, nodes_[nodes_[nt].right].data);
        return nt;
    }

    int copy_dfs(int t, int s, int l, int r, int ql, int qr) {
        if(qr <= l || r <= ql) return t;
        if(ql <= l && r <= qr) return s;
        int nt = clone_node(t);
        int ns = clone_node(s);
        push(nt);
        push(ns);
        int m = (l + r) >> 1;
        nodes_[nt].left = copy_dfs(nodes_[nt].left, nodes_[ns].left, l, m, ql, qr);
        nodes_[nt].right = copy_dfs(nodes_[nt].right, nodes_[ns].right, m, r, ql, qr);
        nodes_[nt].data = Monoid::op(nodes_[nodes_[nt].left].data, nodes_[nodes_[nt].right].data);
        return nt;
    }

    int max_right_dfs(int t, int l, int r, int ql, auto& ok, S& acc, const F& acc_lazy) const {
        if(r <= ql) return r;
        if(ql <= l) {
            S next = Monoid::op(acc, Monoid::mapping(acc_lazy, nodes_[t].data));
            if(ok(next)) {
                acc = move(next);
                return r;
            }
            if(r - l == 1) return l;
        }
        F next_lazy = Monoid::composition(acc_lazy, nodes_[t].lazy);
        int m = (l + r) >> 1;
        int ret = max_right_dfs(nodes_[t].left, l, m, ql, ok, acc, next_lazy);
        if(ret < m) return ret;
        return max_right_dfs(nodes_[t].right, m, r, ql, ok, acc, next_lazy);
    }

    int min_left_dfs(int t, int l, int r, int qr, auto& ok, S& acc, const F& acc_lazy) const {
        if(qr <= l) return l;
        if(r <= qr) {
            S next = Monoid::op(Monoid::mapping(acc_lazy, nodes_[t].data), acc);
            if(ok(next)) {
                acc = move(next);
                return l;
            }
            if(r - l == 1) return r;
        }
        F next_lazy = Monoid::composition(acc_lazy, nodes_[t].lazy);
        int m = (l + r) >> 1;
        int ret = min_left_dfs(nodes_[t].right, m, r, qr, ok, acc, next_lazy);
        if(m < ret) return ret;
        return min_left_dfs(nodes_[t].left, l, m, qr, ok, acc, next_lazy);
    }

public:
    explicit PersistentLazySegTree(int N, const S& x) : PersistentLazySegTree(vector<S>(N, x)) {}
    explicit PersistentLazySegTree(const vector<S>& v) : N_(static_cast<int>(v.size())), siz_(1), log_(0) {
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
        assert(0 <= q && "PersistentLazySegTree::reserve_nodes: 0 <= q");
        nodes_.reserve(nodes_.size() + q * (12 * log_ + 1));
    }

    int set(int p, const S& x) { return set(latest_version(), p, x); }
    int set(int t, int p, const S& x) {
        assert(0 <= t && "PersistentLazySegTree::set: 0 <= t");
        assert(t < version_count() && "PersistentLazySegTree::set: t < version_count()");
        assert(0 <= p && "PersistentLazySegTree::set: 0 <= p");
        assert(p < N_ && "PersistentLazySegTree::set: p < N_");
        roots_.push_back(set_dfs(roots_[t], 0, siz_, p, x));
        return latest_version();
    }

    S get(int p) const { return get(latest_version(), p); }
    S get(int t, int p) const {
        assert(0 <= t && "PersistentLazySegTree::get: 0 <= t");
        assert(t < version_count() && "PersistentLazySegTree::get: t < version_count()");
        assert(0 <= p && "PersistentLazySegTree::get: 0 <= p");
        assert(p < N_ && "PersistentLazySegTree::get: p < N_");
        return get_dfs(roots_[t], 0, siz_, p, Monoid::id());
    }

    S query(int l, int r) const { return query(latest_version(), l, r); }
    S query(int t, int l, int r) const {
        assert(0 <= t && "PersistentLazySegTree::query: 0 <= t");
        assert(t < version_count() && "PersistentLazySegTree::query: t < version_count()");
        assert(0 <= l && "PersistentLazySegTree::query: 0 <= l");
        assert(l <= r && "PersistentLazySegTree::query: l <= r");
        assert(r <= N_ && "PersistentLazySegTree::query: r <= N_");
        return query_dfs(roots_[t], 0, siz_, l, r, Monoid::id());
    }
    S all_query() const { return all_query(latest_version()); }
    S all_query(int t) const {
        assert(0 <= t && "PersistentLazySegTree::all_query: 0 <= t");
        assert(t < version_count() && "PersistentLazySegTree::all_query: t < version_count()");
        return nodes_[roots_[t]].data;
    }

    int apply(int l, int r, const F& f) { return apply(latest_version(), l, r, f); }
    int apply(int t, int l, int r, const F& f) {
        assert(0 <= t && "PersistentLazySegTree::apply: 0 <= t");
        assert(t < version_count() && "PersistentLazySegTree::apply: t < version_count()");
        assert(0 <= l && "PersistentLazySegTree::apply: 0 <= l");
        assert(l <= r && "PersistentLazySegTree::apply: l <= r");
        assert(r <= N_ && "PersistentLazySegTree::apply: r <= N_");
        roots_.push_back(apply_dfs(roots_[t], 0, siz_, l, r, f));
        return latest_version();
    }

    int copy(int src, int l, int r) { return copy(latest_version(), src, l, r); }
    int copy(int base, int src, int l, int r) {
        assert(0 <= base && "PersistentLazySegTree::copy: 0 <= base");
        assert(base < version_count() && "PersistentLazySegTree::copy: base < version_count()");
        assert(0 <= src && "PersistentLazySegTree::copy: 0 <= src");
        assert(src < version_count() && "PersistentLazySegTree::copy: src < version_count()");
        assert(0 <= l && "PersistentLazySegTree::copy: 0 <= l");
        assert(l <= r && "PersistentLazySegTree::copy: l <= r");
        assert(r <= N_ && "PersistentLazySegTree::copy: r <= N_");
        roots_.push_back(copy_dfs(roots_[base], roots_[src], 0, siz_, l, r));
        return latest_version();
    }

    int max_right(int l, auto ok) const { return max_right(latest_version(), l, ok); }
    int max_right(int t, int l, auto ok) const {
        assert(0 <= t && "PersistentLazySegTree::max_right: 0 <= t");
        assert(t < version_count() && "PersistentLazySegTree::max_right: t < version_count()");
        assert(0 <= l && "PersistentLazySegTree::max_right: 0 <= l");
        assert(l <= N_ && "PersistentLazySegTree::max_right: l <= N_");
        assert(ok(Monoid::e()) && "PersistentLazySegTree::max_right: ok(Monoid::e())");
        if(l == N_) return N_;
        S acc = Monoid::e();
        return min(max_right_dfs(roots_[t], 0, siz_, l, ok, acc, Monoid::id()), N_);
    }

    int min_left(int r, auto ok) const { return min_left(latest_version(), r, ok); }
    int min_left(int t, int r, auto ok) const {
        assert(0 <= t && "PersistentLazySegTree::min_left: 0 <= t");
        assert(t < version_count() && "PersistentLazySegTree::min_left: t < version_count()");
        assert(0 <= r && "PersistentLazySegTree::min_left: 0 <= r");
        assert(r <= N_ && "PersistentLazySegTree::min_left: r <= N_");
        assert(ok(Monoid::e()) && "PersistentLazySegTree::min_left: ok(Monoid::e())");
        if(r == 0) return 0;
        S acc = Monoid::e();
        return min_left_dfs(roots_[t], 0, siz_, r, ok, acc, Monoid::id());
    }

    void debug() const { debug(latest_version()); }
    void debug(int t) const {
        assert(0 <= t && "PersistentLazySegTree::debug: 0 <= t");
        assert(t < version_count() && "PersistentLazySegTree::debug: t < version_count()");
        vector<int> order(2 * siz_);
        order[1] = roots_[t];
        for(int i=1; i<siz_; i++) {
            order[i<<1] = nodes_[order[i]].left;
            order[(i<<1)+1] = nodes_[order[i]].right;
        }
        for(int i=2; i<=2*siz_; i++) cout << nodes_[order[i-1]].data << " \n"[(i & -i) == i];
        for(int i=2; i<=siz_; i++) cout << nodes_[order[i-1]].lazy << " \n"[(i & -i) == i];
    }
};
