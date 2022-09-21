#pragma once

#include <vector>
#include <cmath>

#define min(x, y) (x) > (y) ? (y) : (x)


template<class T>
T lerp(T a, T b, float alpha) {
    return a + (b - a) * alpha;
}

float Q_rsqrt( float number )
{
	long i;
	float x2, y;
	const float threehalfs = 1.5F;

	x2 = number * 0.5F;
	y  = number;
	i  = * ( long * ) &y;                       // evil floating point bit level hacking
	i  = 0x5f3759df - ( i >> 1 );               // what the fuck? 
	y  = * ( float * ) &i;
	y  = y * ( threehalfs - ( x2 * y * y ) );   // 1st iteration
//	y  = y * ( threehalfs - ( x2 * y * y ) );   // 2nd iteration, this can be removed

	return y;
}

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

    // float& operator[](int index) {
    //     switch (index) {
    //         case 0: return x;
    //         case 1: return y;
    //         case 2: return z;
    //     }
    // } 

    float& operator[](int index) {
        return *((float*) this + index);
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
        return Vec3(-1.0, 0.0, 0.0);
    }
    inline static Vec3 right() {
        return Vec3(0.0, 1.0, 0.0);
    }
    inline static Vec3 forward() {
        return Vec3(0.0, 0.0, 1.0);
    }
    
    Vec3 cross(Vec3 other) const {
        return Vec3(y * other.z - z * other.y,
                  -(x * other.z - z * other.x),
                    x * other.y - y * other.x);
    }
    
};

struct Mat3x3 {

    float arr[3][3];

    Mat3x3() {}

    // Appends together as column vectors
    Mat3x3(Vec3 c0, Vec3 c1, Vec3 c2) {
        for (int i = 0; i < 3; ++i) {
            arr[i][0] = c0[i];
            arr[i][1] = c1[i];
            arr[i][2] = c2[i];
        }
    }

    float det() const {
        return arr[0][0] * (arr[1][1] * arr[2][2] - arr[2][1] * arr[1][2])
             - arr[0][1] * (arr[1][0] * arr[2][2] - arr[2][0] * arr[1][2])
             + arr[0][2] * (arr[1][0] * arr[2][1] - arr[1][1] * arr[2][0]); 
    }

};


Vec3 operator*(float left, Vec3 right) {
    return right * left;
}

Vec3 operator+(float left, Vec3 right) {
    return right + left;
}

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

    Quat(Vec3 vec, float angle) {
        const float halfan = 0.5 * angle;
        float cos_a = cos(halfan);
        float sin_a = sin(halfan);

        w = cos_a;
        Vec3 ijk = vec * sin_a;
        x = ijk.x, y = ijk.y, z = ijk.z;
    }

    Quat(Vec3 vec) {
        w = 0.0, x = vec.x, y = vec.y, z = vec.z;
    }

    // Computes the Hamiltonian 
    // product of two Quats
    Quat operator*(Quat o) const {
        Quat quat = Quat();
        quat.w = w * o.w - x * o.x - y * o.y - z * o.z;
        quat.x = w * o.x + x * o.w + y * o.z - z * o.y;
        quat.y = w * o.y - x * o.z + y * o.w + z * o.x;
        quat.z = w * o.z + x * o.y - y * o.x + z * o.w;
        return quat;
    }

    Quat operator*(float o) const {
        return Quat(w * o, x * o, y * o, z * o);
    }

    Quat operator+(Quat other) const {
        return Quat(other.w + w, other.x + x, other.y + y, other.z + z);
    }

    Quat operator-(Quat other) const {
        return Quat(w - other.w, x - other.x, y - other.y, z - other.z);
    }

    Vec3 RotateVector(Vec3 vec) const {
        Quat prime = Quat(w, -x, -y, -z);
        Quat v = Quat(vec);
        Quat result = *this * v * prime;

        // Rotation * quat-Vector * Rot.inv
        return Vec3(result.x, result.y, result.z);
    }

};

struct RayIntersectInfo {

    RayIntersectInfo() {
        distance = INFINITY;
        intersectPoint = Vec3(0.0), planeNormal = Vec3(0.0);
    }

    float distance;
    Vec3 intersectPoint;
    Vec3 planeNormal;
};

typedef unsigned int uint32;

struct Triangle {
    uint32 v0;
    uint32 v1;
    uint32 v2;

    Triangle(int t0, int t1, int t2)
    : v0(t0), v1(t1), v2(t2)
    {}
};