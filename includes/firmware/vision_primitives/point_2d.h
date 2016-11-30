#ifndef POINT_2D_H
#define POINT_2D_H

#include <cfloat>
#include <cmath>
#include <tuple>

namespace htwk {

struct point_2d {
    float x;
    float y;

    point_2d() = default;
    point_2d(const point_2d&) = default;
    constexpr point_2d(float x, float y) : x(x), y(y) {}

    point_2d& operator+=(const point_2d& rhs) {
        x += rhs.x;
        y += rhs.y;
        return *this;
    }

    friend point_2d operator+(point_2d lhs, const point_2d& rhs) {
        lhs += rhs;
        return lhs;
    }

    point_2d& operator-=(const point_2d& rhs) {
        x -= rhs.x;
        y -= rhs.y;
        return *this;
    }

    friend point_2d operator-(point_2d lhs, const point_2d& rhs) {
        lhs -= rhs;
        return lhs;
    }

    friend point_2d operator*(point_2d lhs, float rhs) {
        lhs.x *= rhs;
        lhs.y *= rhs;
        return lhs;
    }

    point_2d& operator/=(float rhs) {
        x /= rhs;
        y /= rhs;
        return *this;
    }

    friend point_2d operator/(point_2d lhs, float rhs) {
        lhs /= rhs;
        return lhs;
    }

    constexpr point_2d operator-() const {
        return point_2d(-x, -y);
    }

    inline float norm() const {
        return sqrt(x*x+y*y);
    }

    inline float norm_sqr() const {
        return x*x+y*y;
    }

    inline point_2d mul_elem(const point_2d& b) const {
        return {x * b.x, y * b.y};
    }

    inline float dot(const point_2d& b) const {
        return x * b.x + y * b.y;
    }

    inline std::tuple<float,float> tuple() const {
        return std::make_tuple(x, y);
    }
};

inline bool operator==(const point_2d& lhs, const point_2d& rhs) {
    return lhs.x == rhs.x && lhs.y == rhs.y;
}

}  // namespace htwk

#endif // POINT_2D_H

