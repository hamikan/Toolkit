#include <bits/stdc++.h>
using namespace std;

struct Group {
    using S = long long;
    static S op(const S& a, const S& b) { return a + b; }
    static S inv(const S& x) { return -x; }
    static S e() { return 0; }
};

template<typename Group>
struct CumulativeND {
private:
    using S = typename Group::S;

    int dim_;
    int total_;
    vector<int> siz_;
    vector<int> stride_;
    vector<S> data_;
    bool built_;

    int id(initializer_list<int> p) const {
        int ret = 0;
        int d = 0;
        for(int x : p) {
            ret += x * stride_[d];
            d++;
        }
        return ret;
    }

public:
    template<typename... Args>
    explicit CumulativeND(Args... siz) : CumulativeND(vector<int>{static_cast<int>(siz)...}) {}
    explicit CumulativeND(const vector<int>& siz) : dim_(static_cast<int>(siz.size())), total_(1), siz_(siz), stride_(dim_), built_(false) {
        assert(!siz_.empty() && "CumulativeND::CumulativeND: !siz_.empty()");
        assert(dim_ < 64 && "CumulativeND::CumulativeND: dim_ < 64");
        for(int d=dim_-1; d>=0; d--) {
            assert(0 < siz_[d] && "CumulativeND::CumulativeND: 0 < siz_[d]");
            assert(total_ <= INT_MAX / siz_[d] && "CumulativeND::CumulativeND: total_ overflow");
            stride_[d] = total_;
            total_ *= siz_[d];
        }
        data_.assign(total_, Group::e());
    }

    void set(initializer_list<int> p, const S& x) {
        assert(!built_ && "CumulativeND::set: !built_");
        assert(static_cast<int>(p.size()) == dim_ && "CumulativeND::set: p.size() == dim_");
        int d = 0;
        for(int c : p) {
            assert(0 <= c && "CumulativeND::set: 0 <= p[d]");
            assert(c < siz_[d] && "CumulativeND::set: p[d] < siz_[d]");
            d++;
        }
        data_[id(p)] = x;
    }

    void apply(initializer_list<int> p, const S& x) {
        assert(!built_ && "CumulativeND::apply: !built_");
        assert(static_cast<int>(p.size()) == dim_ && "CumulativeND::apply: p.size() == dim_");
        int d = 0;
        for(int c : p) {
            assert(0 <= c && "CumulativeND::apply: 0 <= p[d]");
            assert(c < siz_[d] && "CumulativeND::apply: p[d] < siz_[d]");
            d++;
        }
        int i = id(p);
        data_[i] = Group::op(data_[i], x);
    }

    void build() {
        assert(!built_ && "CumulativeND::build: !built_");
        for(int d=0; d<dim_; d++) {
            int width = stride_[d];
            int block = width * siz_[d];
            for(int base=0; base<total_; base+=block) {
                for(int x=1; x<siz_[d]; x++) {
                    int now = base + x * width;
                    for(int offset=0; offset<width; offset++) data_[now+offset] = Group::op(data_[now+offset], data_[now-width+offset]);
                }
            }
        }
        built_ = true;
    }

    template<typename... Args>
    S query(Args... args) const {
        assert(built_ && "CumulativeND::query: built_");
        assert(static_cast<int>(sizeof...(Args)) == 2 * dim_ && "CumulativeND::query: sizeof...(Args) == 2 * dim_");
        array<int, sizeof...(Args)> range = {static_cast<int>(args)...};
        for(int d=0; d<dim_; d++) {
            int l = range[2*d], r = range[2*d+1];
            assert(0 <= l && "CumulativeND::query: 0 <= l");
            assert(l <= r && "CumulativeND::query: l <= r");
            assert(r <= siz_[d] && "CumulativeND::query: r <= siz_[d]");
            if(l == r) return Group::e();
        }
        int p = 0;
        for(int d=0; d<dim_; d++) p += (range[2*d+1] - 1) * stride_[d];
        int invalid = 0;
        const uint64_t mask_count = uint64_t(1) << dim_;
        S ret = Group::e();
        for(uint64_t step=0; step<mask_count; step++) {
            uint64_t mask = step ^ (step >> 1);
            if(0 < step) {
                int d = __builtin_ctzll(static_cast<unsigned long long>(step));
                uint64_t bit = uint64_t(1) << d;
                if(mask & bit) {
                    if(range[2*d] == 0) invalid++;
                    else p += (range[2*d] - range[2*d+1]) * stride_[d];
                } else {
                    if(range[2*d] == 0) invalid--;
                    else p -= (range[2*d] - range[2*d+1]) * stride_[d];
                }
            }
            if(invalid == 0) {
                if(step & 1) ret = Group::op(ret, Group::inv(data_[p]));
                else ret = Group::op(ret, data_[p]);
            }
        }
        return ret;
    }

    S all_query() const {
        assert(built_ && "CumulativeND::all_query: built_");
        return data_.back();
    }

    template<typename... Args>
    S prefix(Args... args) const {
        assert(built_ && "CumulativeND::prefix: built_");
        assert(static_cast<int>(sizeof...(Args)) == dim_ && "CumulativeND::prefix: sizeof...(Args) == dim_");
        array<int, sizeof...(Args)> r = {static_cast<int>(args)...};
        int p = 0;
        for(int d=0; d<dim_; d++) {
            assert(0 <= r[d] && "CumulativeND::prefix: 0 <= r[d]");
            assert(r[d] <= siz_[d] && "CumulativeND::prefix: r[d] <= siz_[d]");
            if(r[d] == 0) return Group::e();
            p += (r[d] - 1) * stride_[d];
        }
        return data_[p];
    }
};
