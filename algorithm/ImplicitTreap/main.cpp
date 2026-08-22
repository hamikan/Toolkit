#include <bits/stdc++.h>
using namespace std;

struct Monoid {
    struct S {
        long long val;
        int size;

        friend ostream& operator<<(ostream& os, const S& x) {
            return os << "{" << x.val << ", " << x.size << "}";
        }
    };
    static S op(const S& a, const S& b) {
        return {
            a.val + b.val,
            a.size + b.size
        };
    }
    static S e() {
        return {
            0,
            0
        };
    }
    struct F {
        long long data;

        friend ostream& operator<<(ostream& os, const F& f) {
            return os << "{" << f.data << "}";
        }
    };
    static S mapping(const F& f, const S& x) {
        return {
            x.val + f.data * x.size,
            x.size
        };
    }
    static F composition(const F& f, const F& g) {
        return {
            f.data + g.data
        };
    }
    static F id() {
        return {
            0
        };
    }
};

template<typename Monoid>
struct ImplicitTreap {
private:
    using S = typename Monoid::S;
    using F = typename Monoid::F;

    struct Node {
        S value;
        S acc;
        S racc;
        F lazy;
        uint64_t priority;
        int cnt;
        bool has_lazy;
        bool rev;
        Node *l;
        Node *r;
        template<typename... Args>
        Node(uint64_t p, Args&&... args) : value(forward<Args>(args)...), acc(value), racc(value), lazy(Monoid::id()), priority(p), cnt(1), has_lazy(false), rev(false), l(nullptr), r(nullptr) {}
    };
    using Tree = Node*;

    uint64_t priority_state_ = uint64_t(chrono::steady_clock::now().time_since_epoch().count()) | 1;
    Tree root_ = nullptr;

    static constexpr int block_size_ = 4096;
    Tree current_block_ = nullptr;
    int current_used_ = block_size_;

    uint64_t next_priority() {
        return (
            priority_state_ ^= priority_state_ << 13,
            priority_state_ ^= priority_state_ >> 7,
            priority_state_ ^= priority_state_ << 17
        );
    }

    template<typename... Args>
    Tree make_node(Args&&... args) {
        if(current_used_ == block_size_) {
            current_block_ = allocator<Node>{}.allocate(block_size_);
            current_used_ = 0;
        }
        Tree t = current_block_ + current_used_;
        construct_at(t, next_priority(), forward<Args>(args)...);
        current_used_++;
        return t;
    }

    static int cnt(Tree t) {
        return t ? t->cnt : 0;
    }

    static S acc(Tree t) {
        return t ? t->acc : Monoid::e();
    }

    static S racc(Tree t) {
        return t ? t->racc : Monoid::e();
    }

    static void update(Tree t) {
        if(!t) return;
        t->cnt = 1 + cnt(t->l) + cnt(t->r);
        t->acc = Monoid::op(acc(t->l), Monoid::op(t->value, acc(t->r)));
        t->racc = Monoid::op(racc(t->r), Monoid::op(t->value, racc(t->l)));
    }

    static void all_apply(Tree t, const F& f) {
        if(!t) return;
        t->value = Monoid::mapping(f, t->value);
        t->acc = Monoid::mapping(f, t->acc);
        t->racc = Monoid::mapping(f, t->racc);
        t->lazy = Monoid::composition(f, t->lazy);
        t->has_lazy = true;
    }

    static void all_reverse(Tree t) {
        if(!t) return;
        swap(t->l, t->r);
        swap(t->acc, t->racc);
        t->rev ^= 1;
    }

    static void push(Tree t) {
        if(!t) return;
        if(t->rev) {
            all_reverse(t->l);
            all_reverse(t->r);
            t->rev = false;
        }
        if(t->has_lazy) {
            all_apply(t->l, t->lazy);
            all_apply(t->r, t->lazy);
            t->lazy = Monoid::id();
            t->has_lazy = false;
        }
    }

    static void split(Tree t, int p, Tree& l, Tree& r) {
        if(!t) {
            l = r = nullptr;
            return;
        }
        push(t);
        int left_count = cnt(t->l);
        if(p <= left_count) {
            split(t->l, p, l, t->l);
            r = t;
        } else {
            split(t->r, p - left_count - 1, t->r, r);
            l = t;
        }
        update(t);
    }

    static void merge(Tree& t, Tree l, Tree r) {
        if(!l || !r) {
            t = l ? l : r;
            return;
        }
        if(l->priority > r->priority) {
            push(l);
            merge(l->r, l->r, r);
            t = l;
        } else {
            push(r);
            merge(r->l, l, r->l);
            t = r;
        }
        update(t);
    }

    static void insert_dfs(Tree& t, int p, Tree node) {
        if(!t) {
            t = node;
            return;
        }
        if(t->priority < node->priority) {
            split(t, p, node->l, node->r);
            t = node;
        } else {
            push(t);
            int left_count = cnt(t->l);
            if(p <= left_count) insert_dfs(t->l, p, node);
            else insert_dfs(t->r, p - left_count - 1, node);
        }
        update(t);
    }

    static void set_dfs(Tree t, int p, const S& x) {
        push(t);
        int left_count = cnt(t->l);
        if(p < left_count) set_dfs(t->l, p, x);
        else if(left_count < p) set_dfs(t->r, p - left_count - 1, x);
        else t->value = x;
        update(t);
    }

    static S query_dfs(Tree t, int l, int r) {
        if(l == 0 && r == cnt(t)) return t->acc;
        push(t);
        int left_count = cnt(t->l);
        S ret = Monoid::e();
        if(l < left_count) ret = Monoid::op(ret, query_dfs(t->l, l, min(r, left_count)));
        if(l <= left_count && left_count < r) ret = Monoid::op(ret, t->value);
        if(left_count + 1 < r) ret = Monoid::op(ret, query_dfs(t->r, max(0, l - left_count - 1), r - left_count - 1));
        return ret;
    }

    static void apply_dfs(Tree t, int p, const F& f) {
        push(t);
        int left_count = cnt(t->l);
        if(p < left_count) apply_dfs(t->l, p, f);
        else if(left_count < p) apply_dfs(t->r, p - left_count - 1, f);
        else t->value = Monoid::mapping(f, t->value);
        update(t);
    }
    static void apply_dfs(Tree t, int l, int r, const F& f) {
        if(l == 0 && r == cnt(t)) {
            all_apply(t, f);
            return;
        }
        push(t);
        int left_count = cnt(t->l);
        if(l < left_count) apply_dfs(t->l, l, min(r, left_count), f);
        if(l <= left_count && left_count < r) t->value = Monoid::mapping(f, t->value);
        if(left_count + 1 < r) apply_dfs(t->r, max(0, l - left_count - 1), r - left_count - 1, f);
        update(t);
    }

    static int max_right_dfs(Tree t, int l, auto& ok, S& prefix) {
        if(!t) return 0;
        int count = cnt(t);
        if(l == 0) {
            S next = Monoid::op(prefix, t->acc);
            if(ok(next)) {
                prefix = move(next);
                return count;
            }
        }
        push(t);
        int left_count = cnt(t->l);
        if(l < left_count) {
            int ret = max_right_dfs(t->l, l, ok, prefix);
            if(ret < left_count) return ret;
        }
        if(l <= left_count) {
            S next = Monoid::op(prefix, t->value);
            if(!ok(next)) return left_count;
            prefix = move(next);
        }
        return left_count + 1 + max_right_dfs(t->r, max(0, l - left_count - 1), ok, prefix);
    }

    static int min_left_dfs(Tree t, int r, auto& ok, S& suffix) {
        if(!t) return 0;
        int count = cnt(t);
        if(r == count) {
            S next = Monoid::op(t->acc, suffix);
            if(ok(next)) {
                suffix = move(next);
                return 0;
            }
        }
        push(t);
        int left_count = cnt(t->l);
        if(left_count + 1 < r) {
            int ret = min_left_dfs(t->r, r - left_count - 1, ok, suffix);
            if(0 < ret) return left_count + 1 + ret;
        }
        if(left_count < r) {
            S next = Monoid::op(t->value, suffix);
            if(!ok(next)) return left_count + 1;
            suffix = move(next);
        }
        return min_left_dfs(t->l, min(r, left_count), ok, suffix);
    }

    static void erase_dfs(Tree& t, int l, int r) {
        if(l == 0 && r == cnt(t)) {
            t = nullptr;
            return;
        }
        push(t);
        int left_count = cnt(t->l);
        if(r <= left_count) {
            erase_dfs(t->l, l, r);
        } else if(left_count < l) {
            erase_dfs(t->r, l - left_count - 1, r - left_count - 1);
        } else {
            if(l < left_count) erase_dfs(t->l, l, left_count);
            if(left_count + 1 < r) erase_dfs(t->r, 0, r - left_count - 1);
            merge(t, t->l, t->r);
            return;
        }
        update(t);
    }

    static void debug_dfs(Tree t, int depth, vector<vector<Tree>>& levels) {
        if(!t) return;
        if(depth == static_cast<int>(levels.size())) levels.emplace_back();
        levels[depth].push_back(t);
        debug_dfs(t->l, depth + 1, levels);
        debug_dfs(t->r, depth + 1, levels);
    }

public:
    ImplicitTreap() = default;
    explicit ImplicitTreap(int N, const S& x) : ImplicitTreap(vector<S>(N, x)) {}
    explicit ImplicitTreap(const vector<S>& v) {
        vector<Tree> stack;
        stack.reserve(v.size());
        for(const S& x : v) {
            Tree t = make_node(x);
            Tree last = nullptr;
            while(!stack.empty() && stack.back()->priority < t->priority) {
                update(stack.back());
                last = stack.back();
                stack.pop_back();
            }
            t->l = last;
            if(!stack.empty()) stack.back()->r = t;
            stack.push_back(t);
        }
        while(!stack.empty()) {
            update(stack.back());
            root_ = stack.back();
            stack.pop_back();
        }
    }

    int size() const {
        return cnt(root_);
    }

    void push_back(const S& x) { emplace_back(x); }
    template<typename... Args>
    void emplace_back(Args&&... args) {
        Tree t = make_node(forward<Args>(args)...);
        merge(root_, root_, t);
    }

    void push_front(const S& x) { emplace_front(x); }
    template<typename... Args>
    void emplace_front(Args&&... args) {
        Tree t = make_node(forward<Args>(args)...);
        merge(root_, t, root_);
    }

    void insert(int p, const S& x) {
        assert(0 <= p && "ImplicitTreap::insert: 0 <= p");
        assert(p <= size() && "ImplicitTreap::insert: p <= size()");
        insert_dfs(root_, p, make_node(x));
    }
    template<typename... Args>
    void emplace(int p, Args&&... args) {
        assert(0 <= p && "ImplicitTreap::emplace: 0 <= p");
        assert(p <= size() && "ImplicitTreap::emplace: p <= size()");
        insert_dfs(root_, p, make_node(forward<Args>(args)...));
    }

    void set(int p, const S& x) {
        assert(0 <= p && "ImplicitTreap::set: 0 <= p");
        assert(p < size() && "ImplicitTreap::set: p < size()");
        set_dfs(root_, p, x);
    }

    S get(int p) {
        assert(0 <= p && "ImplicitTreap::get: 0 <= p");
        assert(p < size() && "ImplicitTreap::get: p < size()");
        Tree t = root_;
        while(true) {
            push(t);
            int left_count = cnt(t->l);
            if(p < left_count) {
                t = t->l;
            } else if(left_count < p) {
                p -= left_count + 1;
                t = t->r;
            } else {
                return t->value;
            }
        }
    }

    S query(int l, int r) {
        assert(0 <= l && "ImplicitTreap::query: 0 <= l");
        assert(l <= r && "ImplicitTreap::query: l <= r");
        assert(r <= size() && "ImplicitTreap::query: r <= size()");
        if(l == r) return Monoid::e();
        return query_dfs(root_, l, r);
    }

    void apply(int p, const F& f) {
        assert(0 <= p && "ImplicitTreap::apply: 0 <= p");
        assert(p < size() && "ImplicitTreap::apply: p < size()");
        apply_dfs(root_, p, f);
    }
    void apply(int l, int r, const F& f) {
        assert(0 <= l && "ImplicitTreap::apply: 0 <= l");
        assert(l <= r && "ImplicitTreap::apply: l <= r");
        assert(r <= size() && "ImplicitTreap::apply: r <= size()");
        if(l == r) return;
        apply_dfs(root_, l, r, f);
    }

    void erase(int p) {
        assert(0 <= p && "ImplicitTreap::erase: 0 <= p");
        assert(p < size() && "ImplicitTreap::erase: p < size()");
        erase_dfs(root_, p, p + 1);
    }
    void erase(int l, int r) {
        assert(0 <= l && "ImplicitTreap::erase: 0 <= l");
        assert(l <= r && "ImplicitTreap::erase: l <= r");
        assert(r <= size() && "ImplicitTreap::erase: r <= size()");
        if(l == r) return;
        erase_dfs(root_, l, r);
    }

    void reverse(int l, int r) {
        assert(0 <= l && "ImplicitTreap::reverse: 0 <= l");
        assert(l <= r && "ImplicitTreap::reverse: l <= r");
        assert(r <= size() && "ImplicitTreap::reverse: r <= size()");
        if(l == r) return;
        Tree tl, tm, tr;
        split(root_, l, tl, tm);
        split(tm, r - l, tm, tr);
        all_reverse(tm);
        merge(tl, tl, tm);
        merge(root_, tl, tr);
    }

    void rotate(int l, int m, int r) {
        assert(0 <= l && "ImplicitTreap::rotate: 0 <= l");
        assert(l <= m && "ImplicitTreap::rotate: l <= m");
        assert(m <= r && "ImplicitTreap::rotate: m <= r");
        assert(r <= size() && "ImplicitTreap::rotate: r <= size()");
        if(l == m || m == r) return;
        Tree tl, tml, tmr, tr;
        split(root_, l, tl, tml);
        split(tml, m - l, tml, tmr);
        split(tmr, r - m, tmr, tr);
        merge(tl, tl, tmr);
        merge(tl, tl, tml);
        merge(root_, tl, tr);
    }

    int max_right(int l, auto ok) {
        assert(0 <= l && "ImplicitTreap::max_right: 0 <= l");
        assert(l <= size() && "ImplicitTreap::max_right: l <= size()");
        assert(ok(Monoid::e()) && "ImplicitTreap::max_right: ok(Monoid::e())");
        if(l == size()) return size();
        S prefix = Monoid::e();
        return max_right_dfs(root_, l, ok, prefix);
    }

    int min_left(int r, auto ok) {
        assert(0 <= r && "ImplicitTreap::min_left: 0 <= r");
        assert(r <= size() && "ImplicitTreap::min_left: r <= size()");
        assert(ok(Monoid::e()) && "ImplicitTreap::min_left: ok(Monoid::e())");
        if(r == 0) return 0;
        S suffix = Monoid::e();
        return min_left_dfs(root_, r, ok, suffix);
    }

    void debug() const {
        vector<vector<Tree>> levels;
        debug_dfs(root_, 0, levels);
        for(const auto& nodes : levels) {
            const int len = static_cast<int>(nodes.size());
            for(int i=0; i<len; i++) cout << nodes[i]->acc << " \n"[i + 1 == len];
        }
        for(const auto& nodes : levels) {
            const int len = static_cast<int>(nodes.size());
            for(int i=0; i<len; i++) cout << nodes[i]->lazy << " \n"[i + 1 == len];
        }
    }
};
