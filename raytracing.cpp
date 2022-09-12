#include <iostream>
#include "utils.cpp"

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

    SceneObject closest_object{};
    RayIntersectInfo closestIntersect{INFINITY, Vec3(0.0), Vec3(0.0)};

    for (SceneObject object : world->objects) {

        RayIntersectInfo rayInfo = object.RayObjectIntersect(rayOrigin, rayDireciton);

        if (rayInfo.distance < INFINITY) {

            if (rayInfo.distance < closestIntersect.distance) {
                closestIntersect.distance = rayInfo.distance;
                closest_object = object;
            }
        }

    }
    if (closestIntersect.distance == INFINITY)
        return Vec3(0.5);

    // Vec3 P = rayOrigin + closest_dist * rayDireciton;
    // Vec3 SphereNormal = closest_object.NormalAtPoint(P); // (P - closest_object.position) / closest_object.radius;
    float alpha = (1.0 - world->lightDirection.dot(closestIntersect.planeNormal)) * 0.5;
    return closest_object.color;
}

int main(int argc, char** argv) {

    const int IMG_HEIGHT = 800;
    const int IMG_WIDTH = 800;

    World* w = new World();
    Sphere s = Sphere(Vec3(-1.0, 0.0, 6.0), 1.0);
    s.color = Vec3(1.0, 0.0, 0.0);
    w->objects.push_back(s);

    s = Sphere(Vec3(-0.5, 1.0, 4.0), 0.5);
    s.color = Vec3(0.0, 1.0, 0.0);
    w->objects.push_back(s);

    s = Sphere(Vec3(200.0, 0.0, 6.0), 200.0);
    s.color = Vec3(1.0, 0.0, 1.0).normalize();
    w->objects.push_back(s);

    w->lightDirection = Vec3(1.0, -1.0, 0.5).normalize();

    // SceneObject lightSource = SceneObject();

    SceneObject camera = SceneObject(Vec3(0.5, 0.0, 0.0), Vec3());

    const Vec3 lower_left = Vec3(-0.5, -0.5, 1.0);
    const Vec3 upper_right = Vec3(0.5, 0.5, 1.0);

    std::cout << "P3\n" << IMG_WIDTH << ' ' << IMG_HEIGHT << "\n255\n";

    for (int i = 0; i < IMG_WIDTH; ++i) {
        for (int j = IMG_HEIGHT - 1; j >= 0; --j) {

            float x = float(i), y = float(IMG_HEIGHT - j - 1);

            float alphax = x / (float) (IMG_WIDTH - 1);
            float alphay = y / (float) (IMG_HEIGHT - 1);

            Vec3 viewpoint_pos = lower_left + (upper_right - lower_left) * Vec3(alphax, alphay, 0);
            Vec3 direction = viewpoint_pos - camera.position;
            direction = direction.normalize();
            
            // std::cout << alphax << ' ' << alphay << std::endl; 
            // std::cout << direction.x << ' ' << direction.y << ' ' << direction.z << '\n';

            Vec3 color = RayTrace(w, camera.position, direction);

            int ir = static_cast<int>(255.999 * color.x);
            int ig = static_cast<int>(255.999 * color.y);
            int ib = static_cast<int>(255.999 * color.z);

            std::cout << ir << ' ' << ig << ' ' << ib << '\n';

        }
    }

    return 0;
}