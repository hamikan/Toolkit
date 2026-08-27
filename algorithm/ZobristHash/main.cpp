#include <bits/stdc++.h>
using namespace std;

template<typename T>
struct ZobristHash {
private:
    vector<T> values_;
    vector<uint64_t> hashes_;

public:
    explicit ZobristHash(vector<T> values) : values_(move(values)) {
        sort(values_.begin(), values_.end());
        values_.erase(unique(values_.begin(), values_.end()), values_.end());
        mt19937_64 rng(static_cast<uint64_t>(chrono::steady_clock::now().time_since_epoch().count()));
        hashes_.resize(values_.size());
        for(uint64_t& hash : hashes_) do hash = rng(); while(hash == 0);
    }

    int size() const {
        return static_cast<int>(values_.size());
    }

    uint64_t get(const T& value) const {
        auto it = lower_bound(values_.begin(), values_.end(), value);
        assert(it != values_.end() && "ZobristHash::get: it != values_.end()");
        assert(*it == value && "ZobristHash::get: *it == value");
        int i = static_cast<int>(it - values_.begin());
        return hashes_[i];
    }

    int get_index(const T& value) const {
        auto it = lower_bound(values_.begin(), values_.end(), value);
        assert(it != values_.end() && "ZobristHash::get_index: it != values_.end()");
        assert(*it == value && "ZobristHash::get_index: *it == value");
        return static_cast<int>(it - values_.begin());
    }

    uint64_t get_hash(int i) const {
        assert(0 <= i && "ZobristHash::get_hash: 0 <= i");
        assert(i < size() && "ZobristHash::get_hash: i < size()");
        return hashes_[i];
    }
};
