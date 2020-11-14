#include "world.h"
#include "../Cameras/camera.h"
#include <float.h>
#include "../Lights/light.h"
#include "../Materials/material.h"
#include "../Rays/ray.h"
#include "../Shapes/shape.h"

World makeWorld(float aspect)
{
	World w;
	Vector3 eye = { 3.0f, 5.0f, 14.0f };
	Vector3 focus = { 0.0f, 0.0f, 0.0f };
	Vector3 up = { 0.0f, 1.0f, 0.0f };
	w.camera = (Camera*)calloc(1, sizeof(Camera));
	*w.camera = lookAt(&eye, &focus, &up, 90.0f, aspect);

	w.size = WORLD_SIZE;
	w.maxDepth = MAX_DEPTH;

	w.shapes = (Shape*)calloc(SPHERE_COUNT + PLANE_COUNT + CUBOID_COUNT + 1, sizeof(Shape));
	for (unsigned int i = 0; i < SPHERE_COUNT; i++)
	{
		addSphere(&w, i);
	}

	for (unsigned int i = SPHERE_COUNT; i < SPHERE_COUNT + PLANE_COUNT; i++)
	{
		addPlane(&w, i);
	}

	for (unsigned int i = SPHERE_COUNT + PLANE_COUNT;
		i < SPHERE_COUNT + PLANE_COUNT + CUBOID_COUNT; i++)
	{
		addCuboid(&w, i);
	}

	memset(&w.back_color, 0, sizeof(Vector3));
	w.lights = (Light*)calloc(POINT_LIGHT_COUNT + DIST_LIGHT_COUNT + 
		SPHERE_LIGHT_COUNT + 1, sizeof(Light));
	for (unsigned int i = 0; i < POINT_LIGHT_COUNT; i++)
	{
		addPointLight(&w, i);
	}

	for (unsigned int i = POINT_LIGHT_COUNT; i < POINT_LIGHT_COUNT + DIST_LIGHT_COUNT; i++)
	{
		addDistantLight(&w, i);
	}

	for (unsigned int i = POINT_LIGHT_COUNT + DIST_LIGHT_COUNT;
		i < POINT_LIGHT_COUNT + DIST_LIGHT_COUNT + SPHERE_LIGHT_COUNT; i++)
	{
		addSphereLight(&w, i);
	}

	w.back_color.x /= (POINT_LIGHT_COUNT + DIST_LIGHT_COUNT + SPHERE_LIGHT_COUNT);
	w.back_color.y /= (POINT_LIGHT_COUNT + DIST_LIGHT_COUNT + SPHERE_LIGHT_COUNT);
	w.back_color.z /= (POINT_LIGHT_COUNT + DIST_LIGHT_COUNT + SPHERE_LIGHT_COUNT);

	return w;
}

Vector3 colorAt(World *this, float x, float y)
{
	Ray imgRay = imageRay(this->camera, x, y);
	Intersection nearestHit = closestHit(&imgRay, this->shapes);
	Vector3 color;
	if (nearestHit.t == FLT_MAX)
	{
		color = this->back_color;
	}
	else
	{
		color = nearestHit.shape->material->colorAt(
			nearestHit.shape->material,
			&nearestHit.point,
			&nearestHit.normal,
			&imgRay,
			this);
	}

	color.x = color.x < 0.0f ? 0.0f : (color.x > 1.0f ? 1.0f : color.x);
	color.y = color.y < 0.0f ? 0.0f : (color.y > 1.0f ? 1.0f : color.y);
	color.z = color.z < 0.0f ? 0.0f : (color.z > 1.0f ? 1.0f : color.z);

	return color;
}

void addPointLight(World *this, unsigned int i)
{
	Vector3 point;
	point.x = (float)(rand() % this->size) - (this->size * 0.5f);
	point.y = (float)(rand() % this->size) + (this->size * 0.5f);
	point.z = (float)(rand() % this->size) - (this->size * 0.5f);
	Vector3 color;
	color.x = (float)((rand() % 101) / 100.0f);
	color.y = (float)((rand() % 101) / 100.0f);
	color.z = (float)((rand() % 101) / 100.0f);
	this->lights[i] = makePointLight(&color, &point);
	this->back_color.x += this->lights[i].color.x;
	this->back_color.y += this->lights[i].color.y;
	this->back_color.z += this->lights[i].color.z;
}

void addDistantLight(World *this, unsigned int i)
{
	Vector3 direction;
	direction.x = (float)(rand() % this->size) - (this->size * 0.5f);
	direction.y = (float)(rand() % this->size) + (this->size * 0.5f);
	direction.z = (float)(rand() % this->size) - (this->size * 0.5f);
	float dirLen = 1.0f / sqrtf(
		(direction.x*direction.x) +
		(direction.y*direction.y) +
		(direction.z*direction.z));
	direction.x *= dirLen;
	direction.y *= dirLen;
	direction.z *= dirLen;
	Vector3 color;
	color.x = (float)((rand() % 101) / 100.0f);
	color.y = (float)((rand() % 101) / 100.0f);
	color.z = (float)((rand() % 101) / 100.0f);
	this->lights[i] = makeDistantLight(&color, &direction);
	this->back_color.x += this->lights[i].color.x;
	this->back_color.y += this->lights[i].color.y;
	this->back_color.z += this->lights[i].color.z;
}

void addSphereLight(World *this, unsigned int i)
{
	Vector3 point;
	point.x = (float)(rand() % this->size) - (this->size * 0.5f);
	point.y = (float)(rand() % this->size) + (this->size * 0.5f);
	point.z = (float)(rand() % this->size) - (this->size * 0.5f);
	float radius = 0.50f;
	Vector3 color;
	color.x = (float)((rand() % 100) / 100.0f);
	color.y = (float)((rand() % 100) / 100.0f);
	color.z = (float)((rand() % 100) / 100.0f);
	this->lights[i] = makeSphereLight(&color, &point, radius, LIGHT_SAMPLES);
	this->back_color.x += this->lights[i].color.x;
	this->back_color.y += this->lights[i].color.y;
	this->back_color.z += this->lights[i].color.z;
}

void addSphere(World *this, unsigned int i)
{
	Vector3 point;
	point.x = (float)(rand() % this->size) - (this->size * 0.5f);
	point.y = (float)(rand() % this->size) - (this->size * 0.5f);
	point.z = (float)(rand() % this->size) - (this->size * 0.5f);
	float radius = (0.5f + (float)((rand() % 100) / 100.0f));
	Vector3 color;
	color.x = (float)((rand() % 100) / 100.0f);
	color.y = (float)((rand() % 100) / 100.0f);
	color.z = (float)((rand() % 100) / 100.0f);
	Vector3 specularColor;
	specularColor.x = (float)((rand() % 100) / 100.0f);
	specularColor.y = (float)((rand() % 100) / 100.0f);
	specularColor.z = (float)((rand() % 100) / 100.0f);
	Material *material = (Material*)calloc(1, sizeof(Material));
	*material = makePhong(&color, &specularColor, 0.30f, 0.95f, 0.40f, 0.45f, 16, TRUE, TRUE, FALSE);
	this->shapes[i] = makeSphere(&point, material, radius);
}

void addPlane(World *this, unsigned int i)
{
	Vector3 point;
	point.x = (float)(rand() % this->size) - (this->size * 0.5f);
	point.y = -8.0f;
	point.z = (float)(rand() % this->size) - (this->size * 0.5f);
	Vector3 color;
	color.x = (float)((rand() % 100) / 100.0f);
	color.y = (float)((rand() % 100) / 100.0f);
	color.z = (float)((rand() % 100) / 100.0f);
	Vector3 specularColor;
	specularColor.x = (float)((rand() % 100) / 100.0f);
	specularColor.y = (float)((rand() % 100) / 100.0f);
	specularColor.z = (float)((rand() % 100) / 100.0f);
	Material *material = (Material*)calloc(1, sizeof(Material));
	*material = makePhong(&color, &specularColor, 0.3f, 0.95f, 0.40f, 0.40f, 16, TRUE, TRUE, FALSE);
	Vector3 normal = { 0.0f, 1.0f, 0.0f };
	this->shapes[i] = makePlane(&point, material, &normal);
}

void addCuboid(World *this, unsigned int i)
{
	Vector3 point;
	point.x = (float)(rand() % this->size) - (this->size * 0.5f);
	point.y = (float)(rand() % this->size) - (this->size * 0.5f);
	point.z = (float)(rand() % this->size) - (this->size * 0.5f);
	float width = (0.5f + (float)((rand() % 100) / 100.0f));
	float height = (0.5f + (float)((rand() % 100) / 100.0f));
	float depth = (0.5f + (float)((rand() % 100) / 100.0f));
	Vector3 color;
	color.x = (float)((rand() % 100) / 100.0f);
	color.y = (float)((rand() % 100) / 100.0f);
	color.z = (float)((rand() % 100) / 100.0f);
	Vector3 specularColor;
	specularColor.x = (float)((rand() % 100) / 100.0f);
	specularColor.y = (float)((rand() % 100) / 100.0f);
	specularColor.z = (float)((rand() % 100) / 100.0f);
	Material *material = (Material*)calloc(1, sizeof(Material));
	*material = makePhong(&color, &specularColor, 0.3f, 0.95f, 0.40f, 0.20f, 16, TRUE, TRUE, FALSE);
	this->shapes[i] = makeCuboid(&point, material, width, height, depth);
}

void destructWorld(World *this)
{
	for (unsigned int i = 0; i < SPHERE_COUNT + PLANE_COUNT + CUBOID_COUNT; i++)
	{
		free(this->shapes[i].material);
		free(this->shapes[i].derived);
	}
	for (unsigned int i = 0; 
		i < POINT_LIGHT_COUNT + DIST_LIGHT_COUNT + SPHERE_LIGHT_COUNT; i++)
	{
		free(this->lights[i].derived);
	}

	free(this->shapes);
	free(this->lights);
	free(this->camera);
}