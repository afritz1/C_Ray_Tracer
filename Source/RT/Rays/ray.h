#ifndef RAY_H
#define RAY_H

#include "../../c_rt01.h"
#include "../Vectors/vector3.h"

#define INIT_DEPTH 1

struct Intersection;
struct Shape;

// ---------------------------------
// Ray definition
// ---------------------------------

typedef struct Ray 
{ 
	struct Vector3 point, direction; 
	unsigned char depth;
} Ray;

// ---------------------------------
// Ray closestHit()
// ---------------------------------

struct Intersection closestHit(struct Ray *this, struct Shape *shapes);

#endif