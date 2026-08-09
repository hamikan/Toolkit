#include <bits/stdc++.h>
using namespace std;

struct UnionFind {
private:
    int N_;
    int group_count_;
    vector<int> par_;
    vector<int> siz_;

public:
    explicit UnionFind(int N) {
        init(N);
    }

    void init(int N) {
        assert(0 <= N && "UnionFind::init: 0 <= N");
        N_ = N;
        par_.resize(N);
        iota(par_.begin(), par_.end(), 0);
        siz_.assign(N, 1);
        group_count_ = N;
    }

    int group_count() const {
        return group_count_;
    }

    int root(int x) {
        assert(0 <= x && "UnionFind::root: 0 <= x");
        assert(x < N_ && "UnionFind::root: x < N_");
        while(par_[x] != x) x = par_[x] = par_[par_[x]];
        return x;
    }

    bool unite(int x, int y) {
        assert(0 <= x && "UnionFind::unite: 0 <= x");
        assert(x < N_ && "UnionFind::unite: x < N_");
        assert(0 <= y && "UnionFind::unite: 0 <= y");
        assert(y < N_ && "UnionFind::unite: y < N_");
        x = root(x);
        y = root(y);
        if(x == y) return false;
        if(siz_[x] < siz_[y]) swap(x, y);
        siz_[x] += siz_[y];
        par_[y] = x;
        group_count_--;
        return true;
    }

    bool same(int x, int y) {
        assert(0 <= x && "UnionFind::same: 0 <= x");
        assert(x < N_ && "UnionFind::same: x < N_");
        assert(0 <= y && "UnionFind::same: 0 <= y");
        assert(y < N_ && "UnionFind::same: y < N_");
        return root(x) == root(y);
    }

    int size(int x) {
        assert(0 <= x && "UnionFind::size: 0 <= x");
        assert(x < N_ && "UnionFind::size: x < N_");
        return siz_[root(x)];
    }

    vector<vector<int>> groups() {
        vector ret(N_, vector<int>());
        for(int i=0; i<N_; i++) ret[root(i)].push_back(i);
        ret.erase(remove_if(ret.begin(), ret.end(), [](const vector<int>& group) {
            return group.empty();
        }), ret.end());
        return ret;
    }
};
