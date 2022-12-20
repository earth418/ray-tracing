#pragma once

#include <vector>
#include <iostream>
#include <cmath>
#include <cuda.h>

#define CUDA_HEAD __device__ __host__
#define max(a, b) ((a) > (b)) ? (a) : (b)

template<class T>
CUDA_HEAD T lerp(T a, T b, float alpha) {
    return a + (b - a) * alpha;
}

CUDA_HEAD float Q_rsqrt( float number );


struct Vec3 {

public:

    float x;
    float y;
    float z;

    CUDA_HEAD Vec3(float i = 0.0f) 
        : x(i), y(i), z(i)
    {}

    CUDA_HEAD Vec3(float ix, float iy, float iz) 
        : x(ix), y(iy), z(iz)
    {}

    CUDA_HEAD float& operator[](int index) {
        switch (index) {
            case 0: return x;
            case 1: return y;
            case 2: return z;
        }
        // std::cout << "Seriously? Three-D vector. Index is either 0, 1, 2. Come on.\n";
        return x;
    }

    CUDA_HEAD Vec3 operator+(Vec3 other) const {
        return Vec3(other.x + x, other.y + y, other.z + z);
    }

    CUDA_HEAD Vec3 operator+(float other) const {
        return Vec3(other + x, other + y, other + z);
    }

    CUDA_HEAD Vec3 operator-(Vec3 other) const {
        return Vec3(x - other.x, y - other.y, z - other.z);
    }

    CUDA_HEAD Vec3 operator*(Vec3 other) const {
        return Vec3(other.x * x, other.y * y, other.z * z);
    }

    CUDA_HEAD Vec3 operator*(float other) const {
        return Vec3(other * x, other * y, other * z);
    }

    CUDA_HEAD Vec3 operator/(Vec3 other) const {
        return Vec3(x / other.x, y / other.y, z / other.z);
    }

    CUDA_HEAD Vec3 operator/(float other) const {
        return Vec3(x / other, y / other, z / other);
    }

    CUDA_HEAD float dot(Vec3 other) const {
        return x * other.x + y * other.y + z * other.z;
    }

    CUDA_HEAD float sqrLength() const {
        return dot(*this);
    }

    CUDA_HEAD Vec3 normalize() const {
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
    
    CUDA_HEAD Vec3 cross(Vec3 other) const;
    
};

CUDA_HEAD Vec3 operator+(float left, Vec3 right);

CUDA_HEAD Vec3 operator*(float left, Vec3 right);

struct Mat3x3 {

    float arr[3][3];

    CUDA_HEAD Mat3x3() {}

    // Appends together as column vectors
    CUDA_HEAD Mat3x3(Vec3 c0, Vec3 c1, Vec3 c2);

    CUDA_HEAD float det() const;
};


struct Quat {

    float w;
    float x;
    float y;
    float z;

    CUDA_HEAD Quat(float i = 0.0f) 
        : x(i), y(i), z(i), w(i)
    {}

    CUDA_HEAD Quat(float iw, float ix, float iy, float iz) 
        : x(ix), y(iy), z(iz), w(iw)
    {}

    CUDA_HEAD Quat(Vec3 vec, float angle);

    CUDA_HEAD Quat(Vec3 vec);

    CUDA_HEAD Vec3 toVec() {
        return Vec3(x, y, z).normalize();
    }

    // Computes the Hamiltonian 
    // product of two Quats
    CUDA_HEAD Quat operator*(Quat o) const;

    CUDA_HEAD Quat operator*(float o) const {
        return Quat(w * o, x * o, y * o, z * o);
    }

    Quat operator+(Quat other) const {
        return Quat(other.w + w, other.x + x, other.y + y, other.z + z);
    }

    Quat operator-(Quat other) const {
        return Quat(w - other.w, x - other.x, y - other.y, z - other.z);
    }

    CUDA_HEAD Vec3 RotateVector(Vec3 vec) const;

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