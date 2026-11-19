#include <bits/stdc++.h>
using namespace std;

template<typename T, typename V>
struct RangeMap {
private:
    using Iterator = typename map<T, pair<T, V>>::iterator;

    map<T, pair<T, V>> mp_;
    map<T, T> filled_;
    map<V, T> length_;
    T total_;
    V sum_;

    Iterator split(T x) {
        auto it = mp_.lower_bound(x);
        if(it == mp_.begin() || prev(it)->second.first <= x) return it;
        auto p = prev(it);
        pair<T, V> right = p->second;
        p->second.first = x;
        return mp_.emplace_hint(it, x, right);
    }

    Iterator add_range(Iterator hint, T l, T r, const V& v) {
        length_[v] += (r - l);
        sum_ += v * (r - l);
        return mp_.emplace_hint(hint, l, pair<T, V>{r, v});
    }

    Iterator remove_range(Iterator it) {
        auto len = length_.find(it->second.second);
        len->second -= (it->second.first - it->first);
        if(len->second == T{}) length_.erase(len);
        sum_ -= it->second.second * (it->second.first - it->first);
        return mp_.erase(it);
    }

    void merge(Iterator it) {
        if(it != mp_.begin()) {
            auto p = prev(it);
            if(p->second.first == it->first && p->second.second == it->second.second) {
                p->second.first = it->second.first;
                mp_.erase(it);
                it = p;
            }
        }
        auto nx = next(it);
        if(nx != mp_.end() && it->second.first == nx->first && it->second.second == nx->second.second) {
            it->second.first = nx->second.first;
            mp_.erase(nx);
        }
    }

    void fill(T l, T r) {
        auto it = filled_.lower_bound(l);
        if(it != filled_.begin() && l <= prev(it)->second) it = prev(it);
        while(it != filled_.end() && it->first <= r) {
            l = min(l, it->first);
            r = max(r, it->second);
            total_ -= (it->second - it->first);
            it = filled_.erase(it);
        }
        filled_[l] = r;
        total_ += (r - l);
    }

    void unfill(T l, T r) {
        auto it = filled_.lower_bound(l);
        if(it != filled_.begin() && l < prev(it)->second) it = prev(it);
        while(it != filled_.end() && it->first < r) {
            auto [L, R] = *it;
            it = filled_.erase(it);
            total_ -= (R - L);
            if(L < l) {
                filled_[L] = l;
                total_ += (l - L);
            }
            if(r < R) {
                filled_[r] = R;
                total_ += (R - r);
                break;
            }
        }
    }

public:
    RangeMap() : total_(T{}), sum_(V{}) {}

    bool empty() const {
        return mp_.empty();
    }

    int size() const {
        return static_cast<int>(mp_.size());
    }

    auto begin() const { return mp_.begin(); }
    auto end() const { return mp_.end(); }

    void assign(T l, T r, const V& v) {
        assert(l <= r && "RangeMap::assign: l <= r");
        if(l >= r) return;
        auto it = split(l), last = split(r);
        while(it != last) it = remove_range(it);
        merge(add_range(last, l, r, v));
        fill(l, r);
    }

    void insert(T l, T r, const V& v) {
        assert(l <= r && "RangeMap::insert: l <= r");
        if(l >= r) return;
        for(T x=mex(l); x<r; x=mex(x)) {
            auto it = mp_.lower_bound(x);
            T y = it == mp_.end() ? r : min(r, it->first);
            merge(add_range(it, x, y, v));
            x = y;
        }
        fill(l, r);
    }

    void erase(T l, T r) {
        assert(l <= r && "RangeMap::erase: l <= r");
        if(l >= r) return;
        auto it = split(l), last = split(r);
        while(it != last) it = remove_range(it);
        unfill(l, r);
    }

    V sum() const {
        return sum_;
    }

    V sum(const V& v) const {
        return v * length(v);
    }

    int kinds() const {
        return static_cast<int>(length_.size());
    }

    T length() const {
        return total_;
    }

    T length(const V& v) const {
        auto it = length_.find(v);
        return it == length_.end() ? T{} : it->second;
    }

    bool contains(T x) const {
        auto it = mp_.upper_bound(x);
        if(it == mp_.begin()) return false;
        return prev(it)->first <= x && x < prev(it)->second.first;
    }

    V get(T x) const {
        assert(contains(x) && "RangeMap::get: contains(x)");
        return prev(mp_.upper_bound(x))->second.second;
    }

    bool intersects(T l, T r) const {
        assert(l <= r && "RangeMap::intersects: l <= r");
        if(l >= r) return false;
        auto it = filled_.lower_bound(l);
        if(it != filled_.end() && it->first < r) return true;
        if(it == filled_.begin()) return false;
        return prev(it)->second > l;
    }

    bool covered(T l, T r) const {
        assert(l <= r && "RangeMap::covered: l <= r");
        if(l >= r) return true;
        auto it = filled_.upper_bound(l);
        if(it == filled_.begin()) return false;
        return prev(it)->first <= l && r <= prev(it)->second;
    }

    T mex(T x) const {
        auto it = filled_.upper_bound(x);
        if(it == filled_.begin()) return x;
        return max(x, prev(it)->second);
    }

    void debug() const {
        for(auto it=mp_.begin(); it!=mp_.end(); it++) {
            cout << "[" << it->first << ", " << it->second.first << "): " << it->second.second << " \n"[next(it) == mp_.end()];
        }
    }
};
