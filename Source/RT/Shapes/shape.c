#include "shape.h"
#include <float.h>
#include "../Materials/material.h"
#include <math.h>
#include "../Rays/ray.h"

// ---------------------------------
// Sphere
// ---------------------------------

Intersection hitSphere(Shape *this, Ray *ray)
{
	Sphere *sphere = (Sphere*)this->derived;
	float a =
		(ray->direction.x*ray->direction.x) +
		(ray->direction.y*ray->direction.y) +
		(ray->direction.z*ray->direction.z);
	float b = 2.0f * (
		(ray->direction.x*(ray->point.x - this->point.x)) +
		(ray->direction.y*(ray->point.y - this->point.y)) +
		(ray->direction.z*(ray->point.z - this->point.z)));
	float c =
		(((ray->point.x - this->point.x)*(ray->point.x - this->point.x)) +
		((ray->point.y - this->point.y)*(ray->point.y - this->point.y)) +
		((ray->point.z - this->point.z)*(ray->point.z - this->point.z))) -
		(sphere->radius*sphere->radius);
	float discriminant = sqrtf((b*b) - (4.0f * a * c));
	float radRecip = 1.0f / sphere->radius;
	float t1 = (-b + discriminant) * (0.5f * a);
	float t2 = (-b - discriminant) * (0.5f * a);
	Intersection hit;
	hit.t = t2 > 0.0f ? t2 : (t1 > 0.0f ? t1 : FLT_MAX);
	hit.point.x = ray->point.x + (hit.t * ray->direction.x);
	hit.point.y = ray->point.y + (hit.t * ray->direction.y);
	hit.point.z = ray->point.z + (hit.t * ray->direction.z);
	hit.normal.x = (hit.point.x - this->point.x)*radRecip;
	hit.normal.y = (hit.point.y - this->point.y)*radRecip;
	hit.normal.z = (hit.point.z - this->point.z)*radRecip;
	hit.shape = this;
	return hit;
}

Shape makeSphere(Vector3 *point, Material *material, float radius)
{
	Shape s;
	s.point = *point;
	s.material = material;
	s.hit = &hitSphere;
	s.derived = malloc(sizeof(Sphere));
	Sphere *sphere = (Sphere*)s.derived;
	sphere->radius = radius;
	return s;
}

// ---------------------------------
// Plane
// ---------------------------------

Intersection hitPlane(Shape *this, Ray *ray)
{
	Plane *plane = (Plane*)this->derived;
	float vnDot =
		((ray->direction.x * plane->normal.x) +
		(ray->direction.y * plane->normal.y) +
		(ray->direction.z * plane->normal.z));
	if (vnDot != 0.0f)
	{
		float t =
			(((this->point.x - ray->point.x) * plane->normal.x) / vnDot) +
			(((this->point.y - ray->point.y) * plane->normal.y) / vnDot) +
			(((this->point.z - ray->point.z) * plane->normal.z) / vnDot);
		if (t > 0.0f)
		{
			Intersection hit;
			hit.t = t;
			hit.point.x = ray->point.x + (hit.t * ray->direction.x);
			hit.point.y = ray->point.y + (hit.t * ray->direction.y);
			hit.point.z = ray->point.z + (hit.t * ray->direction.z);
			hit.normal = plane->normal;
			hit.shape = this;
			return hit;
		}
	}

	Intersection noHit;
	noHit.t = FLT_MAX;
	return noHit;
}

Shape makePlane(Vector3 *point, Material *material, Vector3 *normal)
{
	Shape s;
	s.point = *point;
	s.material = material;
	s.hit = &hitPlane;
	s.derived = malloc(sizeof(Plane));
	Plane *plane = (Plane*)s.derived;
	plane->normal = *normal;
	float normLen = 1.0f / sqrtf(
		(plane->normal.x*plane->normal.x) +
		(plane->normal.y*plane->normal.y) +
		(plane->normal.z*plane->normal.z));
	plane->normal.x *= normLen;
	plane->normal.y *= normLen;
	plane->normal.z *= normLen;
	return s;
}

// ---------------------------------
// Cuboid
// ---------------------------------

Intersection hitCuboid(Shape *this, Ray *ray)
{
	Cuboid *cuboid = (Cuboid*)this->derived;
	Intersection intersection;
	intersection.t = FLT_MAX;
	Vector3 nMin, nMax;
	float tMin, tMax;
	float tX1 = (-cuboid->width + this->point.x - ray->point.x) / ray->direction.x;
	float tX2 = (cuboid->width + this->point.x - ray->point.x) / ray->direction.x;
	if (tX1 < tX2)
	{
		tMin = tX1;
		tMax = tX2;
		nMin.x = -cuboid->width;
		nMin.y = 0.0f;
		nMin.z = 0.0f;
		nMax.x = cuboid->width;
		nMax.y = 0.0f;
		nMax.z = 0.0f;
	}
	else
	{
		tMin = tX2;
		tMax = tX1;
		nMin.x = cuboid->width;
		nMin.y = 0.0f;
		nMin.z = 0.0f;
		nMax.x = -cuboid->width;
		nMax.y = 0.0f;
		nMax.z = 0.0f;
	}

	if (tMin > tMax)
	{
		return intersection;
	}

	float tY1 = (-cuboid->height + this->point.y - ray->point.y) / ray->direction.y;
	float tY2 = (cuboid->height + this->point.y - ray->point.y) / ray->direction.y;

	if (tY1 < tY2)
	{
		if (tY1 > tMin)
		{
			tMin = tY1;
			nMin.x = 0.0f;
			nMin.y = -cuboid->height;
			nMin.z = 0.0f;
		}
		if (tY2 < tMax)
		{
			tMax = tY2;
			nMax.x = 0.0f;
			nMax.y = cuboid->height;
			nMax.z = 0.0f;
		}
	}
	else
	{
		if (tY2 > tMin)
		{
			tMin = tY2;
			nMin.x = 0.0f;
			nMin.y = cuboid->height;
			nMin.z = 0.0f;
		}
		if (tY1 < tMax)
		{
			tMax = tY1;
			nMax.x = 0.0f;
			nMax.y = -cuboid->height;
			nMax.z = 0.0f;
		}
	}

	if (tMin > tMax)
	{
		return intersection;
	}

	float tZ1 = (-cuboid->depth + this->point.z - ray->point.z) / ray->direction.z;
	float tZ2 = (cuboid->depth + this->point.z - ray->point.z) / ray->direction.z;

	if (tZ1 < tZ2)
	{
		if (tZ1 > tMin)
		{
			tMin = tZ1;
			nMin.x = 0.0f;
			nMin.y = 0.0f;
			nMin.z = -cuboid->depth;
		}
		if (tZ2 < tMax)
		{
			tMax = tZ2;
			nMax.x = 0.0f;
			nMax.y = 0.0f;
			nMax.z = cuboid->depth;
		}
	}
	else
	{
		if (tZ2 > tMin)
		{
			tMin = tZ2;
			nMin.x = 0.0f;
			nMin.y = 0.0f;
			nMin.z = cuboid->depth;
		}
		if (tZ1 < tMax)
		{
			tMax = tZ1;
			nMax.x = 0.0f;
			nMax.y = 0.0f;
			nMax.z = -cuboid->depth;
		}
	}

	if (tMin > tMax)
	{
		return intersection;
	}

	if (tMin < 0.0f)
	{
		tMin = tMax;
		nMin = nMax;
	}
	
	if (tMin >= 0.0f)
	{
		intersection.t = tMin;
		intersection.point.x = ray->point.x + (intersection.t * ray->direction.x);
		intersection.point.y = ray->point.y + (intersection.t * ray->direction.y);
		intersection.point.z = ray->point.z + (intersection.t * ray->direction.z);
		float normLen = 1.0f / sqrtf((nMin.x*nMin.x) + (nMin.y*nMin.y) + (nMin.z*nMin.z));
		intersection.normal.x = nMin.x * normLen;
		intersection.normal.y = nMin.y * normLen;
		intersection.normal.z = nMin.z * normLen;
		intersection.shape = this;
	}

	return intersection;
}

Shape makeCuboid(Vector3 *point, Material *material, float width,
	float height, float depth)
{
	Shape s;
	s.point = *point;
	s.material = material;
	s.hit = &hitCuboid;
	s.derived = malloc(sizeof(Cuboid));
	Cuboid *cuboid = (Cuboid*)s.derived;
	cuboid->width = width;
	cuboid->height = height;
	cuboid->depth = depth;
	return s;
}