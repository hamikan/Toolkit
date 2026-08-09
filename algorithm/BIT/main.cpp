#include <bits/stdc++.h>
using namespace std;

struct Group {
    using S = long long;
    static S op(const S& a, const S& b) { return a + b; }
    static S inv(const S& x) { return -x; }
    static S e() { return 0; }
};

template<typename Group>
struct FenwickTree {
private:
    using S = typename Group::S;

    int N_;
    int siz_;
    vector<S> tree_;

public:
    explicit FenwickTree(int N) : FenwickTree(N, Group::e()) {}
    explicit FenwickTree(int N, const S& x) : FenwickTree(vector<S>(N, x)) {}
    explicit FenwickTree(const vector<S>& v) : N_(static_cast<int>(v.size())), siz_(1) {
        while(siz_ < N_) siz_ <<= 1;
        tree_.assign(N_ + 1, Group::e());
        for(int i=1; i<=N_; i++) {
            tree_[i] = Group::op(tree_[i], v[i-1]);
            const int p = i + (i & -i);
            if(p <= N_) tree_[p] = Group::op(tree_[p], tree_[i]);
        }
    }

    int size() const {
        return N_;
    }

    S get(int p) const {
        assert(0 <= p && "FenwickTree::get: 0 <= p");
        assert(p < N_ && "FenwickTree::get: p < N_");
        return query(p, p + 1);
    }

    void add(int p, const S& x) {
        assert(0 <= p && "FenwickTree::add: 0 <= p");
        assert(p < N_ && "FenwickTree::add: p < N_");
        const S now = get(p);
        apply(p, Group::op(now + x, Group::inv(now)));
    }

    void set(int p, const S& x) {
        assert(0 <= p && "FenwickTree::set: 0 <= p");
        assert(p < N_ && "FenwickTree::set: p < N_");
        apply(p, Group::op(x, Group::inv(get(p))));
    }

    void apply(int p, const S& x) {
        assert(0 <= p && "FenwickTree::apply: 0 <= p");
        assert(p < N_ && "FenwickTree::apply: p < N_");
        for(p++; p<=N_; p+=(p&-p)) tree_[p] = Group::op(tree_[p], x);
    }

    S query(int l, int r) const {
        assert(0 <= l && "FenwickTree::query: 0 <= l");
        assert(l <= r && "FenwickTree::query: l <= r");
        assert(r <= N_ && "FenwickTree::query: r <= N_");
        auto prefix = [this](int p) {
            S ret = Group::e();
            while(p > 0) {
                ret = Group::op(ret, tree_[p]);
                p -= p & -p;
            }
            return ret;
        };
        return Group::op(prefix(r), Group::inv(prefix(l)));
    }

    int lower_bound(S x) const {
        if(x <= Group::e()) return 0;
        int pos = 0;
        for(int k=siz_; k>0; k>>=1) {
            if(pos + k <= N_ && tree_[pos+k] < x) {
                x = Group::op(x, Group::inv(tree_[pos+k]));
                pos += k;
            }
        }
        return pos;
    }

    int upper_bound(S x) const {
        if(x < Group::e()) return 0;
        int pos = 0;
        for(int k=siz_; k>0; k>>=1) {
            if(pos + k <= N_ && tree_[pos+k] <= x) {
                x = Group::op(x, Group::inv(tree_[pos+k]));
                pos += k;
            }
        }
        return pos;
    }
};
