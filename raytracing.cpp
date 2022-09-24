#include <iostream>
#include <fstream>
// #include <string>

#include "utils.h"
#include "scene.h"

#define pi 3.14159265358979323

Vec3 RayTrace(World* world, Vec3 rayOrigin, Vec3 rayDireciton) {

    RayIntersectInfo closestIntersect{};

    // First run-through to determine what object it hits
    for (SceneObject* object : world->objects) {

        RayIntersectInfo rayInfo{};

        if (object->RayObjectIntersect(rayOrigin, rayDireciton, rayInfo)) {

            if (rayInfo.distance < closestIntersect.distance) {
                closestIntersect = rayInfo;
            }
        }

    }

    // No hit; return background color
    if (closestIntersect.distance == INFINITY)
        return Vec3(0.3, 0.4, 0.7);


    // Second run-through for shadows (and maybe reflections too idk)
    for (SceneObject* object : world->objects) {

        RayIntersectInfo rayInfo{};

        if (object != closestIntersect.closestObj &&     // IF the object DID intersect, but NOT with the current object 
            object->RayObjectIntersect(
                closestIntersect.intersectPoint, -1.0 * world->lightDirection, rayInfo, 0.0001)) {

            return Vec3(0.0);
        }

    }
    
    // Vec3 P = rayOrigin + closest_dist * rayDireciton;
    // Vec3 SphereNormal = closest_object.NormalAtPoint(P); // (P - closest_object.position) / closest_object.radius;
    float alpha = (1.0 - world->lightDirection.dot(closestIntersect.planeNormal)) * 0.5;
    return alpha * closestIntersect.pointColor;
}

int main(int argc, char** argv) {

    const int IMG_HEIGHT = 500;
    const int IMG_WIDTH = 500;

    World* w = new World();
    // Sphere* s = new Sphere(Vec3(5.0, 0.0, 2.0), 2.0);
    // s->color = Vec3(0.0, 1.0, 0.0);
    // w->objects.push_back(s);

    // s = new Sphere(Vec3(-0.5, 0.0, 6.5), 0.5);
    // s->color = Vec3(0.0, 0.0, 1.0);
    // w->objects.push_back(s);

    Sphere* gnd = new Sphere(Vec3(0.0, 0.0, -500.0), 500.0);
    gnd->color = Vec3(0.5, 0.0, 0.5).normalize();
    w->objects.push_back(gnd);

    // Where the light is coming from
    w->lightDirection = Vec3(0.0, 0.0, -1.0).normalize();

    // SceneObject lightSource = SceneObject();

    // TriMesh* t = new TriMesh("../cube.obj");
    TriMesh* t = new TriMesh();
    
    t->position = Vec3(0.0, -1.5, 0.0);
    t->scale = Vec3(0.1, 1.0, 5.0);

    t->verts.push_back(Vec3(0.0, 0.0, 0.0));
    t->verts.push_back(Vec3(0.0, 3.0, 0.0));
    t->verts.push_back(Vec3(0.0, 0.0, 3.0));
    t->verts.push_back(Vec3(0.0, 3.0, 3.0));

    t->color = Vec3(1.0, 1.0, 0.0);

    t->tris.push_back(TriangleInfo(2, 1, 0));
    t->tris.push_back(TriangleInfo(2, 3, 1));

    w->objects.push_back(t);

    // for (Vec3 v : t->verts) {
    //     w->objects.push_back(new Sphere(v + t->position, 0.25));
    // }

    char filename[23];
    const float focal_length = 0.5;

    int N_FRAMES;
    if (argc == 2)
        N_FRAMES = atoi(argv[1]);
    else
        N_FRAMES = 1;

    const Quat start_rot = Quat(Vec3::up(), 0.0 * pi / 180.0);
    const Quat end_rot = Quat(Vec3::up(), 180.0 * pi / 180.0); // * Quat(Vec3::forward(), 3.14159);
    
    Quat lilRot = Quat(Vec3::up(), 8.0 * (pi / (float) N_FRAMES));

    // const Vec3 start_loc = Vec3(-1.0, 3.0, 1.0);
    // const Vec3 middle_loc = Vec3(-4.0, 0.0, 5.5);
    // const Vec3 end_loc = Vec3(-1.0, 3.0, 11.0);

    for (int frame = 0; frame < N_FRAMES; ++frame) {

        float frameAlpha = 0.0;

        if (N_FRAMES > 1)
            frameAlpha = float(frame) / float(N_FRAMES - 1);

        // Create the image file for this frame
        sprintf(filename, "./images/image%04d.ppm", frame);

        std::ofstream imgout(filename); // std::format("./images/image{frame:4d}.ppm"));

        // std::streambuf *coutbuf = std::cout.rdbuf(); //save old buf
        // std::cout.rdbuf(out.rdbuf()); //redirect std::cout to out.txt!

        // Quat rot = Quat(Vec3::up(), 0.0); // lerp(start_rot, end_rot, frameAlpha); 

        Quat rot = Quat(Vec3::up(), pi * 2.0 * frameAlpha); 
        Vec3 cameraPos = rot.RotateVector(Vec3(-5.0, 0.0, 1.0));

        // w->objects[0]->scale = 1.03 * w->objects[0]->scale;
    
        // w->lightDirection = lilRot.RotateVector(w->lightDirection);

        imgout << "P3\n" << IMG_WIDTH << ' ' << IMG_HEIGHT << "\n255\n";

        for (int i = 0; i < IMG_WIDTH; ++i) {
            for (int j = IMG_HEIGHT - 1; j >= 0; --j) {

                float y = float(i), z = float(IMG_HEIGHT - j - 1);

                float alphay = y / (float) (IMG_WIDTH - 1);
                float alphaz = z / (float) (IMG_HEIGHT - 1);

                Vec3 rel_viewpoint_pos = Vec3(focal_length, -0.5 + alphaz, 0.5 - alphay);
                rel_viewpoint_pos = rot.RotateVector(rel_viewpoint_pos);

                Vec3 direction = rel_viewpoint_pos.normalize();
            
                Vec3 color = RayTrace(w, cameraPos, direction);

                int ir = static_cast<int>(255.999 * color.x);
                int ig = static_cast<int>(255.999 * color.y);
                int ib = static_cast<int>(255.999 * color.z);

                imgout << ir << ' ' << ig << ' ' << ib << '\n';

            }
        }

        imgout.close();
    }

    for (SceneObject* s : w->objects)
        delete s;

    return 0;
}