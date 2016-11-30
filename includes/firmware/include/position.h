#ifndef POSITION_H
#define POSITION_H

#include <cmath>
#include <fast_math.h>

struct Position{
    float x;
    float y;
    float a;

    Position():x(0.f),y(0.f),a(0.f){}
    Position(float _x,float _y,float _a):x(_x),y(_y),a(_a){}

    void normalize(){
        float fac=1.f/sqrt(x*x+y*y);
        x*=fac;
        y*=fac;
    }

    inline float norm() {
        return sqrt(x * x + y * y);
    }

    inline float norm_sqr() {
        return x * x + y * y;
    }

    Position mirrored() const {
        return Position(-x, -y, normalizeRotation(a + M_PI));
    }

    void mirror() {
        x = -x;
        y = -y;
        a = normalizeRotation(a + M_PI);
    }

    bool isAnyNan() const {
        return std::isnan(x) || std::isnan(y) || std::isnan(a);
    }

    float weightedNorm(float alphaWeight) const {
        return sqrt(x * x + y * y + alphaWeight * a * a);
    }

    Position& operator+=(const Position& rhs) {
        x += rhs.x;
        y += rhs.y;
        a = normalizeRotation(a + rhs.a);
        return *this;
    }

    Position& operator-=(const Position& rhs) {
        x -= rhs.x;
        y -= rhs.y;
        a = normalizeRotation(a - rhs.a);
        return *this;
    }

    Position& operator*=(float rhs) {
        x *= rhs;
        y *= rhs;
        a = normalizeRotation(a * rhs);
        return *this;
    }

    friend Position operator+(Position lhs, const Position& rhs){
        lhs += rhs;
        return lhs;
    }

    friend Position operator-(Position lhs, const Position& rhs){
        lhs -= rhs;
        return lhs;
    }

    friend Position operator*(Position lhs, float rhs){
        lhs *= rhs;
        return lhs;
    }

    friend Position operator*(float lhs, Position rhs){
        rhs *= lhs;
        return rhs;
    }

    bool operator==(const Position& rhs) const {
        return x==rhs.x && y==rhs.y && a==rhs.a;
    }

};

#endif // POSITION_H
