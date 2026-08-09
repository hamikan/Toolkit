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
    };
    static S e() {
        return {
            0
        };
    };
};

template<typename Monoid>
struct SegTree {
private:
    using S = typename Monoid::S;

    int N_;
    int siz_;
    vector<S> tree_;

    void update(int p) {
        tree_[p] = Monoid::op(tree_[p*2], tree_[p*2+1]);
    }

public:
    explicit SegTree(int N) : SegTree(vector<S>(N, Monoid::e())) {};
    explicit SegTree(int N, const S& x) : SegTree(vector<S>(N, x)) {}
    explicit SegTree(const vector<S>& v) : N_(static_cast<int>(v.size())), siz_(1) {
        while(siz_ < N_) siz_ <<= 1;
        tree_.assign(2 * siz_, Monoid::e());
        for(int i=0; i<N_; i++) tree_[siz_+i] = v[i];
        for(int i=siz_-1; i>0; i--) update(i);
    }

    int size() const {
        return N_;
    }

    void set(int p, const S& x) {
        assert(0 <= p && "SegTree::set: 0 <= p");
        assert(p < N_ && "SegTree::set: p < N_");
        p += siz_;
        tree_[p] = x;
        while(1 < p) {
            p >>= 1;
            update(p);
        }
    }

    S get(int p) const {
        assert(0 <= p && "SegTree::get: 0 <= p");
        assert(p < N_ && "SegTree::get: p < N_");
        return tree_[siz_ + p];
    }

    S query(int l, int r) const {
        assert(0 <= l && "SegTree::query: 0 <= l");
        assert(l <= r && "SegTree::query: l <= r");
        assert(r <= N_ && "SegTree::query: r <= N_");
        S sml = Monoid::e();
        S smr = Monoid::e();
        l += siz_, r += siz_;
        while(l < r) {
            if(l & 1) sml = Monoid::op(sml, tree_[l++]);
            if(r & 1) smr = Monoid::op(tree_[--r], smr);
            l >>= 1, r >>= 1;
        }
        return Monoid::op(sml, smr);
    }

    S all_query() const {
        return tree_[1];
    }

    int max_right(int l, auto ok) const {
        assert(0 <= l && "SegTree::max_right: 0 <= l");
        assert(l <= N_ && "SegTree::max_right: l <= N_");
        assert(ok(Monoid::e()) && "SegTree::max_right: ok(Monoid::e())");
        if(l == N_) return N_;
        l += siz_;
        S acc = Monoid::e();
        do {
            while((l & 1) == 0) l >>= 1;
            S next = Monoid::op(acc, tree_[l]);
            if(!ok(next)) {
                while(l < siz_) {
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

    int min_left(int r, auto ok) const {
        assert(0 <= r && "SegTree::min_left: 0 <= r");
        assert(r <= N_ && "SegTree::min_left: r <= N_");
        assert(ok(Monoid::e()) && "SegTree::min_left: ok(Monoid::e())");
        if(r == 0) return 0;
        r += siz_;
        S acc = Monoid::e();
        do {
            r--;
            while(r > 1 && (r & 1)) r >>= 1;
            S next = Monoid::op(tree_[r], acc);
            if(!ok(next)) {
                while(r < siz_) {
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

    void debug() const {
        for(int i=1; i<=2*siz_; i++) cout << tree_[i-1] << " \n"[(i & -i) == i];
    }
};
