#pragma once

#include <vector>
#include <cmath>

#define max(a, b) ((a) > (b)) ? (a) : (b)

template<class T>
T lerp(T a, T b, float alpha) {
    return a + (b - a) * alpha;
}

float Q_rsqrt( float number );


struct Vec3 {

public:

    float x;
    float y;
    float z;

    Vec3(float i = 0.0f) 
        : x(i), y(i), z(i)
    {}

    Vec3(float ix, float iy, float iz) 
        : x(ix), y(iy), z(iz)
    {}

    float& operator[](int index) {
        switch (index) {
            case 0: return x;
            case 1: return y;
            case 2: return z;
        }
    } 

    Vec3 operator+(Vec3 other) const {
        return Vec3(other.x + x, other.y + y, other.z + z);
    }

    Vec3 operator+(float other) const {
        return Vec3(other + x, other + y, other + z);
    }

    Vec3 operator-(Vec3 other) const {
        return Vec3(x - other.x, y - other.y, z - other.z);
    }

    Vec3 operator*(Vec3 other) const {
        return Vec3(other.x * x, other.y * y, other.z * z);
    }

    Vec3 operator*(float other) const {
        return Vec3(other * x, other * y, other * z);
    }

    Vec3 operator/(Vec3 other) const {
        return Vec3(x / other.x, y / other.y, z / other.z);
    }

    Vec3 operator/(float other) const {
        return Vec3(x / other, y / other, z / other);
    }

    float dot(Vec3 other) const {
        return x * other.x + y * other.y + z * other.z;
    }

    float sqrLength() const {
        return dot(*this);
    }

    Vec3 normalize() const {
        return *this * Q_rsqrt(sqrLength());
    }

    inline static Vec3 up() {
        return Vec3(0.0, 0.0, 1.0);
    }
    inline static Vec3 right() {
        return Vec3(0.0, 1.0, 0.0);
    }
    inline static Vec3 forward() {
        return Vec3(1.0, 0.0, 0.0);
    }
    
    Vec3 cross(Vec3 other) const;
    
};

Vec3 operator+(float left, Vec3 right);

Vec3 operator*(float left, Vec3 right);

struct Mat3x3 {

    float arr[3][3];

    Mat3x3() {}

    // Appends together as column vectors
    Mat3x3(Vec3 c0, Vec3 c1, Vec3 c2);

    float det() const;
};


struct Quat {

    float w;
    float x;
    float y;
    float z;

    Quat(float i = 0.0f) 
        : x(i), y(i), z(i), w(i)
    {}

    Quat(float iw, float ix, float iy, float iz) 
        : x(ix), y(iy), z(iz), w(iw)
    {}

    Quat(Vec3 vec, float angle);

    Quat(Vec3 vec);

    Vec3 toVec() {
        return Vec3(x, y, z).normalize();
    }

    // Computes the Hamiltonian 
    // product of two Quats
    Quat operator*(Quat o) const;

    Quat operator*(float o) const {
        return Quat(w * o, x * o, y * o, z * o);
    }

    Quat operator+(Quat other) const {
        return Quat(other.w + w, other.x + x, other.y + y, other.z + z);
    }

    Quat operator-(Quat other) const {
        return Quat(w - other.w, x - other.x, y - other.y, z - other.z);
    }

    Vec3 RotateVector(Vec3 vec) const;

};

typedef unsigned int uint32;

struct TriangleInfo {
    uint32 v0, v1, v2;
    uint32 n0, n1, n2;
    uint32 uv0, uv1, uv2;

    TriangleInfo()
    {}

    TriangleInfo(int t0, int t1, int t2)
    : v0(t0), v1(t1), v2(t2)
    {}
};