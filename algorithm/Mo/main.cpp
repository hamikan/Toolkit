#include <bits/stdc++.h>
using namespace std;

struct Mo {
private:
    int N_;
    vector<pair<int, int>> queries_;

public:
    explicit Mo(int N) : N_(N) {}

    int size() const {
        return N_;
    }

    int query_count() const {
        return static_cast<int>(queries_.size());
    }

    void add_query(int l, int r) {
        assert(0 <= l && "Mo::add_query: 0 <= l");
        assert(l <= r && "Mo::add_query: l <= r");
        assert(r <= N_ && "Mo::add_query: r <= N_");
        queries_.emplace_back(l, r);
    }

    template<typename Add, typename Del, typename Answer>
    void run(const Add& add, const Del& del, const Answer& answer) const {
        run(add, add, del, del, answer);
    }
    template<typename AddLeft, typename AddRight, typename DelLeft, typename DelRight, typename Answer>
    void run(const AddLeft& add_left, const AddRight& add_right, const DelLeft& del_left, const DelRight& del_right, const Answer& answer) const {
        int Q = static_cast<int>(queries_.size());
        if(Q == 0) return;
        int block_size = max(1, static_cast<int>(N_ / sqrt(Q)));
        vector<int> order(Q);
        iota(order.begin(), order.end(), 0);
        sort(order.begin(), order.end(), [&](int a, int b) {
            int block_a = queries_[a].first / block_size;
            int block_b = queries_[b].first / block_size;
            if(block_a != block_b) return block_a < block_b;
            if(block_a & 1) return queries_[a].second > queries_[b].second;
            else return queries_[a].second < queries_[b].second;
        });
        int l = 0, r = 0;
        for(int i : order) {
            auto [ql, qr] = queries_[i];
            while(l > ql) add_left(--l);
            while(r < qr) add_right(r++);
            while(l < ql) del_left(l++);
            while(r > qr) del_right(--r);
            answer(i);
        }
    }
};
