#include "light.h"
#include <math.h>

// ---------------------------------
// PointLight
// ---------------------------------

Vector3 getPointLightDirection(Light *this, Vector3 *shapePoint)
{
	PointLight *pl = (PointLight*)this->derived;
	Vector3 dir;
	dir.x = pl->point.x - shapePoint->x;
	dir.y = pl->point.y - shapePoint->y;
	dir.z = pl->point.z - shapePoint->z;
	float dirLen = 1.0f / sqrtf((dir.x*dir.x) + (dir.y*dir.y) + (dir.z*dir.z));
	dir.x *= dirLen;
	dir.y *= dirLen;
	dir.z *= dirLen;
	return dir;
}

Light makePointLight(Vector3 *color, Vector3 *point)
{
	Light l;
	l.color = *color;
	l.getDirection = &getPointLightDirection;
	l.samples = 1;
	l.derived = malloc(sizeof(PointLight));
	PointLight *pl = (PointLight*)l.derived;
	pl->point = *point;
	return l;
}

// ---------------------------------
// DistantLight definition
// ---------------------------------

Vector3 getDistantLightDirection(Light *this, Vector3 *shapePoint)
{
	DistantLight *dl = (DistantLight*)this->derived;
	return dl->direction;
}

Light makeDistantLight(Vector3 *color, Vector3 *direction)
{
	Light l;
	l.color = *color;
	l.getDirection = &getDistantLightDirection;
	l.samples = 1;
	l.derived = malloc(sizeof(DistantLight));
	DistantLight *dl = (DistantLight*)l.derived;
	dl->direction = *direction;
	float dirLen = 1.0f / sqrtf(
		(dl->direction.x*dl->direction.x) + 
		(dl->direction.y*dl->direction.y) + 
		(dl->direction.z*dl->direction.z));
	dl->direction.x *= dirLen;
	dl->direction.y *= dirLen;
	dl->direction.z *= dirLen;
	return l;
}

// ---------------------------------
// SphereLight
// ---------------------------------

Vector3 getSphereLightDirection(Light *this, Vector3 *shapePoint)
{
	SphereLight *sl = (SphereLight*)this->derived;	
	Vector3 diff;
	diff.x = (float)(rand() % 3) - 1.0f;
	diff.y = (float)(rand() % 3) - 1.0f;
	diff.z = (float)(rand() % 3) - 1.0f;
	float diffLen = 1.0f / sqrtf(
		(diff.x*diff.x) + 
		(diff.y*diff.y) + 
		(diff.z*diff.z));
	diff.x *= diffLen * (sl->radius * ((float)(rand() % 101) / 100.0f));
	diff.y *= diffLen * (sl->radius * ((float)(rand() % 101) / 100.0f));
	diff.z *= diffLen * (sl->radius * ((float)(rand() % 101) / 100.0f));
	Vector3 dir =
	{
		(sl->point.x + diff.x) - shapePoint->x,
		(sl->point.y + diff.y) - shapePoint->y,
		(sl->point.z + diff.z) - shapePoint->z
	}; 
	float dirLen = 1.0f / sqrtf((dir.x*dir.x) + (dir.y*dir.y) + (dir.z*dir.z));
	dir.x *= dirLen;
	dir.y *= dirLen;
	dir.z *= dirLen;
	return dir;
}

Light makeSphereLight(Vector3 *color, Vector3 *point, float radius, unsigned int samples)
{
	Light l;
	l.color = *color;
	l.getDirection = &getSphereLightDirection;
	l.samples = samples;
	l.derived = malloc(sizeof(SphereLight));
	SphereLight *dl = (SphereLight*)l.derived;
	dl->point = *point;
	dl->radius = radius;
	return l;
}