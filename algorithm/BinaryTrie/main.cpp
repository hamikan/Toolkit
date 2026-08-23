#include <bits/stdc++.h>
using namespace std;
using ll = long long;

template<typename T>
struct BinaryTrie {
private:
    static_assert(is_integral_v<T> && is_signed_v<T>, "BinaryTrie: is_integral_v<T> && is_signed_v<T>");

    struct Node {
        array<int, 2> next;
        int cnt;
        Node() : cnt(0) { next.fill(-1); }
    };

    int bit_len_;
    T lazy_;
    vector<Node> nodes_;

public:
    explicit BinaryTrie(int bit_len = numeric_limits<T>::digits, size_t capacity = 0) : bit_len_(bit_len), lazy_(T{}) {
        assert(1 <= bit_len_ && "BinaryTrie::BinaryTrie: 1 <= bit_len_");
        assert(bit_len_ <= numeric_limits<T>::digits && "BinaryTrie::BinaryTrie: bit_len_ <= numeric_limits<T>::digits");
        if(0 < capacity) nodes_.reserve(capacity);
        nodes_.emplace_back();
    }

    int size() const {
        return nodes_[0].cnt;
    }

    bool empty() const {
        return size() == 0;
    }

    void insert(T value) {
        assert(0 <= value && "BinaryTrie::insert: 0 <= value");
        assert((value >> bit_len_) == 0 && "BinaryTrie::insert: value < 2^bit_len_");
        value ^= lazy_;
        int v = 0;
        nodes_[v].cnt++;
        for(int i=bit_len_-1; i>=0; i--) {
            int bit = (value >> i) & 1;
            int to = nodes_[v].next[bit];
            if(to == -1) {
                to = static_cast<int>(nodes_.size());
                nodes_[v].next[bit] = to;
                nodes_.emplace_back();
            }
            v = to;
            nodes_[v].cnt++;
        }
    }

    int count(T value) const {
        assert(0 <= value && "BinaryTrie::count: 0 <= value");
        assert((value >> bit_len_) == 0 && "BinaryTrie::count: value < 2^bit_len_");
        value ^= lazy_;
        int v = 0;
        for(int i=bit_len_-1; i>=0; i--) {
            int bit = (value >> i) & 1;
            v = nodes_[v].next[bit];
            if(v == -1) return 0;
        }
        return nodes_[v].cnt;
    }

    void erase(T value) {
        assert(0 <= value && "BinaryTrie::erase: 0 <= value");
        assert((value >> bit_len_) == 0 && "BinaryTrie::erase: value < 2^bit_len_");
        if(count(value) == 0) return;
        value ^= lazy_;
        int v = 0;
        nodes_[v].cnt--;
        for(int i=bit_len_-1; i>=0; i--) {
            int bit = (value >> i) & 1;
            v = nodes_[v].next[bit];
            nodes_[v].cnt--;
        }
    }

    void xor_all(T x) {
        assert(0 <= x && "BinaryTrie::xor_all: 0 <= x");
        assert((x >> bit_len_) == 0 && "BinaryTrie::xor_all: x < 2^bit_len_");
        lazy_ ^= x;
    }

    void erase_all(T value) {
        assert(0 <= value && "BinaryTrie::erase_all: 0 <= value");
        assert((value >> bit_len_) == 0 && "BinaryTrie::erase_all: value < 2^bit_len_");
        int cnt = count(value);
        if(cnt == 0) return;
        value ^= lazy_;
        int v = 0;
        nodes_[v].cnt -= cnt;
        for(int i=bit_len_-1; i>=0; i--) {
            int bit = (value >> i) & 1;
            v = nodes_[v].next[bit];
            nodes_[v].cnt -= cnt;
        }
    }

    void clear() {
        lazy_ = T{};
        nodes_.clear();
        nodes_.emplace_back();
    }

    T kth(int k, T x = T{}) const {
        assert(1 <= k && "BinaryTrie::kth: 1 <= k");
        assert(k <= nodes_[0].cnt && "BinaryTrie::kth: k <= nodes_[0].cnt");
        assert(0 <= x && "BinaryTrie::kth: 0 <= x");
        assert((x >> bit_len_) == 0 && "BinaryTrie::kth: x < 2^bit_len_");
        x ^= lazy_;
        T ret = T{};
        int v = 0;
        for(int i=bit_len_-1; i>=0; i--) {
            int bit = (x >> i) & 1;
            int to = nodes_[v].next[bit];
            int cnt = to == -1 ? 0 : nodes_[to].cnt;
            if(k <= cnt) {
                v = to;
            } else {
                k -= cnt;
                v = nodes_[v].next[1-bit];
                ret |= T(1) << i;
            }
        }
        return ret;
    }

    int count_less(T bound, T x = T{}) const {
        assert(0 <= bound && "BinaryTrie::count_less: 0 <= bound");
        assert(0 <= x && "BinaryTrie::count_less: 0 <= x");
        assert((x >> bit_len_) == 0 && "BinaryTrie::count_less: x < 2^bit_len_");
        if((bound >> bit_len_) != 0) return size();
        x ^= lazy_;
        int v = 0, ret = 0;
        for(int i=bit_len_-1; i>=0; i--) {
            if(v == -1 || nodes_[v].cnt == 0) break;
            int bit = (x >> i) & 1;
            if((bound >> i) & 1) {
                int to = nodes_[v].next[bit];
                if(to != -1) ret += nodes_[to].cnt;
                v = nodes_[v].next[1-bit];
            } else {
                v = nodes_[v].next[bit];
            }
        }
        return ret;
    }

    T min_xor(T x, T lower = T{}) const {
        assert(0 <= x && "BinaryTrie::min_xor: 0 <= x");
        assert((x >> bit_len_) == 0 && "BinaryTrie::min_xor: x < 2^bit_len_");
        assert(0 <= lower && "BinaryTrie::min_xor: 0 <= lower");
        int k = count_less(lower, x) + 1;
        if(size() < k) return -1;
        return kth(k, x);
    }

    T max_xor(T x, T upper = numeric_limits<T>::max()) const {
        assert(0 <= x && "BinaryTrie::max_xor: 0 <= x");
        assert((x >> bit_len_) == 0 && "BinaryTrie::max_xor: x < 2^bit_len_");
        assert(0 <= upper && "BinaryTrie::max_xor: 0 <= upper");
        T limit = numeric_limits<T>::max() >> (numeric_limits<T>::digits - bit_len_);
        int k = limit <= upper ? size() : count_less(upper + 1, x);
        if(k == 0) return -1;
        return kth(k, x);
    }

    vector<T> to_vector() const {
        vector<T> ret;
        ret.reserve(size());
        for(int k=1; k<=size(); k++) ret.push_back(kth(k));
        return ret;
    }
};
