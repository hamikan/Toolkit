#include <bits/stdc++.h>
using namespace std;

struct RollingHash {
private:
    using u64 = uint64_t;
    using u128 = __uint128_t;

    static constexpr u64 mod_ = (1ULL << 61) - 1;

    static u64 add(u64 a, u64 b) {
        a += b;
        if(mod_ <= a) a -= mod_;
        return a;
    }

    static u64 sub(u64 a, u64 b) {
        return b <= a ? a - b : a + mod_ - b;
    }

    static u64 mul(u64 a, u64 b) {
        u128 x = u128(a) * b;
        u64 res = u64(x >> 61) + u64(x & mod_);
        if(mod_ <= res) res -= mod_;
        return res;
    }

    inline static const u64 base_ = [] {
        mt19937_64 rng(u64(chrono::steady_clock::now().time_since_epoch().count()));
        uniform_int_distribution<u64> dist(1ULL << 30, mod_ - 2);
        return dist(rng);
    }();
    inline static vector<u64> power_ = {1};

    static void ensure_power(int N) {
        while(static_cast<int>(power_.size()) <= N) power_.push_back(mul(power_.back(), base_));
    }

    int N_;
    vector<u64> pref_;

public:
    explicit RollingHash(const string& s) : N_(static_cast<int>(s.size())) {
        ensure_power(N_);
        pref_.assign(N_ + 1, u64{});
        for(int i=0; i<N_; i++) {
            pref_[i+1] = add(mul(pref_[i], base_), u64(static_cast<unsigned char>(s[i])) + 1);
        }
    }
    template<typename T>
    explicit RollingHash(const vector<T>& v) : N_(static_cast<int>(v.size())) {
        static_assert(is_integral_v<T>, "RollingHash::RollingHash: T is integral");
        ensure_power(N_);
        pref_.assign(N_ + 1, u64{});
        for(int i=0; i<N_; i++) {
            if constexpr(is_signed_v<T>) assert(0 <= v[i] && "RollingHash::RollingHash: 0 <= v[i]");
            u64 x = static_cast<u64>(v[i]);
            assert(x <= mod_ - 2 && "RollingHash::RollingHash: v[i] <= mod_ - 2");
            pref_[i+1] = add(mul(pref_[i], base_), x + 1);
        }
    }

    int size() const {
        return N_;
    }

    u64 hash(int l, int r) const {
        assert(0 <= l && "RollingHash::hash: 0 <= l");
        assert(l <= r && "RollingHash::hash: l <= r");
        assert(r <= N_ && "RollingHash::hash: r <= N_");
        return sub(pref_[r], mul(pref_[l], power_[r-l]));
    }

    bool equal(int l1, int r1, int l2, int r2) const { return equal(l1, r1, *this, l2, r2); }
    bool equal(int l1, int r1, const RollingHash& other, int l2, int r2) const {
        assert(0 <= l1 && "RollingHash::equal: 0 <= l1");
        assert(l1 <= r1 && "RollingHash::equal: l1 <= r1");
        assert(r1 <= N_ && "RollingHash::equal: r1 <= N_");
        assert(0 <= l2 && "RollingHash::equal: 0 <= l2");
        assert(l2 <= r2 && "RollingHash::equal: l2 <= r2");
        assert(r2 <= other.N_ && "RollingHash::equal: r2 <= other.N_");
        if(r1 - l1 != r2 - l2) return false;
        return hash(l1, r1) == other.hash(l2, r2);
    }

    static u64 concat(u64 h1, u64 h2, int len2) {
        assert(0 <= len2 && "RollingHash::concat: 0 <= len2");
        ensure_power(len2);
        return add(mul(h1, power_[len2]), h2);
    }

    int lcp(int i, int j) const { return lcp(i, N_, j, N_); }
    int lcp(int l1, int r1, int l2, int r2) const { return lcp(l1, r1, *this, l2, r2); }
    int lcp(int i, const RollingHash& other, int j) const { return lcp(i, N_, other, j, other.N_); }
    int lcp(int l1, int r1, const RollingHash& other, int l2, int r2) const {
        assert(0 <= l1 && "RollingHash::lcp: 0 <= l1");
        assert(l1 <= r1 && "RollingHash::lcp: l1 <= r1");
        assert(r1 <= N_ && "RollingHash::lcp: r1 <= N_");
        assert(0 <= l2 && "RollingHash::lcp: 0 <= l2");
        assert(l2 <= r2 && "RollingHash::lcp: l2 <= r2");
        assert(r2 <= other.N_ && "RollingHash::lcp: r2 <= other.N_");
        int ok = 0, ng = min(r1 - l1, r2 - l2) + 1;
        while(ng - ok > 1) {
            int mid = (ok + ng) >> 1;
            if(hash(l1, l1 + mid) == other.hash(l2, l2 + mid)) ok = mid;
            else ng = mid;
        }
        return ok;
    }
};
