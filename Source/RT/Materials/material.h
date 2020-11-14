#ifndef MATERIAL_H
#define MATERIAL_H

#include "../../c_rt01.h"
#include "../Vectors/vector3.h"

struct Ray;
struct World;

// ---------------------------------
// Material definition
// ---------------------------------

typedef struct Material
{
	void *derived;
	struct Vector3(*colorAt)
		(struct Material *this, struct Vector3 *point, struct Vector3 *normal,
		struct Ray *ray, struct World *world);
	unsigned char(*castsShadow)(struct Material *this);
} Material;

// ---------------------------------
// Flat definition
// ---------------------------------

typedef struct Flat
{
	struct Vector3 color;
} Flat;

// ---------------------------------
// Flat colorAt()
// ---------------------------------

struct Vector3 flatColorAt(struct Material *this, struct Vector3 *point,
	struct Vector3 *normal, struct Ray *ray, struct World *world);

// ---------------------------------
// Flat castsShadow
// ---------------------------------

unsigned char flatCastsShadow(struct Material *this);

// ---------------------------------
// Flat constructor
// ---------------------------------

struct Material makeFlat(struct Vector3 *color);

// ---------------------------------
// Phong definition
// ---------------------------------

typedef struct Phong
{
	struct Vector3 color, specularColor;
	float ambient, diffuse, specular, refraction;
	unsigned char shiny, shadows, reflective, refractive;
} Phong;

// ---------------------------------
// Phong colorAt()
// ---------------------------------

struct Vector3 phongColorAt(struct Material *this, struct Vector3 *point,
	struct Vector3 *normal, struct Ray *ray, struct World *world);

// ---------------------------------
// Phong castsShadow
// ---------------------------------

unsigned char phongCastsShadow(struct Material *this);

// ---------------------------------
// Phong constructor
// ---------------------------------

struct Material makePhong(struct Vector3 *color, struct Vector3 *specularColor,
	float ambient, float diffuse, float specular, float refraction, 
	unsigned char shiny, unsigned char shadows, unsigned char reflective,
	unsigned char refractive);

#endif