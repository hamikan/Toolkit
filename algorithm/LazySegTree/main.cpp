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

template <typename Monoid>
struct LazySegTree {
private:
    using S = typename Monoid::S;
    using F = typename Monoid::F;

    int N_, siz_, log_;
    vector<S> tree_;
    vector<F> lazy_;

    void update(int k) { tree_[k] = Monoid::op(tree_[k<<1], tree_[(k<<1)+1]); }
    void all_apply(int k, const F& f) {
        tree_[k] = Monoid::mapping(f, tree_[k]);
        if(k < siz_) lazy_[k] = Monoid::composition(f, lazy_[k]);
    }
    void push(int k) {
        all_apply(k << 1, lazy_[k]);
        all_apply((k << 1) + 1, lazy_[k]);
        lazy_[k] = Monoid::id();
    }

public:
    explicit LazySegTree(int N, const S& x) : LazySegTree(vector<S>(N, x)) {}
    explicit LazySegTree(const vector<S>& v) : N_(static_cast<int>(v.size())), siz_(1), log_(0) {
        while(siz_ < N_) siz_ <<= 1, log_++;
        tree_.assign(siz_ << 1, Monoid::e());
        lazy_.assign(siz_, Monoid::id());
        for(int i=0; i<N_; i++) tree_[siz_+i] = v[i];
        for(int i=siz_-1; i>=1; i--) update(i);
    }

    int size() const {
        return N_;
    }

    void set(int p, const S& x) {
        assert(0 <= p && "LazySegTree::set: 0 <= p");
        assert(p < N_ && "LazySegTree::set: p < N_");
        p += siz_;
        for(int i=log_; i>=1; i--) push(p >> i);
        tree_[p] = x;
        for(int i=1; i<=log_; i++) update(p >> i);
    }

    S get(int p) {
        assert(0 <= p && "LazySegTree::get: 0 <= p");
        assert(p < N_ && "LazySegTree::get: p < N_");
        p += siz_;
        for(int i=log_; i>=1; i--) push(p >> i);
        return tree_[p];
    }

    S query(int l, int r) {
        assert(0 <= l && "LazySegTree::query: 0 <= l");
        assert(l <= r && "LazySegTree::query: l <= r");
        assert(r <= N_ && "LazySegTree::query: r <= N_");
        if(l == r) return Monoid::e();
        l += siz_, r += siz_;
        const int l_ctz = __builtin_ctz(l);
        const int r_ctz = __builtin_ctz(r);
        for(int i=log_; i>=1; i--) {
            if(l_ctz < i) push(l >> i);
            if(r_ctz < i) push((r - 1) >> i);
        }
        S sml = Monoid::e(), smr = Monoid::e();
        while(l < r) {
            if(l & 1) sml = Monoid::op(sml, tree_[l++]);
            if(r & 1) smr = Monoid::op(tree_[--r], smr);
            l >>= 1, r >>= 1;
        }
        return Monoid::op(sml, smr);
    }

    S all_query() const { return tree_[1]; }

    void apply(int p, const F& f) {
        assert(0 <= p && "LazySegTree::apply: 0 <= p");
        assert(p < N_ && "LazySegTree::apply: p < N_");
        p += siz_;
        for(int i=log_; i>=1; i--) push(p >> i);
        tree_[p] = Monoid::mapping(f, tree_[p]);
        for(int i=1; i<=log_; i++) update(p >> i);
    }
    void apply(int l, int r, const F& f) {
        assert(0 <= l && "LazySegTree::apply: 0 <= l");
        assert(l <= r && "LazySegTree::apply: l <= r");
        assert(r <= N_ && "LazySegTree::apply: r <= N_");
        if(l == r) return;
        l += siz_, r += siz_;
        const int l_ctz = __builtin_ctz(l);
        const int r_ctz = __builtin_ctz(r);
        for(int i=log_; i>=1; i--) {
            if(l_ctz < i) push(l >> i);
            if(r_ctz < i) push((r - 1) >> i);
        }
        const int tl = l, tr = r;
        while(l < r) {
            if(l & 1) all_apply(l++, f);
            if(r & 1) all_apply(--r, f);
            l >>= 1, r >>= 1;
        }
        l = tl, r = tr;
        for(int i=1; i<=log_; i++) {
            if(l_ctz < i) update(l >> i);
            if(r_ctz < i) update((r - 1) >> i);
        }
    }

    int max_right(int l, auto ok) {
        assert(0 <= l && "LazySegTree::max_right: 0 <= l");
        assert(l <= N_ && "LazySegTree::max_right: l <= N_");
        assert(ok(Monoid::e()) && "LazySegTree::max_right: ok(Monoid::e())");
        if(l == N_) return N_;
        l += siz_;
        for(int i=log_; i>=1; i--) push(l >> i);
        S acc = Monoid::e();
        do {
            while((l & 1) == 0) l >>= 1;
            S next = Monoid::op(acc, tree_[l]);
            if(!ok(next)) {
                while(l < siz_) {
                    push(l);
                    l <<= 1;
                    next = Monoid::op(acc, tree_[l]);
                    if(ok(next)) {
                        acc = move(next);
                        l++;
                    }
                }
                return l - siz_;
            }
            acc = move(next);
            l++;
        } while((l & -l) != l);
        return N_;
    }

    int min_left(int r, auto ok) {
        assert(0 <= r && "LazySegTree::min_left: 0 <= r");
        assert(r <= N_ && "LazySegTree::min_left: r <= N_");
        assert(ok(Monoid::e()) && "LazySegTree::min_left: ok(Monoid::e())");
        if(r == 0) return 0;
        r += siz_;
        for(int i=log_; i>=1; i--) push((r - 1) >> i);
        S acc = Monoid::e();
        do {
            r--;
            while(r > 1 && (r & 1)) r >>= 1;
            S next = Monoid::op(tree_[r], acc);
            if(!ok(next)) {
                while(r < siz_) {
                    push(r);
                    r = (r << 1) + 1;
                    next = Monoid::op(tree_[r], acc);
                    if(ok(next)) {
                        acc = move(next);
                        r--;
                    }
                }
                return r + 1 - siz_;
            }
            acc = move(next);
        } while((r & -r) != r);
        return 0;
    }

    friend ostream& operator<<(ostream& os, LazySegTree& seg) {
        os << "[";
        for(int k=1; k<seg.siz_; k++) seg.push(k);
        for(int i=0; i<seg.N_; i++) {
            os << seg.tree_[seg.siz_+i];
            if(i + 1 < seg.N_) os << ", ";
        }
        os << "]";
        return os;
    }

    void debug() const {
        for(int i=1; i<=2*siz_; i++) cout << tree_[i-1] << " \n"[(i & -i) == i];
        for(int i=1; i<=siz_; i++) cout << lazy_[i-1] << " \n"[(i & -i) == i];
    }
};
