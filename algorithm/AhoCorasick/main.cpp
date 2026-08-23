#include <bits/stdc++.h>
using namespace std;

template<int CHAR_SIZE = 26, char BASE_CHAR = 'a'>
struct AhoCorasick {
private:
    static_assert(0 < CHAR_SIZE, "AhoCorasick: 0 < CHAR_SIZE");

    struct Node {
        array<int, CHAR_SIZE> next;
        int fail;
        int output_link;
        vector<int> output;

        Node() : fail(0), output_link(-1) {
            next.fill(-1);
        }
    };

    vector<Node> nodes_;
    vector<string> patterns_;
    vector<int> order_;
    bool built_;

    static bool valid(char ch) {
        int c = ch - BASE_CHAR;
        return 0 <= c && c < CHAR_SIZE;
    }

    static bool valid(string_view s) {
        for(char ch : s) if(!valid(ch)) return false;
        return true;
    }

public:
    explicit AhoCorasick(size_t capacity = 0) : built_(false) {
        if(0 < capacity) nodes_.reserve(capacity);
        nodes_.emplace_back();
    }

    int insert(string_view s) {
        assert(!built_ && "AhoCorasick::insert: !built_");
        assert(valid(s) && "AhoCorasick::insert: valid(s)");
        assert(!s.empty() && "AhoCorasick::insert: !s.empty()");
        int id = static_cast<int>(patterns_.size());
        patterns_.emplace_back(s);
        int v = 0;
        for(char ch : patterns_.back()) {
            int c = ch - BASE_CHAR;
            if(nodes_[v].next[c] == -1) {
                nodes_[v].next[c] = static_cast<int>(nodes_.size());
                nodes_.emplace_back();
            }
            v = nodes_[v].next[c];
        }
        nodes_[v].output.push_back(id);
        return id;
    }

    void build() {
        assert(!built_ && "AhoCorasick::build: !built_");
        order_.reserve(nodes_.size());
        for(int c=0; c<CHAR_SIZE; c++) {
            int to = nodes_[0].next[c];
            if(to == -1) {
                nodes_[0].next[c] = 0;
            } else {
                nodes_[to].fail = 0;
                order_.push_back(to);
            }
        }
        for(size_t head=0; head<order_.size(); head++) {
            int v = order_[head];
            int f = nodes_[v].fail;
            if(!nodes_[f].output.empty()) nodes_[v].output_link = f;
            else nodes_[v].output_link = nodes_[f].output_link;
            for(int c=0; c<CHAR_SIZE; c++) {
                int to = nodes_[v].next[c];
                if(to == -1) {
                    nodes_[v].next[c] = nodes_[f].next[c];
                } else {
                    nodes_[to].fail = nodes_[f].next[c];
                    order_.push_back(to);
                }
            }
        }
        built_ = true;
    }

    int size() const {
        return static_cast<int>(patterns_.size());
    }

    int state_count() const {
        return static_cast<int>(nodes_.size());
    }

    const string& get(int id) const {
        assert(0 <= id && "AhoCorasick::get: 0 <= id");
        assert(id < static_cast<int>(patterns_.size()) && "AhoCorasick::get: id < patterns_.size()");
        return patterns_[id];
    }

    int move(int v, char ch) const {
        assert(built_ && "AhoCorasick::move: built_");
        assert(0 <= v && "AhoCorasick::move: 0 <= v");
        assert(v < static_cast<int>(nodes_.size()) && "AhoCorasick::move: v < nodes_.size()");
        assert(valid(ch) && "AhoCorasick::move: valid(ch)");
        int c = ch - BASE_CHAR;
        return nodes_[v].next[c];
    }

    vector<int> accepts(int v) const {
        assert(built_ && "AhoCorasick::accepts: built_");
        assert(0 <= v && "AhoCorasick::accepts: 0 <= v");
        assert(v < static_cast<int>(nodes_.size()) && "AhoCorasick::accepts: v < nodes_.size()");
        vector<int> ret;
        while(v != -1) {
            for(int id : nodes_[v].output) ret.push_back(id);
            v = nodes_[v].output_link;
        }
        return ret;
    }

    vector<vector<int>> match(string_view s) const {
        assert(built_ && "AhoCorasick::match: built_");
        assert(valid(s) && "AhoCorasick::match: valid(s)");
        vector<vector<int>> ret;
        ret.reserve(s.size());
        int v = 0;
        for(char ch : s) {
            int c = ch - BASE_CHAR;
            v = nodes_[v].next[c];
            ret.push_back(accepts(v));
        }
        return ret;
    }

    vector<int> count_matches(string_view s) const {
        assert(built_ && "AhoCorasick::count_matches: built_");
        assert(valid(s) && "AhoCorasick::count_matches: valid(s)");
        vector<int> cnt(nodes_.size());
        int state = 0;
        for(char ch : s) {
            int c = ch - BASE_CHAR;
            state = nodes_[state].next[c];
            cnt[state]++;
        }
        for(int i=static_cast<int>(order_.size())-1; i>=0; i--) {
            int v = order_[i];
            cnt[nodes_[v].fail] += cnt[v];
        }
        vector<int> ret(patterns_.size());
        for(int v=0; v<static_cast<int>(nodes_.size()); v++) {
            for(int id : nodes_[v].output) ret[id] = cnt[v];
        }
        return ret;
    }

    bool is_accept(int v) const {
        assert(built_ && "AhoCorasick::is_accept: built_");
        assert(0 <= v && "AhoCorasick::is_accept: 0 <= v");
        assert(v < static_cast<int>(nodes_.size()) && "AhoCorasick::is_accept: v < nodes_.size()");
        return !nodes_[v].output.empty() || nodes_[v].output_link != -1;
    }
};
