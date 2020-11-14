#ifndef SHAPE_H
#define SHAPE_H

#include "../../c_rt01.h"
#include "../Vectors/vector3.h"

struct Intersection;
struct Material;
struct Ray;

// ---------------------------------
// Shape definition
// ---------------------------------

typedef struct Shape
{
	struct Vector3 point;
	struct Material *material;
	struct Intersection (*hit)(struct Shape *this, struct Ray *ray);
	void *derived;
} Shape;

// ---------------------------------
// Intersection definition
// ---------------------------------

typedef struct Intersection
{
	float t;
	struct Vector3 point, normal;
	struct Shape *shape;
} Intersection;

// ---------------------------------
// Sphere definition
// ---------------------------------

typedef struct Sphere
{
	float radius;
} Sphere;

// ---------------------------------
// Sphere hit()
// ---------------------------------

struct Intersection hitSphere(struct Shape *this, struct Ray *ray);

// ---------------------------------
// Sphere constructor
// ---------------------------------

struct Shape makeSphere(struct Vector3 *point, struct Material *material, float radius);

// ---------------------------------
// Plane definition
// ---------------------------------

typedef struct Plane
{
	struct Vector3 normal;
} Plane;

// ---------------------------------
// Plane hit()
// ---------------------------------

struct Intersection hitPlane(struct Shape *this, struct Ray *ray);

// ---------------------------------
// Plane constructor
// ---------------------------------

struct Shape makePlane(struct Vector3 *point, struct Material *material, struct Vector3 *normal);

// ---------------------------------
// Cuboid definition
// ---------------------------------

typedef struct Cuboid
{
	float width, height, depth;
} Cuboid;

// ---------------------------------
// Cuboid hit()
// ---------------------------------

struct Intersection hitCuboid(struct Shape *this, struct Ray *ray);

// ---------------------------------
// Cuboid constructor
// ---------------------------------

struct Shape makeCuboid(struct Vector3 *point, struct Material *material,
	float width, float height, float depth);

#endif