#ifndef LIGHT_H
#define LIGHT_H

#include "../../c_rt01.h"
#include "../Vectors/vector3.h"

// ---------------------------------
// Light definition
// ---------------------------------

typedef struct Light
{
	struct Vector3 color;
	struct Vector3(*getDirection)(struct Light *this, struct Vector3 *shapePoint);
	void *derived;
	unsigned int samples;
} Light;

// ---------------------------------
// PointLight definition
// -> The point light is now obsolete, as the sphere light
//	  may have a radius of zero (although it still has inaccuracies).
// ---------------------------------

typedef struct PointLight
{
	struct Vector3 point;
} PointLight;

// ---------------------------------
// PointLight getDirection()
// ---------------------------------

struct Vector3 getPointLightDirection(struct Light *this, struct Vector3 *shapePoint);

// ---------------------------------
// PointLight constructor
// ---------------------------------

struct Light makePointLight(struct Vector3 *color, struct Vector3 *point);

// ---------------------------------
// DistantLight definition
// ---------------------------------

typedef struct DistantLight
{
	struct Vector3 direction;
} DistantLight;

// ---------------------------------
// DistantLight getDirection()
// ---------------------------------

struct Vector3 getDistantLightDirection(struct Light *this, struct Vector3 *shapePoint);

// ---------------------------------
// DistantLight constructor
// ---------------------------------

struct Light makeDistantLight(struct Vector3 *color, struct Vector3 *direction);

// ---------------------------------
// SphereLight definition
// ---------------------------------

typedef struct SphereLight
{
	struct Vector3 point;
	float radius;
} SphereLight;

// ---------------------------------
// SphereLight getDirection()
// ---------------------------------

struct Vector3 getSphereLightDirection(struct Light *this, struct Vector3 *shapePoint);

// ---------------------------------
// SphereLight constructor
// ---------------------------------

struct Light makeSphereLight(struct Vector3 *color, struct Vector3 *point, 
	float radius, unsigned int samples);

#endif