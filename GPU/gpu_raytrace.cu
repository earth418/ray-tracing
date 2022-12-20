#include <iostream>
#include <fstream>
#include <cuda.h>

#include "utils.h"
#include "scene.h"

#define pi 3.14159265358979323

__device__ Vec3 RayTrace(const World* world, const Vec3& rayOrigin, const Vec3& rayDireciton);

struct raytraceInfo {

    int IMG_HEIGHT;
    int IMG_WIDTH;

    Quat cameraRot;
    Vec3 cameraPos;
    float focal_length;

};

struct Int3 {

    int x;
    int y;
    int z;

    CUDA_HEAD Int3()
    : x(0), y(0), z(0)
    {}

    CUDA_HEAD Int3(int a, int b, int c)
    : x(a), y(b), z(c) 
    {}

    CUDA_HEAD Int3(Vec3 in)
    : Int3(
        static_cast<int>(in.x), 
        static_cast<int>(in.y),
        static_cast<int>(in.z)) 
    {}

    
};

__global__ void rayTraceGPU(Int3* colors, const World* world, const raytraceInfo& info) {

    int row = threadIdx.y + blockDim.y * blockIdx.y;
    int col = threadIdx.x + blockDim.x * blockIdx.x;
    
    int index = row * info.IMG_WIDTH + col;

    colors[index] = Int3(255, 255, 255);
    return;

    if (row > info.IMG_HEIGHT || col > info.IMG_WIDTH)
        return;

    
    float y = float(row), z = float(info.IMG_HEIGHT - col - 1);

    float alphay = y / (float) (info.IMG_WIDTH - 1);
    float alphaz = z / (float) (info.IMG_HEIGHT - 1);

    Vec3 rel_viewpoint_pos = Vec3(info.focal_length, -0.5 + alphaz, 0.5 - alphay);
    rel_viewpoint_pos = info.cameraRot.RotateVector(rel_viewpoint_pos);

    Vec3 color = RayTrace(world, info.cameraPos, rel_viewpoint_pos.normalize());

    colors[index] = Int3(color * 255.999);
    colors[index] = Int3(255, 255, 255);
}

__device__ Vec3 RayTrace(const World* world, const Vec3& rayOrigin, const Vec3& rayDireciton) {

    RayIntersectInfo closestRay{};

    // First run-through to determine what object it hits
    // for (SceneObject* object : world->objects) {
    for (int i = 0; i < MAX_OBJS; ++i) {
        const SceneObject* object = world->objects[i];

        RayIntersectInfo rayInfo{};

        if (object->RayObjectIntersect(rayOrigin, rayDireciton, rayInfo) 
            && rayInfo.distance < closestRay.distance)
            closestRay = rayInfo;

    }

    // No hit; return background color
    if (closestRay.distance == INFINITY)
        return Vec3(0.3, 0.4, 0.7);

    float lightIntensity = 0.0;
    float maxIntensity = 0.0;
    // for (LightSource* light : world->lights) {
    for (int i = 0; i < MAX_LGHT; ++i) {

        const LightSource* light = world->lights[i];

        maxIntensity += light->intensity;

        Vec3 lightDir = light->getDirection(closestRay.intersectPoint);

        RayIntersectInfo shadowRayInfo{};

        // Second run-through for shadows (and maybe reflections too idk)
        // for (SceneObject* object : world->objects) {
        for (int i = 0; i < MAX_OBJS; ++i) {
            const SceneObject* object = world->objects[i];

            if (object == closestRay.hitObj || object->castShadow == false)
                continue; 

            RayIntersectInfo rayInfo{};

            if (object->RayObjectIntersect(closestRay.intersectPoint, -1.0 * lightDir, rayInfo, 0.0001)
                && rayInfo.distance < shadowRayInfo.distance)
                    shadowRayInfo = rayInfo;

        }

        if (shadowRayInfo.distance == INFINITY) { // No hit
            lightIntensity += light->calculateIntensity(closestRay.planeNormal, 
                closestRay.intersectPoint);
        }
        
    }

    lightIntensity /= maxIntensity;
    
    // Vec3 P = rayOrigin + closest_dist * rayDireciton;
    // Vec3 SphereNormal = closest_object.NormalAtPoint(P); // (P - closest_object.position) / closest_object.radius;
    // float alpha = (1.0 - world->lightDirection.dot(closestIntersect.planeNormal)) * 0.5;
    return lightIntensity * closestRay.pointColor;
}

int main(int argc, char** argv) {

    const int IMG_HEIGHT = 1600;
    const int IMG_WIDTH = 1600;

    World* w = new World();
    Sphere* s;
    
    // s = new Sphere(Vec3(5.0, 0.0, 2.0), 2.0);
    // s->color = Vec3(0.0, 1.0, 0.0);
    // w->objects.push_back(s);

    s = new Sphere(Vec3(0.0, 0.0, 1.5), 1.0);
    s->color = Vec3(0.0, 0.0, 1.0);
    // w->objects.push_back(s);
    w->objects[0] = s;

    Sphere* gnd = new Sphere(Vec3(0.0, 0.0, -500.0), 500.0);
    gnd->color = Vec3(0.5, 0.0, 0.5).normalize();
    // w->objects.push_back(gnd);
    w->objects[1] = gnd;

    // Where the light is coming from
    Vec3 lightDir = Vec3(0.0, 0.0, -1.0).normalize();
    // w->lights.push_back(new DirectionalLight(lightDir, 1.0));
    w->lights[0] = new DirectionalLight(lightDir, 1.0);

    PointLight* p = new PointLight();
    p->intensity = 0.5;
    p->position = Vec3(1.0, 1.0, 3.0);
    // w->lights.push_back(p);
    w->lights[1] = p;

    s = new Sphere(p->position, 0.5);
    s->color = Vec3(1.0);
    s->castShadow = false;
    // w->objects.push_back(s);
    w->objects[2] = s;

    // SceneObject lightSource = SceneObject();


    // TriMesh* t = new TriMesh("../cube.obj");
    // // TriMesh* t = new TriMesh();
    
    // t->position = Vec3(3.0, 0.0, 0.0);
    // // t->scale = Vec3(0.1, 1.0, 5.0);

    // // t->verts.push_back(Vec3(0.0, 0.0, 0.0));
    // // t->verts.push_back(Vec3(0.0, 3.0, 0.0));
    // // t->verts.push_back(Vec3(0.0, 0.0, 3.0));
    // // t->verts.push_back(Vec3(0.0, 3.0, 3.0));

    // t->color = Vec3(1.0, 1.0, 0.0);

    // // t->tris.push_back(TriangleInfo(2, 1, 0));
    // // t->tris.push_back(TriangleInfo(2, 3, 1));

    // w->objects.push_back(t);

    // for (Vec3 v : t->verts) {
    //     w->objects.push_back(new Sphere(v + t->position, 0.25));
    // }

    char filename[32];
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

    size_t size_data = IMG_WIDTH * IMG_HEIGHT * sizeof(Int3);
    Int3* gpuColors;
    Int3* colors = new Int3[IMG_WIDTH * IMG_HEIGHT];

    cudaMalloc((void**) &gpuColors, size_data);

    int threadsPer = 16;
    // block dimension
    dim3 Block(threadsPer, threadsPer);
    // grid of blocks
    // dim3 Grid(IMG_WIDTH / threadsPer, IMG_HEIGHT / threadsPer);
    dim3 Grid(1, 1);

    raytraceInfo rtInfo;

    rtInfo.IMG_HEIGHT = IMG_HEIGHT;
    rtInfo.IMG_WIDTH = IMG_WIDTH;
    rtInfo.focal_length = focal_length;
    
    cudaEvent_t start, stop; // using cuda events to measure time
    float elapsed_time_ms; // which is applicable for asynchronous code also
    cudaError_t errorcode;

    int gpucount = 0;

    errorcode = cudaGetDeviceCount(&gpucount);
    if (errorcode == cudaErrorNoDevice) {
        printf("No GPUs are visible\n");
        exit(-1);
    }
    else {
        printf("Device count = %d\n",gpucount);
    }
    
    cudaSetDevice(0);
    printf("Using device %d\n",0);

    for (int frame = 0; frame < N_FRAMES; ++frame) {

        float frameAlpha = 0.0;

        if (N_FRAMES > 1)
            frameAlpha = float(frame) / float(N_FRAMES - 1);

        // Create the image file for this frame
        sprintf(filename, "./images/image%04d.ppm", frame);

        std::ofstream imgout(filename); // std::format("./images/image{frame:4d}.ppm"));

        Quat rot = Quat(Vec3::up(), pi * 2.0 * frameAlpha); 
        Vec3 cameraPos = rot.RotateVector(Vec3(-5.0, 0.0, 1.0));

        imgout << "P3\n" << IMG_WIDTH << ' ' << IMG_HEIGHT << "\n255\n";


        rtInfo.cameraPos = cameraPos;
        rtInfo.cameraRot = rot;

        // Timing stuff start!
        cudaEventCreate(&start); // instrument code to measure start time
        cudaEventCreate(&stop);
        cudaEventRecord(start, 0);

        // Calls the kernel
        printf("Launching kernel\n");
        rayTraceGPU<<<Grid, Block>>>(gpuColors, w, rtInfo);

        // Timing stuff end!
        cudaEventRecord(stop, 0); // instrument code to measure end time
        cudaEventSynchronize(stop);
        cudaEventElapsedTime(&elapsed_time_ms, start, stop );

        printf("Back from kernel\n");
        cudaMemcpy(colors, gpuColors, size_data, cudaMemcpyDeviceToHost);

        printf("Frame %d/%d took %f ms.\n", frame+1, N_FRAMES, elapsed_time_ms); 

        for (int i = 0; i < IMG_WIDTH; ++i) {
            for (int j = IMG_HEIGHT - 1; j >= 0; --j) {

                int i2 = i, j2 = IMG_HEIGHT - j - 1;

                // float alphay = y / (float) (IMG_WIDTH - 1);
                // float alphaz = z / (float) (IMG_HEIGHT - 1);

                // Vec3 rel_viewpoint_pos = Vec3(focal_length, -0.5 + alphaz, 0.5 - alphay);
                // rel_viewpoint_pos = rot.RotateVector(rel_viewpoint_pos);

                // Vec3 direction = rel_viewpoint_pos.normalize();
            
                // Vec3 color = RayTrace(w, cameraPos, direction);

                // int ir = static_cast<int>(255.999 * color.x);
                // int ig = static_cast<int>(255.999 * color.y);
                // int ib = static_cast<int>(255.999 * color.z);
                Int3 i = colors[i2 * IMG_WIDTH + j2]; 


                imgout << i.x << ' ' << i.y << ' ' << i.z << '\n';

            }
            
        }

        imgout.close();
    }

    // for (SceneObject* s : w->objects)
    for (int i = 0; i < MAX_OBJS; ++i)
        delete w->objects[i];

    

    for (int i = 0; i < MAX_LGHT; ++i)
        delete w->lights[i];

    return 0;
}