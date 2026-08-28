#include <bits/stdc++.h>
using namespace std;

template<typename T>
struct DoublyLinkedList {
private:
    struct Node {
        int prev;
        int next;
        T value;

        template<typename... Args>
        Node(int prev_id, int next_id, Args&&... args) : prev(prev_id), next(next_id), value(forward<Args>(args)...) {}
    };

    static constexpr int nil_ = 0;

    int siz_;
    vector<Node> nodes_;

    template<typename... Args>
    int insert_node(int next_id, Args&&... args) {
        int id = static_cast<int>(nodes_.size());
        int prev_id = nodes_[next_id].prev;
        nodes_.emplace_back(prev_id, next_id, forward<Args>(args)...);
        nodes_[prev_id].next = id;
        nodes_[next_id].prev = id;
        siz_++;
        return id;
    }

public:
    DoublyLinkedList() : siz_(0), nodes_(1, Node{nil_, nil_, T{}}) {}

    int size() const { return siz_; }
    bool empty() const { return siz_ == 0; }
    bool contains(int id) const { return 0 < id && id < static_cast<int>(nodes_.size()) && nodes_[id].prev != -1; }

    int begin() const { return nodes_[nil_].next; }
    int end() const { return nil_; }
    int next(int id) const {
        assert(contains(id) && "DoublyLinkedList::next: contains(id)");
        return nodes_[id].next;
    }
    int prev(int id) const {
        assert((id == end() || contains(id)) && "DoublyLinkedList::prev: id == end() || contains(id)");
        return nodes_[id].prev;
    }

    T& get(int id) {
        assert(contains(id) && "DoublyLinkedList::get: contains(id)");
        return nodes_[id].value;
    }
    const T& get(int id) const {
        assert(contains(id) && "DoublyLinkedList::get: contains(id)");
        return nodes_[id].value;
    }

    T& front() {
        assert(!empty() && "DoublyLinkedList::front: !empty()");
        return nodes_[begin()].value;
    }
    const T& front() const {
        assert(!empty() && "DoublyLinkedList::front: !empty()");
        return nodes_[begin()].value;
    }
    T& back() {
        assert(!empty() && "DoublyLinkedList::back: !empty()");
        return nodes_[prev(end())].value;
    }
    const T& back() const {
        assert(!empty() && "DoublyLinkedList::back: !empty()");
        return nodes_[prev(end())].value;
    }

    int insert_before(int id, const T& x) {
        assert((id == end() || contains(id)) && "DoublyLinkedList::insert_before: id == end() || contains(id)");
        return insert_node(id, x);
    }
    template<typename... Args>
    int emplace_before(int id, Args&&... args) {
        assert((id == end() || contains(id)) && "DoublyLinkedList::emplace_before: id == end() || contains(id)");
        return insert_node(id, forward<Args>(args)...);
    }
    int insert_after(int id, const T& x) {
        assert(contains(id) && "DoublyLinkedList::insert_after: contains(id)");
        return insert_node(nodes_[id].next, x);
    }
    template<typename... Args>
    int emplace_after(int id, Args&&... args) {
        assert(contains(id) && "DoublyLinkedList::emplace_after: contains(id)");
        return insert_node(nodes_[id].next, forward<Args>(args)...);
    }

    int push_front(const T& x) { return emplace_front(x); }
    template<typename... Args>
    int emplace_front(Args&&... args) { return insert_node(begin(), forward<Args>(args)...); }
    int push_back(const T& x) { return emplace_back(x); }
    template<typename... Args>
    int emplace_back(Args&&... args) { return insert_node(end(), forward<Args>(args)...); }

    int erase(int id) {
        assert(contains(id) && "DoublyLinkedList::erase: contains(id)");
        int prev_id = nodes_[id].prev;
        int next_id = nodes_[id].next;
        nodes_[prev_id].next = next_id;
        nodes_[next_id].prev = prev_id;
        nodes_[id].prev = -1;
        nodes_[id].next = -1;
        siz_--;
        return next_id;
    }

    void pop_front() {
        assert(!empty() && "DoublyLinkedList::pop_front: !empty()");
        erase(begin());
    }
    void pop_back() {
        assert(!empty() && "DoublyLinkedList::pop_back: !empty()");
        erase(prev(end()));
    }

    vector<T> to_vector() const {
        vector<T> ret;
        ret.reserve(siz_);
        for(int id=begin(); id!=end(); id=nodes_[id].next) ret.push_back(nodes_[id].value);
        return ret;
    }

    friend ostream& operator<<(ostream& os, const DoublyLinkedList& self) {
        for(int id=self.begin(); id!=self.end(); id=self.nodes_[id].next) os << self.nodes_[id].value << " \n"[self.nodes_[id].next == self.end()];
        return os;
    }
};
