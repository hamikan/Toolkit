#include <bits/stdc++.h>
#include <atcoder/modint>
using namespace std;
using mint = atcoder::modint998244353;

struct Combinatorics {
private:
    int N_;
    vector<mint> fac_, ifac_;

public:
    explicit Combinatorics(int N) : N_(N), fac_(N + 1), ifac_(N + 1) {
        fac_[0] = 1;
        for(int i=1; i<=N_; i++) fac_[i] = fac_[i-1] * i;
        ifac_[N_] = fac_[N_].inv();
        for(int i=N_; i>0; i--) ifac_[i-1] = ifac_[i] * i;
    }

    mint C(int n, int r) const {
        if(n < 0 || r < 0 || n < r) return 0;
        assert(n <= N_ && "Combinatorics::C: n <= N_");
        return fac_[n] * ifac_[r] * ifac_[n-r];
    }

    mint P(int n, int r) const {
        if(n < 0 || r < 0 || n < r) return 0;
        assert(n <= N_ && "Combinatorics::P: n <= N_");
        return fac_[n] * ifac_[n-r];
    }

    mint H(int n, int r) const {
        if(n < 0 || r < 0) return 0;
        if(n == 0) return r == 0;
        assert(n + r - 1 <= N_ && "Combinatorics::H: n + r - 1 <= N_");
        return C(n + r - 1, r);
    }

    mint fac(int n) const {
        assert(0 <= n && "Combinatorics::fac: 0 <= n");
        assert(n <= N_ && "Combinatorics::fac: n <= N_");
        return fac_[n];
    }

    mint ifac(int n) const {
        assert(0 <= n && "Combinatorics::ifac: 0 <= n");
        assert(n <= N_ && "Combinatorics::ifac: n <= N_");
        return ifac_[n];
    }
};
