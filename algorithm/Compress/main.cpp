#include <bits/stdc++.h>
using namespace std;

template <typename T>
struct Compress {
private:
    vector<T> vec_;
    bool built_;

public:
    Compress() : built_(false) {}
    explicit Compress(const vector<T>& v) : vec_(v), built_(false) {
        build();
    }

    void add(T x) {
        vec_.push_back(x);
        built_ = false;
    }

    void add(const vector<T>& v) {
        vec_.insert(vec_.end(), v.begin(), v.end());
        built_ = false;
    }

    void build() {
        sort(vec_.begin(), vec_.end());
        vec_.erase(unique(vec_.begin(), vec_.end()), vec_.end());
        built_ = true;
    }

    int size() const {
        assert(built_ && "Compress::size: built_");
        return static_cast<int>(vec_.size());
    }

    const vector<T>& list() const {
        assert(built_ && "Compress::list: built_");
        return vec_;
    }

    T val(int i) const {
        assert(built_ && "Compress::val: built_");
        assert(0 <= i && "Compress::val: 0 <= i");
        assert(i < static_cast<int>(vec_.size()) && "Compress::val: i < size()");
        return vec_[i];
    }

    bool contains(T x) const {
        assert(built_ && "Compress::contains: built_");
        auto it = std::lower_bound(vec_.begin(), vec_.end(), x);
        return it != vec_.end() && *it == x;
    }

    int pos(T x) const {
        assert(built_ && "Compress::pos: built_");
        int ret = lower_bound(x);
        assert((ret < static_cast<int>(vec_.size()) && vec_[ret] == x) && "Compress::pos: contains(x)");
        return ret;
    }

    int lower_bound(T x) const {
        assert(built_ && "Compress::lower_bound: built_");
        return static_cast<int>(std::lower_bound(vec_.begin(), vec_.end(), x) - vec_.begin());
    }

    int upper_bound(T x) const {
        assert(built_ && "Compress::upper_bound: built_");
        return static_cast<int>(std::upper_bound(vec_.begin(), vec_.end(), x) - vec_.begin());
    }
};
