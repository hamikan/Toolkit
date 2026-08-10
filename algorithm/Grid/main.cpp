#include <bits/stdc++.h>
using namespace std;

template<typename T>
struct Grid {
private:
    int H_;
    int W_;
    vector<vector<T>> grid_;

public:
    Grid(int H, int W, const T& val = T{}) : H_(H), W_(W), grid_(H, vector<T>(W, val)) {}
    explicit Grid(const vector<vector<T>>& g) : H_(static_cast<int>(g.size())), W_(static_cast<int>(g[0].size())), grid_(g) {}
    explicit Grid(const vector<string>& S) : H_(static_cast<int>(S.size())), W_(static_cast<int>(S[0].size())) {
        grid_.resize(H_);
        for(int i=0; i<H_; i++) for(int j=0; j<W_; j++) grid_[i].push_back(S[i][j]);
    }

    const vector<T>& operator[](int i) const {
        assert(0 <= i && "Grid::operator[]: 0 <= i");
        assert(i < H_ && "Grid::operator[]: i < H_");
        return grid_[i];
    }
    vector<T>& operator[](int i) {
        assert(0 <= i && "Grid::operator[]: 0 <= i");
        assert(i < H_ && "Grid::operator[]: i < H_");
        return grid_[i];
    }
    const T& operator[](pair<int,int> p) const {
        assert(0 <= p.first && "Grid::operator[]: 0 <= p.first");
        assert(p.first < H_ && "Grid::operator[]: p.first < H_");
        assert(0 <= p.second && "Grid::operator[]: 0 <= p.second");
        assert(p.second < W_ && "Grid::operator[]: p.second < W_");
        return grid_[p.first][p.second];
    }
    T& operator[](pair<int,int> p) {
        assert(0 <= p.first && "Grid::operator[]: 0 <= p.first");
        assert(p.first < H_ && "Grid::operator[]: p.first < H_");
        assert(0 <= p.second && "Grid::operator[]: 0 <= p.second");
        assert(p.second < W_ && "Grid::operator[]: p.second < W_");
        return grid_[p.first][p.second];
    }

    bool in(pair<int, int> p) const { return in(p.first, p.second); }
    bool in(int x, int y) const { return 0 <= x && x < H_ && 0 <= y && y < W_; }

    int id(pair<int, int> p) const { return id(p.first, p.second); }
    int id(int x, int y) const {
        assert(0 <= x && "Grid::id: 0 <= x");
        assert(x < H_ && "Grid::id: x < H_");
        assert(0 <= y && "Grid::id: 0 <= y");
        assert(y < W_ && "Grid::id: y < W_");
        return x * W_ + y;
    }

    pair<int, int> pos(int i) const {
        assert(0 <= i && "Grid::pos: 0 <= i");
        assert(i < H_ * W_ && "Grid::pos: i < H_ * W_");
        return {i / W_, i % W_};
    }

    pair<int, int> find(const T& val) const {
        for(int i=0; i<H_; i++) for(int j=0; j<W_; j++) if(grid_[i][j] == val) return {i, j};
        return {-1, -1};
    }
    vector<pair<int, int>> find_all(const T& val) const {
        vector<pair<int, int>> res;
        for(int i=0; i<H_; i++) for(int j=0; j<W_; j++) if(grid_[i][j] == val) res.emplace_back(i, j);
        return res;
    }

    vector<pair<int, int>> to(pair<int, int> p, int mode = 4) const { return to(p.first, p.second, mode); }
    vector<pair<int, int>> to(int x, int y, int mode = 4) const {
        assert(in(x, y) && "Grid::to: 0 <= x && x < H_ && 0 <= y && y < W_");
        assert((mode == 4 || mode == 8) && "Grid::to: mode == 4 || mode == 8");
        static const int dx[] = {0, 1, 0, -1, 1, 1, -1, -1};
        static const int dy[] = {1, 0, -1, 0, 1, -1, -1, 1};
        vector<pair<int, int>> res;
        for(int t=0; t<mode; t++) {
            int nx = x + dx[t];
            int ny = y + dy[t];
            if(in(nx, ny)) res.emplace_back(nx, ny);
        }
        return res;
    }

    friend istream& operator>>(istream& is, Grid<T>& self) {
        for(int i=0; i<self.H_; i++) for(int j=0; j<self.W_; j++) is >> self.grid_[i][j];
        return is;
    }

    friend ostream& operator<<(ostream& os, const Grid<T>& self) {
        for(int i=0; i<self.H_; i++) for(int j=0; j<self.W_; j++) os << self.grid_[i][j] << " \n"[j == self.W_ - 1];
        return os;
    }
};
