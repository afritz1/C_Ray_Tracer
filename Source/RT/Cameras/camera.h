#ifndef CAMERA_H
#define CAMERA_H

#include "../../c_rt01.h"
#include "../Vectors/vector3.h"

struct Ray;

// ---------------------------------
// Camera definition
// ---------------------------------

typedef struct Camera 
{ 
	Vector3 eye, forward, right, up; 
	float aspect; 
} Camera;

// ---------------------------------
// Camera constructor
// ---------------------------------

struct Camera lookAt(struct Vector3 *eye, struct Vector3 *focus, struct Vector3 *up, 
	float fovY, float aspect);

// ---------------------------------
// Camera methods
// ---------------------------------

struct Ray imageRay(struct Camera *this, float x, float y);

#endif