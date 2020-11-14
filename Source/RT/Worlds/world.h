#ifndef WORLD_H
#define WORLD_H

#include "../../c_rt01.h"
#include "../Vectors/vector3.h"

#define SPHERE_COUNT 4
#define PLANE_COUNT 1
#define CUBOID_COUNT 4
#define POINT_LIGHT_COUNT 0
#define DIST_LIGHT_COUNT 0
#define SPHERE_LIGHT_COUNT 1
#define WORLD_SIZE 16
#define MAX_DEPTH 6
#define LIGHT_SAMPLES 8

struct Shape;
struct Light;
struct Camera;

typedef struct World
{
	struct Shape *shapes;
	struct Light *lights;
	struct Camera *camera;
	struct Vector3 back_color;
	unsigned short size;
	unsigned char maxDepth;
} World;

struct Vector3 colorAt(struct World *this, float x, float y);

World makeWorld(float aspect);
void addPointLight(struct World *this, unsigned int i);
void addDistantLight(struct World *this, unsigned int i);
void addSphereLight(struct World *this, unsigned int i);
void addSphere(struct World *this, unsigned int i);
void addPlane(struct World *this, unsigned int i);
void addCuboid(struct World *this, unsigned int i);
void destructWorld(struct World *world);

#endif