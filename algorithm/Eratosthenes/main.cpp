#include <bits/stdc++.h>
using namespace std;
using ll = long long;

struct Eratosthenes {
private:
    int N_;
    vector<bool> is_prime_;
    vector<int> primes_;
    vector<int> spf_;

public:
    explicit Eratosthenes(int N) : N_(N), is_prime_(N + 1, true), spf_(N + 1, 0) {
        assert(2 <= N_ && "Eratosthenes::Eratosthenes: 2 <= N_");
        is_prime_[0] = is_prime_[1] = false;
        for(int i=2; i<=N_; i++) {
            if(is_prime_[i]) {
                spf_[i] = i;
                primes_.push_back(i);
            }
            for(int p : primes_) {
                if(N_ < ll(i) * p) break;
                is_prime_[i*p] = false;
                spf_[i*p] = p;
                if(i % p == 0) break;
            }
        }
    }

    bool is_prime(ll n) const {
        if(n < 0) return false;
        if(n <= N_) return is_prime_[n];
        assert(n < ll(N_ + 1) * (N_ + 1) && "Eratosthenes::is_prime: n < (N_ + 1) * (N_ + 1)");
        for(int p : primes_) {
            if(n < ll(p) * p) break;
            if(n % p == 0) return false;
        }
        return true;
    }

    const vector<int>& list() const {
        return primes_;
    }

    int prime_count(int n) const {
        assert(n <= N_ && "Eratosthenes::prime_count: n <= N_");
        return static_cast<int>(upper_bound(primes_.begin(), primes_.end(), n) - primes_.begin());
    }

    int divisor_count(ll n) const {
        assert(1 <= n && "Eratosthenes::divisor_count: 1 <= n");
        assert(n < ll(N_ + 1) * (N_ + 1) && "Eratosthenes::divisor_count: n < (N_ + 1) * (N_ + 1)");
        int ret = 1;
        for(auto [p, e] : factorize(n)) ret *= (e + 1);
        return ret;
    }

    vector<pair<ll, int>> factorize(ll n) const {
        assert(1 <= n && "Eratosthenes::factorize: 1 <= n");
        assert(n < ll(N_ + 1) * (N_ + 1) && "Eratosthenes::factorize: n < (N_ + 1) * (N_ + 1)");
        vector<pair<ll, int>> ret;
        if(N_ < n) {
            for(int p : primes_) {
                if(ll(p) * p > n) break;
                if(n % p == 0) {
                    int cnt = 0;
                    while(n % p == 0) { n /= p; cnt++; }
                    ret.emplace_back(p, cnt);
                }
            }
            if(1 < n) ret.emplace_back(n, 1);
            return ret;
        }
        while(1 < n) {
            int p = spf_[n], cnt = 0;
            while(n % p == 0) { n /= p; cnt++; }
            ret.emplace_back(p, cnt);
        }
        return ret;
    }

    vector<ll> divisors(ll n) const {
        assert(1 <= n && "Eratosthenes::divisors: 1 <= n");
        assert(n < ll(N_ + 1) * (N_ + 1) && "Eratosthenes::divisors: n < (N_ + 1) * (N_ + 1)");
        vector<ll> ret = {1};
        for(auto [p, e] : factorize(n)) {
            int sz = static_cast<int>(ret.size());
            ll pe = 1;
            for(int i=0; i<e; i++) {
                pe *= p;
                for(int j=0; j<sz; j++) ret.push_back(ret[j] * pe);
            }
        }
        sort(ret.begin(), ret.end());
        return ret;
    }

    ll euler_phi(ll n) const {
        assert(1 <= n && "Eratosthenes::euler_phi: 1 <= n");
        assert(n < ll(N_ + 1) * (N_ + 1) && "Eratosthenes::euler_phi: n < (N_ + 1) * (N_ + 1)");
        ll ret = n;
        for(auto [p, e] : factorize(n)) ret -= ret / p;
        return ret;
    }

    int mobius(ll n) const {
        assert(1 <= n && "Eratosthenes::mobius: 1 <= n");
        assert(n < ll(N_ + 1) * (N_ + 1) && "Eratosthenes::mobius: n < (N_ + 1) * (N_ + 1)");
        int ret = 1;
        for(auto [p, e] : factorize(n)) {
            if(e >= 2) return 0;
            ret = -ret;
        }
        return ret;
    }
};
