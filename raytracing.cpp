#include <iostream>
#include <fstream>
// #include <string>

#include "utils.cpp"
#include "scene.cpp"

// bool RaySphereIntersect(Sphere sphere, Vec3 rayOrigin, Vec3 rayDirection, float* t0, float* t1) {

//     Vec3 vc = rayOrigin - sphere.position;
//     float b = 2.0 * rayDirection.dot(vc), 
//     c = vc.dot(vc) - sphere.radius * sphere.radius;

//     float disc = b * b - 4 * c;

//     if (disc < 0) {
//         *t0 = INFINITY;
//         *t1 = INFINITY;
//         return false;
//     }
    
//     *t0 = (-b - sqrt(disc)) * 0.5;
//     *t1 = (-b + sqrt(disc)) * 0.5;
//     return true;
// }

Vec3 RayTrace(World* world, Vec3 rayOrigin, Vec3 rayDireciton) {

    SceneObject* closest_object{};
    RayIntersectInfo closestIntersect{};

    for (SceneObject* object : world->objects) {

        RayIntersectInfo rayInfo{};

        if (object->RayObjectIntersect(rayOrigin, rayDireciton, rayInfo)) {

            if (rayInfo.distance < closestIntersect.distance) {
                closestIntersect = rayInfo;
                closest_object = object;
            }
        }

    }

    for (SceneObject* object : world->objects) {

        RayIntersectInfo rayInfo{};

        if (closestIntersect.distance != INFINITY && 
            object != closest_object &&     // IF the object DID intersect, but NOT with the current object 
            object->RayObjectIntersect(closestIntersect.intersectPoint, -1.0 * world->lightDirection, rayInfo, 0.0001)) {

            return Vec3(0.0);
        }

    }
    if (closestIntersect.distance == INFINITY)
        return Vec3(0.5);

    

    // Vec3 P = rayOrigin + closest_dist * rayDireciton;
    // Vec3 SphereNormal = closest_object.NormalAtPoint(P); // (P - closest_object.position) / closest_object.radius;
    float alpha = (1.0 - world->lightDirection.dot(closestIntersect.planeNormal)) * 0.5;
    return alpha * closest_object->color;
}

int main(int argc, char** argv) {

    const int IMG_HEIGHT = 800;
    const int IMG_WIDTH = 800;

    World* w = new World();
    // Sphere* s = new Sphere(Vec3(-1.5, 0.0, 5.0), 1.0);
    // s->color = Vec3(0.0, 1.0, 0.0);
    // w->objects.push_back(s);

    // s = new Sphere(Vec3(-0.5, 0.0, 6.5), 0.5);
    // s->color = Vec3(0.0, 0.0, 1.0);
    // w->objects.push_back(s);

    Sphere* gnd = new Sphere(Vec3(-200.0, 0.0, 5.0), 200.0);
    gnd->color = Vec3(1.0, 0.0, 1.0).normalize();
    w->objects.push_back(gnd);

    // Where the light is coming from
    w->lightDirection = Vec3(1.0, -0.5, 0.0).normalize();

    // SceneObject lightSource = SceneObject();

    TriMesh* t = new TriMesh();
    t->verts.push_back(Vec3(-0.5, 0.0, 5.0));
    t->verts.push_back(Vec3(0.5, 0.5, 5.0));
    t->verts.push_back(Vec3(0.5, -0.5, 5.0));
    
    t->color = Vec3(1.0, 1.0, 0.0);

    t->tris.push_back(Triangle(0, 1, 2));

    w->objects.push_back(t);


    // const Vec3 viewport_lower_left = Vec3(-0.5, -0.5, 1.0);
    // const Vec3 viewport_upper_right = Vec3(0.5, 0.5, 1.0);
    char filename[23];
    const float focal_length = 0.75;

    int N_FRAMES;
    if (argc == 2)
        N_FRAMES = atoi(argv[1]);
    else
        N_FRAMES = 1;

    const Quat start_rot = Quat(Vec3::up(), -30.0 * 3.14159 / 180.0);
    const Quat end_rot = Quat(Vec3::up(), -150.0 * 3.14159 / 180.0); // * Quat(Vec3::forward(), 3.14159);
    
    Quat lilRot = Quat(Vec3::up(), 8.0 * (3.14159 / (float) N_FRAMES));

    const Vec3 start_loc = Vec3(-1.0, 3.0, 1.0);
    // const Vec3 middle_loc = Vec3(-4.0, 0.0, 5.5);
    const Vec3 end_loc = Vec3(-1.0, 3.0, 11.0);

    for (int frame = 0; frame < N_FRAMES; ++frame) {

        float frameAlpha = float(frame) / float(N_FRAMES - 1);

        // Create the image file for this frame
        sprintf(filename, "./images/image%04d.ppm", frame);
        std::ofstream out(filename); // std::format("./images/image{frame:4d}.ppm"));
        std::streambuf *coutbuf = std::cout.rdbuf(); //save old buf
        std::cout.rdbuf(out.rdbuf()); //redirect std::cout to out.txt!

        Vec3 cameraPos = Vec3(-5.0, 0.0, 0.0); // lerp(start_loc, end_loc, frameAlpha);
        Quat rot = Quat(Vec3::up(), 0.0); // lerp(start_rot, end_rot, frameAlpha);

        // w->objects[0]->scale = 1.03 * w->objects[0]->scale;
    
        w->lightDirection = lilRot.RotateVector(w->lightDirection);

        std::cout << "P3\n" << IMG_WIDTH << ' ' << IMG_HEIGHT << "\n255\n";

        for (int i = 0; i < IMG_WIDTH; ++i) {
            for (int j = IMG_HEIGHT - 1; j >= 0; --j) {

                float x = -float(i), y = -float(IMG_HEIGHT - j - 1);

                float alphax = x / (float) (IMG_WIDTH - 1);
                float alphay = y / (float) (IMG_HEIGHT - 1);

                Vec3 rel_viewpoint_pos = Vec3(-0.5 + alphax, -0.5 + alphay, focal_length);
                rel_viewpoint_pos = rot.RotateVector(rel_viewpoint_pos);

                Vec3 direction = rel_viewpoint_pos.normalize();
            

                Vec3 color = RayTrace(w, cameraPos, direction);

                int ir = static_cast<int>(255.999 * color.x);
                int ig = static_cast<int>(255.999 * color.y);
                int ib = static_cast<int>(255.999 * color.z);

                std::cout << ir << ' ' << ig << ' ' << ib << '\n';

            }
        }
    }

    return 0;
}