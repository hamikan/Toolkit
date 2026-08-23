#include <bits/stdc++.h>
using namespace std;

template<int CHAR_SIZE = 26, char BASE_CHAR = 'a'>
struct Trie {
private:
    static_assert(0 < CHAR_SIZE, "Trie: 0 < CHAR_SIZE");

    struct Node {
        array<int, CHAR_SIZE> next;
        int end;
        int cnt;
        Node() : end(0), cnt(0) { next.fill(-1); }
    };

    vector<Node> nodes_;

    string kth(int k, int v) const {
        string ret;
        while(true) {
            if(0 < nodes_[v].end) {
                if(k <= nodes_[v].end) return ret;
                k -= nodes_[v].end;
            }
            for(int c=0; c<CHAR_SIZE; c++) {
                int to = nodes_[v].next[c];
                if(to == -1) continue;
                if(k <= nodes_[to].cnt) {
                    ret += static_cast<char>(BASE_CHAR + c);
                    v = to;
                    break;
                }
                k -= nodes_[to].cnt;
            }
        }
    }

    void dfs(int v, string& cur, vector<string>& ret) const {
        for(int i=0; i<nodes_[v].end; i++) ret.push_back(cur);
        for(int c=0; c<CHAR_SIZE; c++) {
            int to = nodes_[v].next[c];
            if(to == -1 || nodes_[to].cnt == 0) continue;
            cur += static_cast<char>(BASE_CHAR + c);
            dfs(to, cur, ret);
            cur.pop_back();
        }
    }

    static bool valid(string_view s) {
        for(char ch : s) {
            int c = ch - BASE_CHAR;
            if(c < 0 || CHAR_SIZE <= c) return false;
        }
        return true;
    }

public:
    explicit Trie(size_t capacity = 0) {
        if(0 < capacity) nodes_.reserve(capacity);
        nodes_.emplace_back();
    }

    int size() const {
        return nodes_[0].cnt;
    }

    bool empty() const {
        return nodes_[0].cnt == 0;
    }

    void insert(string_view s) {
        assert(valid(s) && "Trie::insert: valid(s)");
        assert(!s.empty() && "Trie::insert: !s.empty()");
        int v = 0;
        nodes_[v].cnt++;
        for(char ch : s) {
            int c = ch - BASE_CHAR;
            if(nodes_[v].next[c] == -1) {
                nodes_[v].next[c] = static_cast<int>(nodes_.size());
                nodes_.emplace_back();
            }
            v = nodes_[v].next[c];
            nodes_[v].cnt++;
        }
        nodes_[v].end++;
    }

    void erase(string_view s) {
        assert(valid(s) && "Trie::erase: valid(s)");
        if(count(s) == 0) return;
        int v = 0;
        nodes_[v].cnt--;
        for(char ch : s) {
            int c = ch - BASE_CHAR;
            v = nodes_[v].next[c];
            nodes_[v].cnt--;
        }
        nodes_[v].end--;
    }

    void erase_all(string_view s) {
        assert(valid(s) && "Trie::erase_all: valid(s)");
        int cnt = count(s);
        if(cnt == 0) return;
        int v = 0;
        nodes_[v].cnt -= cnt;
        for(char ch : s) {
            int c = ch - BASE_CHAR;
            v = nodes_[v].next[c];
            nodes_[v].cnt -= cnt;
        }
        nodes_[v].end = 0;
    }

    void erase_prefix(string_view prefix) {
        assert(valid(prefix) && "Trie::erase_prefix: valid(prefix)");
        if(prefix.empty()) {
            clear();
            return;
        }
        int v = 0;
        for(char ch : prefix) {
            int c = ch - BASE_CHAR;
            if(nodes_[v].next[c] == -1) return;
            v = nodes_[v].next[c];
        }
        int cnt = nodes_[v].cnt;
        if(cnt == 0) return;
        v = 0;
        nodes_[v].cnt -= cnt;
        const int len = static_cast<int>(prefix.size());
        for(int i=0; i+1<len; i++) {
            int c = prefix[i] - BASE_CHAR;
            v = nodes_[v].next[c];
            nodes_[v].cnt -= cnt;
        }
        int c = prefix.back() - BASE_CHAR;
        nodes_[v].next[c] = -1;
    }

    void clear() {
        nodes_.clear();
        nodes_.emplace_back();
    }

    int count(string_view s) const {
        assert(valid(s) && "Trie::count: valid(s)");
        int v = 0;
        for(char ch : s) {
            int c = ch - BASE_CHAR;
            if(nodes_[v].next[c] == -1) return 0;
            v = nodes_[v].next[c];
        }
        return nodes_[v].end;
    }

    int count_prefixes(string_view s) const {
        assert(valid(s) && "Trie::count_prefixes: valid(s)");
        int v = 0, ret = 0;
        for(char ch : s) {
            int c = ch - BASE_CHAR;
            if(nodes_[v].next[c] == -1) break;
            v = nodes_[v].next[c];
            ret += nodes_[v].end;
        }
        return ret;
    }

    int count_starts_with(string_view prefix) const {
        assert(valid(prefix) && "Trie::count_starts_with: valid(prefix)");
        int v = 0;
        for(char ch : prefix) {
            int c = ch - BASE_CHAR;
            if(nodes_[v].next[c] == -1) return 0;
            v = nodes_[v].next[c];
        }
        return nodes_[v].cnt;
    }

    int count_less(string_view s) const {
        assert(valid(s) && "Trie::count_less: valid(s)");
        int v = 0, ret = 0;
        for(char ch : s) {
            int c = ch - BASE_CHAR;
            ret += nodes_[v].end;
            for(int i=0; i<c; i++) {
                int to = nodes_[v].next[i];
                if(to != -1) ret += nodes_[to].cnt;
            }
            if(nodes_[v].next[c] == -1) return ret;
            v = nodes_[v].next[c];
        }
        return ret;
    }

    int longest_prefix_length(string_view s) const {
        assert(valid(s) && "Trie::longest_prefix_length: valid(s)");
        int v = 0, ret = -1;
        for(int i=0; i<static_cast<int>(s.size()); i++) {
            int c = s[i] - BASE_CHAR;
            if(nodes_[v].next[c] == -1) break;
            v = nodes_[v].next[c];
            if(0 < nodes_[v].end) ret = i + 1;
        }
        return ret;
    }

    string kth(int k) const {
        assert(1 <= k && "Trie::kth: 1 <= k");
        assert(k <= nodes_[0].cnt && "Trie::kth: k <= nodes_[0].cnt");
        return kth(k, 0);
    }

    string kth_with_prefix(string_view prefix, int k) const {
        assert(valid(prefix) && "Trie::kth_with_prefix: valid(prefix)");
        assert(1 <= k && "Trie::kth_with_prefix: 1 <= k");
        int v = 0;
        for(char ch : prefix) {
            int c = ch - BASE_CHAR;
            if(nodes_[v].next[c] == -1) return "";
            v = nodes_[v].next[c];
        }
        if(nodes_[v].cnt < k) return "";
        return string(prefix) + kth(k, v);
    }

    string lcp() const {
        string ret;
        int v = 0;
        while(nodes_[v].end == 0) {
            int c = -1;
            for(int i=0; i<CHAR_SIZE; i++) {
                int to = nodes_[v].next[i];
                if(to == -1 || nodes_[to].cnt == 0) continue;
                if(c != -1) return ret;
                c = i;
            }
            if(c == -1) break;
            ret += static_cast<char>(BASE_CHAR + c);
            v = nodes_[v].next[c];
        }
        return ret;
    }

    vector<string> to_vector() const {
        vector<string> ret;
        ret.reserve(nodes_[0].cnt);
        string cur;
        dfs(0, cur, ret);
        return ret;
    }
};
