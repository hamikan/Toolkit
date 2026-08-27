#include <bits/stdc++.h>
using namespace std;

struct Semiring {
    using S = long long;
    static S add(const S& a, const S& b) { return max(a, b); }
    static S zero() { return numeric_limits<S>::lowest(); }
    static S mul(const S& a, const S& b) {
        if(a == zero() || b == zero()) return zero();
        return a + b;
    }
    static S one() { return 0; }
};

template<typename Semiring>
struct Matrix {
private:
    using S = typename Semiring::S;

    int H_;
    int W_;
    vector<vector<S>> matrix_;

public:
    explicit Matrix(int N) : Matrix(N, N) {}
    Matrix(int H, int W) : Matrix(H, W, Semiring::zero()) {}
    Matrix(int H, int W, const S& x) : H_(H), W_(W), matrix_(H, vector<S>(W, x)) {}
    explicit Matrix(vector<vector<S>> v) : H_(static_cast<int>(v.size())), W_(H_ == 0 ? 0 : static_cast<int>(v[0].size())), matrix_(move(v)) {}

    int height() const { return H_; }
    int width() const { return W_; }

    vector<S>& operator[](int i) {
        assert(0 <= i && "Matrix::operator[]: 0 <= i");
        assert(i < H_ && "Matrix::operator[]: i < H_");
        return matrix_[i];
    }

    const vector<S>& operator[](int i) const {
        assert(0 <= i && "Matrix::operator[]: 0 <= i");
        assert(i < H_ && "Matrix::operator[]: i < H_");
        return matrix_[i];
    }

    Matrix operator+(const Matrix& a) const {
        assert(H_ == a.H_ && "Matrix::operator+: H_ == a.H_");
        assert(W_ == a.W_ && "Matrix::operator+: W_ == a.W_");
        return Matrix(*this) += a;
    }
    Matrix operator*(const Matrix& a) const {
        assert(W_ == a.H_ && "Matrix::operator*: W_ == a.H_");
        return Matrix(*this) *= a;
    }

    vector<S> operator*(const vector<S>& v) const {
        assert(static_cast<int>(v.size()) == W_ && "Matrix::operator*: v.size() == W_");
        const S zero = Semiring::zero();
        vector<S> ret(H_, zero);
        for(int i=0; i<H_; i++) {
            for(int j=0; j<W_; j++) {
                if(matrix_[i][j] == zero || v[j] == zero) continue;
                ret[i] = Semiring::add(ret[i], Semiring::mul(matrix_[i][j], v[j]));
            }
        }
        return ret;
    }

    Matrix& operator+=(const Matrix& a) {
        assert(H_ == a.H_ && "Matrix::operator+=: H_ == a.H_");
        assert(W_ == a.W_ && "Matrix::operator+=: W_ == a.W_");
        for(int i=0; i<H_; i++) for(int j=0; j<W_; j++) matrix_[i][j] = Semiring::add(matrix_[i][j], a.matrix_[i][j]);
        return *this;
    }
    Matrix& operator*=(const Matrix& a) {
        assert(W_ == a.H_ && "Matrix::operator*=: W_ == a.H_");
        Matrix ret(H_, a.W_);
        const S zero = Semiring::zero();
        for(int i=0; i<H_; i++) {
            for(int k=0; k<W_; k++) {
                if(matrix_[i][k] == zero) continue;
                for(int j=0; j<a.W_; j++) {
                    if(a.matrix_[k][j] == zero) continue;
                    ret.matrix_[i][j] = Semiring::add(ret.matrix_[i][j], Semiring::mul(matrix_[i][k], a.matrix_[k][j]));
                }
            }
        }
        *this = move(ret);
        return *this;
    }

    bool operator==(const Matrix& a) const { return H_ == a.H_ && W_ == a.W_ && matrix_ == a.matrix_; }

    static Matrix identity(int N) {
        Matrix ret(N);
        for(int i=0; i<N; i++) ret.matrix_[i][i] = Semiring::one();
        return ret;
    }

    Matrix transpose() const {
        Matrix ret(W_, H_);
        for(int i=0; i<H_; i++) for(int j=0; j<W_; j++) ret.matrix_[j][i] = matrix_[i][j];
        return ret;
    }

    Matrix pow(long long k) const {
        assert(0 <= k && "Matrix::pow: 0 <= k");
        assert(H_ == W_ && "Matrix::pow: H_ == W_");
        Matrix ret = identity(H_);
        Matrix a = *this;
        while(0 < k) {
            if(k & 1) ret *= a;
            k >>= 1;
            if(0 < k) a *= a;
        }
        return ret;
    }

    friend istream& operator>>(istream& is, Matrix& self) {
        for(int i=0; i<self.H_; i++) for(int j=0; j<self.W_; j++) is >> self.matrix_[i][j];
        return is;
    }

    friend ostream& operator<<(ostream& os, const Matrix& self) {
        for(int i=0; i<self.H_; i++) for(int j=0; j<self.W_; j++) os << self.matrix_[i][j] << " \n"[j == self.W_ - 1];
        return os;
    }
};
