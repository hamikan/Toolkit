#include <bits/stdc++.h>
using namespace std;

template <typename T = long long>
struct RollbackUnionFind {
private:
    struct History {
        int x, y;
        int data_x, data_y;
        T potential_y;
    };

    int N_;
    int group_count_;
    int active_count_;

    vector<int> data_;
    vector<T> potential_;
    vector<History> history_;

    void ensure_node(int x) {
        if(data_[x] != INT_MAX) return;
        history_.emplace_back(-1, x, 0, 0, T{});
        data_[x] = -1;
        group_count_++;
        active_count_++;
    }

public:
    explicit RollbackUnionFind(int N, bool allocate = true) {
        init(N, allocate);
    }

    void init(int N, bool allocate = true) {
        assert(0 <= N && "RollbackUnionFind::init: 0 <= N");
        N_ = N;
        group_count_ = allocate ? N : 0;
        active_count_ = allocate ? N : 0;
        data_.assign(N, allocate ? -1 : INT_MAX);
        potential_.assign(N, T{});
        history_.clear();
    }

    int group_count() const {
        return group_count_;
    }

    int active_count() const {
        return active_count_;
    }

    int snapshot() const {
        return static_cast<int>(history_.size());
    }

    int root(int x) {
        assert(0 <= x && "RollbackUnionFind::root: 0 <= x");
        assert(x < N_ && "RollbackUnionFind::root: x < N_");
        ensure_node(x);
        while(data_[x] >= 0) x = data_[x];
        return x;
    }

    bool unite(int x, int y, T w = T{}) {
        assert(0 <= x && "RollbackUnionFind::unite: 0 <= x");
        assert(x < N_ && "RollbackUnionFind::unite: x < N_");
        assert(0 <= y && "RollbackUnionFind::unite: 0 <= y");
        assert(y < N_ && "RollbackUnionFind::unite: y < N_");
        int rx = root(x), ry = root(y);
        if(rx == ry) return false;
        if(data_[rx] > data_[ry]) {
            swap(rx, ry);
            swap(x, y);
            w = -w;
        }
        group_count_--;
        history_.emplace_back(rx, ry, data_[rx], data_[ry], potential_[ry]);
        data_[rx] += data_[ry];
        potential_[ry] = weight(x) + w - weight(y);
        data_[ry] = rx;
        return true;
    }

    T weight(int x) {
        assert(0 <= x && "RollbackUnionFind::weight: 0 <= x");
        assert(x < N_ && "RollbackUnionFind::weight: x < N_");
        ensure_node(x);
        T sum = T{};
        while(data_[x] >= 0) {
            sum += potential_[x];
            x = data_[x];
        }
        return sum;
    }

    bool same(int x, int y) {
        assert(0 <= x && "RollbackUnionFind::same: 0 <= x");
        assert(x < N_ && "RollbackUnionFind::same: x < N_");
        assert(0 <= y && "RollbackUnionFind::same: 0 <= y");
        assert(y < N_ && "RollbackUnionFind::same: y < N_");
        return root(x) == root(y);
    }

    int size(int x) {
        assert(0 <= x && "RollbackUnionFind::size: 0 <= x");
        assert(x < N_ && "RollbackUnionFind::size: x < N_");
        return -data_[root(x)];
    }

    T diff(int x, int y) {
        assert(0 <= x && "RollbackUnionFind::diff: 0 <= x");
        assert(x < N_ && "RollbackUnionFind::diff: x < N_");
        assert(0 <= y && "RollbackUnionFind::diff: 0 <= y");
        assert(y < N_ && "RollbackUnionFind::diff: y < N_");
        assert(same(x, y) && "RollbackUnionFind::diff: same(x, y)");
        return weight(y) - weight(x);
    }

    bool is_active(int x) const {
        assert(0 <= x && "RollbackUnionFind::is_active: 0 <= x");
        assert(x < N_ && "RollbackUnionFind::is_active: x < N_");
        return data_[x] != INT_MAX;
    }

    void activate(int x) {
        assert(0 <= x && "RollbackUnionFind::activate: 0 <= x");
        assert(x < N_ && "RollbackUnionFind::activate: x < N_");
        ensure_node(x);
    }

    void undo() {
        if(history_.empty()) return;
        auto [x, y, data_x, data_y, potential_y] = history_.back();
        history_.pop_back();
        if(x == -1) {
            data_[y] = INT_MAX;
            potential_[y] = T{};
            group_count_--;
            active_count_--;
            return;
        }
        group_count_++;
        data_[x] = data_x;
        data_[y] = data_y;
        potential_[y] = potential_y;
    }

    void rollback(int state) {
        assert(0 <= state && "RollbackUnionFind::rollback: 0 <= state");
        assert(state <= snapshot() && "RollbackUnionFind::rollback: state <= snapshot()");
        while(state < snapshot()) undo();
    }

    vector<vector<int>> groups() {
        vector ret(N_, vector<int>());
        for(int i=0; i<N_; i++) if(data_[i] != INT_MAX) ret[root(i)].push_back(i);
        ret.erase(remove_if(ret.begin(), ret.end(), [](const vector<int>& group) {
            return group.empty();
        }), ret.end());
        return ret;
    }
};
