#include "utils.h"

// #define min(x, y) (x) > (y) ? (y) : (x)

CUDA_HEAD float Q_rsqrt( float number )
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

    // Appends together as column vectors
CUDA_HEAD Mat3x3::Mat3x3(Vec3 c0, Vec3 c1, Vec3 c2) {
    for (int i = 0; i < 3; ++i) {
        arr[i][0] = c0[i];
        arr[i][1] = c1[i];
        arr[i][2] = c2[i];
    }
}

CUDA_HEAD float Mat3x3::det() const {
    return arr[0][0] * (arr[1][1] * arr[2][2] - arr[2][1] * arr[1][2])
            - arr[0][1] * (arr[1][0] * arr[2][2] - arr[2][0] * arr[1][2])
            + arr[0][2] * (arr[1][0] * arr[2][1] - arr[1][1] * arr[2][0]); 
}

Vec3 Vec3::cross(Vec3 other) const {
    return Vec3(y * other.z - z * other.y,
                -(x * other.z - z * other.x),
                x * other.y - y * other.x);
}

CUDA_HEAD Vec3 operator*(float left, Vec3 right) {
    return right * left;
}

CUDA_HEAD Vec3 operator+(float left, Vec3 right) {
    return right + left;
}

CUDA_HEAD Quat::Quat(Vec3 vec, float angle) {
    const float halfan = 0.5 * angle;
    float cos_a = cos(halfan);
    float sin_a = sin(halfan);

    w = cos_a;
    Vec3 ijk = vec * sin_a;
    x = ijk.x, y = ijk.y, z = ijk.z;
}

CUDA_HEAD Quat::Quat(Vec3 vec) {
    w = 0.0, x = vec.x, y = vec.y, z = vec.z;
}

// Computes the Hamiltonian 
// product of two Quats
CUDA_HEAD Quat Quat::operator*(Quat o) const {
    Quat quat = Quat();
    quat.w = w * o.w - x * o.x - y * o.y - z * o.z;
    quat.x = w * o.x + x * o.w + y * o.z - z * o.y;
    quat.y = w * o.y - x * o.z + y * o.w + z * o.x;
    quat.z = w * o.z + x * o.y - y * o.x + z * o.w;
    return quat;
}


CUDA_HEAD Vec3 Quat::RotateVector(Vec3 vec) const {
    Quat prime = Quat(w, -x, -y, -z);
    Quat v = Quat(vec);
    Quat result = *this * v * prime;

    // Rotation * quat-Vector * Rot.inv
    return Vec3(result.x, result.y, result.z);
}
