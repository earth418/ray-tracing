#pragma once

#include "utils.h"

#include <cuda.h>
#include <vector>
#include <cstring>
#include <iostream>

#define MAX_OBJS 16
#define MAX_LGHT 4

class SceneObject;

struct RayIntersectInfo {

    CUDA_HEAD RayIntersectInfo();

    float distance;
    SceneObject* hitObj;
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

    inline CUDA_HEAD virtual bool RayObjectIntersect(Vec3 rayOrigin, Vec3 rayDirection, RayIntersectInfo& info,
     float minDist = 1.0f, float maxDist = INFINITY) const = 0;

public:
    Vec3 position;
    Vec3 scale;
    Quat rotation;

    Vec3 color;

    bool castShadow = true;
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

    CUDA_HEAD bool RayObjectIntersect(Vec3 rayOrigin, Vec3 rayDirection, RayIntersectInfo& info, 
        float minDist = 1.0f, float maxDist = INFINITY) const override;

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

    CUDA_HEAD bool RayObjectIntersect(Vec3 rayOrigin, Vec3 rayDirection, RayIntersectInfo& info, 
        float minDist = 1.0f, float maxDist = INFINITY) const override;
};

class LightSource {
    
public:

    Vec3 position;
    Vec3 color;

    LightSource()
    : intensity(0.0)
    {
        color = Vec3(1.0, 1.0, 1.0);
    }

    LightSource(float iintensity) 
    : intensity(iintensity)
    {
    }

public:
    float intensity;

    CUDA_HEAD virtual Vec3 getDirection(Vec3 pointPosition) const = 0;

    CUDA_HEAD virtual float calculateIntensity(Vec3 pointNormal, Vec3 pointPosition) const = 0;
};

class PointLight : public LightSource {
    
    
    CUDA_HEAD Vec3 getDirection(Vec3 pointPosition) const override
    {
        return (pointPosition - position).normalize();
    }

    CUDA_HEAD float calculateIntensity(Vec3 pointNormal, Vec3 pointPosition) const override {
        Vec3 ld = pointPosition - position;
        return max(0.0, 0.0 - pointNormal.dot(ld.normalize())) * intensity; // / ld.sqrLength();
    }
};

class DirectionalLight : public LightSource {

public:
    Vec3 direction;

    DirectionalLight()
    : LightSource()
    {
    }

    DirectionalLight(Vec3 idirection, float iintensity) 
    : direction(idirection)
    {
        intensity = iintensity;
    }

    CUDA_HEAD Vec3 getDirection(Vec3 pointPosition) const override
    {
        return direction;
    }

    CUDA_HEAD float calculateIntensity(Vec3 pointNormal, Vec3 pointPosition) const override {
        return max(0.0, -pointNormal.dot(direction)) * intensity;
    }

};

class World {
public:
    // std::vector<SceneObject*> objects;
    // SceneObject** objects = new SceneObject*[MAX_OBJS];
    SceneObject* objects[MAX_OBJS];

    // std::vector<LightSource*> lights;
    LightSource* lights[MAX_LGHT];

    // Vec3 lightDirection;

};