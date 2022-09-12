#include <vector>
#include <cmath>

#define min(x, y) (x) > (y) ? (y) : (x)

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

    Vec3 normalize() {
        return *this * Q_rsqrt(sqrLength());
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
    Quat operator*(Quat o) {
        Quat quat = Quat();
        quat.w = w * o.w - x * o.x - y * o.y - z * o.z;
        quat.x = w * o.x + x * o.w + y * o.z - z * o.y;
        quat.y = w * o.y - x * o.z + y * o.w + z * o.x;
        quat.z = w * o.z + x * o.y - y * o.x + z * o.w;
        return quat;
    }

    Quat RotateVector(Vec3 vec) {
        return operator*(Quat(vec));
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

struct SceneObject {

public:

    SceneObject()
    : position(Vec3()), scale(Vec3())
    {}

    SceneObject(Vec3 _pos, Vec3 _scale)
    : position(_pos), scale(_scale)
    {

    }

    virtual bool RayObjectIntersect(Vec3 rayOrigin, Vec3 rayDirection, RayIntersectInfo& info) = 0;

    // virtual Vec3 NormalAtPoint(Vec3 P);

    Vec3 position;
    Vec3 scale;
    Quat rotation;

    Vec3 color;

};

struct LightSource : SceneObject {
    
    float intensity;

};

struct Sphere : SceneObject {

    Sphere()
    : radius(0.0)
    {
    }

    Sphere(Vec3 _pos, float radius)
    : radius(radius)
    {
        this->position = _pos;
        this->scale = Vec3(1.0);
    }

    float radius;

    bool RayObjectIntersect(Vec3 rayOrigin, Vec3 rayDirection, RayIntersectInfo& info) override {
        Vec3 vc = rayOrigin - position;
        float b = 2.0 * rayDirection.dot(vc), 
        c = vc.dot(vc) - radius * radius;

        float disc = b * b - 4 * c;

        // std::cout << disc << '\n';

        if (disc < 0) {
            info = RayIntersectInfo();
            return false; // {INFINITY, Vec3(0.0), Vec3(0.0)};
        }
        
        float r0 = (-b - sqrt(disc)) * 0.5;
        float r1 = (-b + sqrt(disc)) * 0.5;

        
        // std::cout << r0 << ' ' << r1 << '\n';

        if (r0 < 1.0) {
            if (r1 < 1.0) {
                info = RayIntersectInfo();
                return false;
            }
            
            r0 = INFINITY; // Force other collision point
        }

        if (r1 < 1.0)
            r1 = INFINITY; // Force other collision point

        info = RayIntersectInfo();
        info.distance = min(r0, r1);
        info.intersectPoint = rayOrigin + info.distance * rayDirection;
        info.planeNormal = (info.intersectPoint - position) / radius;
        return true;
    }

};

struct World {

    std::vector<SceneObject*> objects;

    Vec3 lightDirection;

};