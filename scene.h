#pragma once

#include "utils.h"

#include <vector>
#include <cstring>
#include <iostream>

class SceneObject;

struct RayIntersectInfo {

    RayIntersectInfo();

    float distance;
    SceneObject* closestObj;
    Vec3 intersectPoint;
    Vec3 pointColor;
    Vec3 planeNormal;
};

class SceneObject {

public:

    SceneObject()
    : position(Vec3()), scale(Vec3(1.0))
    {}

    SceneObject(Vec3 _pos, Vec3 _scale)
    : position(_pos), scale(_scale)
    {

    }

    virtual bool RayObjectIntersect(Vec3 rayOrigin, Vec3 rayDirection, RayIntersectInfo& info,
     float minDist = 1.0f, float maxDist = INFINITY) = 0;

public:
    Vec3 position;
    Vec3 scale;
    Quat rotation;

    Vec3 color;
};



class TriMesh : public SceneObject {

public:
    TriMesh() 
    : SceneObject()
    {}

    // Must be a .OBJ file
    TriMesh(const char* filename);

    std::vector<TriangleInfo> tris;
    std::vector<Vec3> verts;
    std::vector<Vec3> normals;
    std::vector<Vec3> uvs; // Replace with Vec2 once it exists; for now keep z at 0
    std::vector<Vec3> colors;

    bool RayObjectIntersect(Vec3 rayOrigin, Vec3 rayDirection, RayIntersectInfo& info, 
        float minDist = 1.0f, float maxDist = INFINITY) override;

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
        float minDist = 1.0f, float maxDist = INFINITY) override;
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