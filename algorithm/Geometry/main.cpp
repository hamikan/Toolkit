#include <bits/stdc++.h>
using namespace std;

constexpr long double EPS = 1e-9L;
constexpr bool eq(long double a, long double b) noexcept { return fabsl(a - b) < EPS; }

template <typename T>
struct Vector {
    T x, y, z;
    constexpr Vector() : x(T{}), y(T{}), z(T{}) {}
    constexpr Vector(T x_, T y_, T z_ = T{}) : x(x_), y(y_), z(z_) {}

    template <typename U> constexpr auto operator+(const Vector<U>& v) const { return Vector<common_type_t<T, U>>(x + v.x, y + v.y, z + v.z); }
    template <typename U> constexpr auto operator-(const Vector<U>& v) const { return Vector<common_type_t<T, U>>(x - v.x, y - v.y, z - v.z); }
    template <typename U> constexpr auto operator*(U k) const { return Vector<common_type_t<T, U>>(x * k, y * k, z * k); }
    template <typename U> constexpr auto operator/(U k) const {
        assert(k != 0 && "Vector::operator/: k != 0");
        return Vector<common_type_t<T, U>>(x / k, y / k, z / k);
    }

    constexpr Vector& operator+=(const Vector& v) { x += v.x; y += v.y; z += v.z; return *this; }
    constexpr Vector& operator-=(const Vector& v) { x -= v.x; y -= v.y; z -= v.z; return *this; }
    constexpr Vector& operator*=(auto k) { x *= k; y *= k; z *= k; return *this; }
    constexpr Vector& operator/=(auto k) {
        assert(k != 0 && "Vector::operator/=: k != 0");
        x /= k, y /= k, z /= k;
        return *this;
    }

    constexpr Vector operator-() const { return {-x, -y, -z}; }

    template <typename U> friend constexpr auto operator*(U k, const Vector<T>& v) { return v * k; }

    constexpr bool operator==(const Vector& v) const { return x == v.x && y == v.y && z == v.z; }

    constexpr bool same(const Vector& v) const {
        return eq(x, v.x) && eq(y, v.y) && eq(z, v.z);
    }

    template<typename U> constexpr common_type_t<T, U> dot(const Vector<U>& v) const { return x * v.x + y * v.y + z * v.z; }
    template<typename U> constexpr auto cross(const Vector<U>& v) const {
        return Vector<common_type_t<T, U>>(
            y * v.z - z * v.y,
            z * v.x - x * v.z,
            x * v.y - y * v.x
        );
    }

    template<typename U> constexpr common_type_t<T, U> distance2(const Vector<U>& v) const { return (*this - v).norm2(); }
    template<typename U> long double distance(const Vector<U>& v) const { return (*this - v).norm(); }

    constexpr auto longer(const Vector& v) const { return (norm2() >= v.norm2() ? *this : v); }
    constexpr auto shorter(const Vector& v) const { return (norm2() <= v.norm2() ? *this : v); }

    template<typename U> long double angle(const Vector<U>& v) const {
        long double na = norm(), nb = v.norm();
        assert(EPS <= na && "Vector::angle: EPS <= na");
        assert(EPS <= nb && "Vector::angle: EPS <= nb");
        long double cos_theta = dot(v) / (na * nb);
        if(cos_theta > 1) cos_theta = 1.0L;
        if(cos_theta < -1) cos_theta = -1.0L;
        return acosl(cos_theta);
    }

    constexpr T norm2() const { return x * x + y * y + z * z; }
    long double norm() const { return sqrt((long double)norm2()); }
    Vector<long double> unit() const {
        long double n = norm();
        assert(EPS <= n && "Vector::unit: EPS <= n");
        return *this / n;
    }

    bool is_zero() const {
        return norm() < EPS;
    }

    friend ostream& operator<<(ostream& os, const Vector& v) {
        os << "<" << v.x << ", " << v.y << ", " << v.z << ">";
        return os;
    }
};

template <typename T>
struct Point {
    T x, y, z;
    constexpr Point() : x(T{}), y(T{}), z(T{}) {}
    constexpr Point(T x_, T y_, T z_ = T{}) : x(x_), y(y_), z(z_) {}

    constexpr bool operator<(const Point& p) const {
        if(x != p.x) return x < p.x;
        if(y != p.y) return y < p.y;
        return z < p.z;
    }
    constexpr bool operator>(const Point& p) const { return p < *this; }
    constexpr bool operator<=(const Point& p) const { return !(p < *this); }
    constexpr bool operator>=(const Point& p) const { return !(*this < p); }
    constexpr bool operator==(const Point& p) const { return x == p.x && y == p.y && z == p.z; }

    constexpr bool same(const Point& p) const { return eq(x, p.x) && eq(y, p.y) && eq(z, p.z); }

    template <typename U> constexpr auto operator-(const Point<U>& p) const { return Vector<common_type_t<T, U>>(x - p.x, y - p.y, z - p.z); }
    template <typename U> constexpr auto operator+(const Vector<U>& v) const { return Point<common_type_t<T, U>>(x + v.x, y + v.y, z + v.z); }
    template <typename U> constexpr auto operator-(const Vector<U>& v) const { return Point<common_type_t<T, U>>(x - v.x, y - v.y, z - v.z); }

    template <typename U> constexpr Point& operator+=(const Vector<U>& v) { x += v.x; y += v.y; z += v.z; return *this; }
    template <typename U> constexpr Point& operator-=(const Vector<U>& v) { x -= v.x; y -= v.y; z -= v.z; return *this; }

    template<typename U> constexpr auto distance2(const Point<U>& p) const { return (*this - p).norm2(); }
    template<typename U> long double distance(const Point<U>& p) const { return (*this - p).norm(); }

    friend ostream& operator<<(ostream& os, const Point& p) {
        os << "(" << p.x << ", " << p.y << ", " << p.z << ")";
        return os;
    }
};

template<typename T>
struct Segment {
    Point<T> a, b;

    Segment(const Point<T>& a_, const Point<T>& b_) : a(a_), b(b_) {}

    Vector<T> direction() const {
        return b - a;
    }

    T length2() const {
        return direction().norm2();
    }

    long double length() const {
        return direction().norm();
    }

    auto mid() const {
        return a + direction() * 0.5L;
    }

    auto at(long double t) const {
        assert(-EPS <= t && "Segment::at: -EPS <= t");
        assert(t <= 1.0L + EPS && "Segment::at: t <= 1 + EPS");
        t = clamp(t, 0.0L, 1.0L);
        return a + direction() * t;
    }

    template<typename U>
    int side(const Point<U>& p) const {
        assert(!is_point() && "Segment::side: !is_point()");
        const long double value = static_cast<long double>(direction().cross(p - a).z);
        if(fabsl(value) < EPS * length()) return 0;
        return 0 < value ? 1 : -1;
    }

    template<typename U>
    auto closest(const Point<U>& p) const {
        const Vector<T> d = direction();
        if(is_point()) return a + d * 0.0L;
        const auto ap = p - a;
        long double t = static_cast<long double>(d.dot(ap)) / static_cast<long double>(length2());
        t = clamp(t, 0.0L, 1.0L);
        return a + d * t;
    }

    template<typename U>
    long double distance(const Point<U>& p) const {
        return (p - closest(p)).norm();
    }

    template<typename U>
    auto project(const Point<U>& p) const {
        const Vector<T> d = direction();
        if(is_point()) return a + d * 0.0L;
        const auto ap = p - a;
        const long double t = static_cast<long double>(d.dot(ap)) / static_cast<long double>(length2());
        return a + d * t;
    }

    template<typename U>
    long double line_distance(const Point<U>& p) const {
        return p.distance(project(p));
    }

    template<typename U>
    bool on_segment(const Point<U>& p) const {
        if(is_point()) return (p - a).norm() < EPS;
        const Vector<T> d = direction();
        const auto ap = p - a;
        if(EPS * length() < d.cross(ap).norm()) return false;
        const long double t = static_cast<long double>(d.dot(ap)) / static_cast<long double>(length2());
        return -EPS <= t && t <= 1.0L + EPS;
    }

    template<typename U>
    long double triangle_area(const Point<U>& p) const {
        return direction().cross(p - a).norm() * 0.5L;
    }

    bool is_point() const {
        return length() < EPS;
    }

    friend ostream& operator<<(ostream& os, const Segment& s) {
        return os << "[" << s.a << ", " << s.b << "]";
    }
};
