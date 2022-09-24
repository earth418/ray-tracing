
#include "scene.h"


RayIntersectInfo::RayIntersectInfo() {
    distance = INFINITY;
    intersectPoint = Vec3(0.0), planeNormal = Vec3(0.0), pointColor = Vec3(0.0, 0.0, 0.0);
}

// Must be a .OBJ file
TriMesh::TriMesh(const char* filename) {
    std::FILE* objfile = fopen(filename, "r");
    
    if (!objfile) {
        std::cerr << "Failed, lol." << std::endl;
        return;
    }

    char linehead[64];
    while (true) {

        if (fscanf(objfile, "%s", linehead) == EOF) {
            fclose(objfile);
            return;
        }

        if (strcmp("v", linehead) == 0) {
            Vec3 vert;
            fscanf(objfile, "%f %f %f\n", &vert.x, &vert.y, &vert.z);
            verts.push_back(vert);
        }
        else if (strcmp("vt", linehead) == 0) {
            Vec3 uv;
            fscanf(objfile, "%f %f\n", &uv.x, &uv.y);
            uvs.push_back(uv);
        }
        else if (strcmp("vn", linehead) == 0) {
            Vec3 normal;
            fscanf(objfile, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
            normals.push_back(normal);
        }
        else if (strcmp("f", linehead) == 0) {
            uint32 vertexIndex[3], uvIndex[3], normalIndex[3];
            int matches = fscanf(objfile, "%d/%d/%d %d/%d/%d %d/%d/%d\n", 
                &vertexIndex[0], &uvIndex[0], &normalIndex[0], 
                &vertexIndex[1], &uvIndex[1], &normalIndex[1],
                &vertexIndex[2], &uvIndex[2], &normalIndex[2]);

            if (matches != 9) {
                fclose(objfile);
                std::cerr << "File can't be read by our simple parser :( Try exporting with other options\n";
                return;
            }

            TriangleInfo t{};
            t.v0 = vertexIndex[0];
            t.v1 = vertexIndex[1];
            t.v2 = vertexIndex[2];

            t.uv0 = uvIndex[0];
            t.uv1 = uvIndex[1];
            t.uv2 = uvIndex[2];

            t.n0 = normalIndex[0];
            t.n1 = normalIndex[1];
            t.n2 = normalIndex[2];

            tris.push_back(t);
        }
    }
}

    
bool TriMesh::RayObjectIntersect(Vec3 rayOrigin, Vec3 rayDirection, RayIntersectInfo& info, 
float minDist, float maxDist)  
{
    // float minTriDist = INFINITY;
    info = RayIntersectInfo();
    info.distance = INFINITY;

    rayDirection = rayDirection / scale;
    // rayDirection = rotation.RotateVector(rayDirection);
    rayOrigin = rayOrigin - position;

    for (int i = 0; i < tris.size(); ++i) {
        const TriangleInfo tri = tris[i];

        const Vec3 v0 = verts[tri.v0];
        const Vec3 v1 = verts[tri.v1];
        const Vec3 v2 = verts[tri.v2];
        
        const Vec3 v0v1 = v0 - v1;
        const Vec3 v0v2 = v0 - v2;

        Vec3 N;

        if (normals.size() == 0)
            N = v0v1.cross(v0v2).normalize();
        else
            N = (normals[tri.n0] + normals[tri.n1] + normals[tri.n2]) / 3.0;

        // if (N.dot(rayDirection) > 0.0) {
        //     info = RayIntersectInfo();
        //     return false;
        // }

        const Mat3x3 A = Mat3x3(v0v1, v0v2, rayDirection);
        const float detA = A.det();
        if (A.det() == 0.0)
            continue;

        const float IdetA = 1.0 / A.det();
        
        const Vec3 b = v0 - rayOrigin;

        const float detA0 = Mat3x3(b, v0v2, rayDirection).det();
        const float detA1 = Mat3x3(v0v1, b, rayDirection).det();
        const float detA2 = Mat3x3(v0v1, v0v2, b).det();

        const float beta = IdetA * detA0;
        const float gamma = IdetA * detA1;
        const float t = IdetA * detA2;

        if (t >= maxDist || t < minDist) {
            info = RayIntersectInfo();
            return false;
        }
        // const float alpha = 1 - beta - gamma;

        if (beta > 0.0 && gamma > 0.0 && beta + gamma < 1.0 && t < info.distance)  {
            
            // minTriDist = t;

            // info = RayIntersectInfo();
            info.distance = t;
            info.intersectPoint = rayOrigin + t * rayDirection;
            info.planeNormal = N;
            info.closestObj = this;
            info.pointColor = this->color; // Vec3(float(i) / float(tris.size())); // this->color;
        }

    }

    if (info.distance == INFINITY) {
        info = RayIntersectInfo();
        return false;
    }
    else {

        return true;
    }

}

bool Sphere::RayObjectIntersect(Vec3 rayOrigin, Vec3 rayDirection, RayIntersectInfo& info, 
float minDist, float maxDist) {
    Vec3 vc = rayOrigin - position;
    float b = 2.0 * rayDirection.dot(vc), 
    c = vc.dot(vc) - radius * radius;

    float disc = b * b - 4 * c;

    // std::cout << disc << '\n';

    if (disc < 0) {
        info = RayIntersectInfo();
        return false;
    }
    
    float r0 = (-b - sqrt(disc)) * 0.5;
    float r1 = (-b + sqrt(disc)) * 0.5;

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
    info.distance = (r0 < r1) ? r0 : r1;
    info.intersectPoint = rayOrigin + info.distance * rayDirection;
    info.planeNormal = (info.intersectPoint - position) / radius;
    info.closestObj = this;
    info.pointColor = this->color;
    return true;
}