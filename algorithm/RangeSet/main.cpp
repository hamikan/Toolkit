#include <bits/stdc++.h>
using namespace std;

template<typename T>
struct RangeSet {
private:
    map<T, T> mp_;
    T total_;

public:
    RangeSet() : total_(T{}) {}

    bool empty() const {
        return mp_.empty();
    }

    auto begin() const { return mp_.begin(); }
    auto end() const { return mp_.end(); }

    void insert(T l, T r) {
        assert(l <= r && "RangeSet::insert: l <= r");
        if(l >= r) return;
        auto it = mp_.lower_bound(l);
        if(it != mp_.begin() && l <= prev(it)->second) it = prev(it);
        while(it != mp_.end() && it->first <= r) {
            l = min(l, it->first);
            r = max(r, it->second);
            total_ -= (it->second - it->first);
            it = mp_.erase(it);
        }
        mp_[l] = r;
        total_ += (r - l);
    }

    void erase(T l, T r) {
        assert(l <= r && "RangeSet::erase: l <= r");
        if(l >= r) return;
        auto it = mp_.lower_bound(l);
        if(it != mp_.begin() && l < prev(it)->second) it = prev(it);
        while(it != mp_.end() && it->first < r) {
            auto [L, R] = *it;
            it = mp_.erase(it);
            total_ -= (R - L);
            if(L < l) {
                mp_[L] = l;
                total_ += (l - L);
            }
            if(r < R) {
                mp_[r] = R;
                total_ += (R - r);
                break;
            }
        }
    }

    T sum() const {
        return total_;
    }

    bool contains(T x) const {
        auto it = mp_.upper_bound(x);
        if(it == mp_.begin()) return false;
        return prev(it)->first <= x && x < prev(it)->second;
    }

    bool intersects(T l, T r) const {
        assert(l <= r && "RangeSet::intersects: l <= r");
        if(l >= r) return false;
        auto it = mp_.lower_bound(l);
        if(it != mp_.end() && it->first < r) return true;
        if(it == mp_.begin()) return false;
        return prev(it)->second > l;
    }

    bool covered(T l, T r) const {
        assert(l <= r && "RangeSet::covered: l <= r");
        if(l >= r) return true;
        auto it = mp_.upper_bound(l);
        if(it == mp_.begin()) return false;
        return prev(it)->first <= l && r <= prev(it)->second;
    }

    T mex(T x) const {
        auto it = mp_.upper_bound(x);
        if(it == mp_.begin()) return x;
        return max(x, prev(it)->second);
    }

    void debug() const {
        for(auto it=mp_.begin(); it!=mp_.end(); it++) {
            cout << "[" << it->first << ", " << it->second << ")" << " \n"[next(it) == mp_.end()];
        }
    }
};
