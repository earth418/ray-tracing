#pragma once

#include "utils.cpp"
#include <vector>

class SceneObject {

public:

    SceneObject()
    : position(Vec3()), scale(Vec3())
    {}

    SceneObject(Vec3 _pos, Vec3 _scale)
    : position(_pos), scale(_scale)
    {

    }

    virtual bool RayObjectIntersect(Vec3 rayOrigin, Vec3 rayDirection, RayIntersectInfo& info,
     float minDist = 1.0f, float maxDist = INFINITY) = 0;

    // virtual Vec3 NormalAtPoint(Vec3 P);
public:
    Vec3 position;
    Vec3 scale;
    Quat rotation;

    Vec3 color;
};

class TriMesh : public SceneObject {

public:

    TriMesh() {}

    // Must be a .OBJ file
    TriMesh(const char* filename) {
        ;
    }

    std::vector<Triangle> tris;
    std::vector<Vec3> verts;
    std::vector<Vec3> normals;

    bool RayObjectIntersect(Vec3 rayOrigin, Vec3 rayDirection, RayIntersectInfo& info, 
    float minDist = 1.0f, float maxDist = INFINITY) override 
    {

        for (auto tri : tris) {

            const Vec3 v0 = verts[tri.v0];
            const Vec3 v1 = verts[tri.v1];
            const Vec3 v2 = verts[tri.v2];
            
            const Vec3 v0v1 = v0 - v1;
            const Vec3 v0v2 = v0 - v2;

            const Vec3 N = v0v1.cross(v0v2).normalize();

            float t = (v0 - rayOrigin).dot(N) / rayDirection.dot(N);

            if (t >= maxDist || t < minDist) {
                info = RayIntersectInfo();
                return false;
            }

            const Vec3 pt = rayOrigin + rayDirection * t;

            const Mat3x3 A = Mat3x3(v0v1, v0v2, rayDirection);
            const float IdetA = 1.0 / A.det();

            const Vec3 b = v0 - pt;

            const float detA0 = Mat3x3(b, v0v2, rayDirection).det();
            const float detA1 = Mat3x3(v0v1, b, rayDirection).det();
            const float detA2 = Mat3x3(v0v1, v0v2, b).det();

            const float beta = IdetA * detA0;
            const float gamma = IdetA * detA1;
            const float tau = IdetA * detA2;

            if (beta >= -0.001 && gamma >= -0.001 && tau >= -0.001)  {
                
                info = RayIntersectInfo();
                info.distance = t;
                info.intersectPoint = pt;
                info.planeNormal = N;
                
                return true;
            }

        }

        
        info = RayIntersectInfo();
        return false;

    }

};

class Sphere : public SceneObject {

public:

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

    bool RayObjectIntersect(Vec3 rayOrigin, Vec3 rayDirection, RayIntersectInfo& info, 
    float minDist = 1.0f, float maxDist = INFINITY) override {
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

        if (r0 < minDist) {
            if (r1 < minDist) {
                info = RayIntersectInfo();
                return false;
            }
            
            r0 = maxDist; // Force other collision point
        }

        if (r1 < 1.0)
            r1 = maxDist; // Force other collision point


        if (r1 >= maxDist && r0 >= maxDist) {
            info = RayIntersectInfo();
            return false;
        }


        info = RayIntersectInfo();
        info.distance = min(r0, r1);
        info.intersectPoint = rayOrigin + info.distance * rayDirection;
        info.planeNormal = (info.intersectPoint - position) / radius;
        return true;
    }

};

class LightSource : public Sphere {
    
public:
    LightSource(float iintensity) 
    : intensity(iintensity)
    {}

    float intensity;

};

class World {
public:
    std::vector<SceneObject*> objects;

    Vec3 lightDirection;

};