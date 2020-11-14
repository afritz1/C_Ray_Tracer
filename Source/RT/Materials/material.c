#include "material.h"
#include <float.h>
#include "../Lights/light.h"
#include <math.h>
#include "../Rays/ray.h"
#include "../Shapes/shape.h"
#include "../Worlds/world.h"

// ---------------------------------
// Flat
// ---------------------------------

Vector3 flatColorAt(Material *this, Vector3 *point, Vector3 *normal, Ray *ray, World *world)
{
	Flat *flat = (Flat*)this->derived;
	return flat->color;
}

unsigned char flatCastsShadow(Material *this)
{
	return FALSE;
}

Material makeFlat(Vector3 *color)
{
	Material m;
	m.colorAt = &flatColorAt;
	m.castsShadow = &flatCastsShadow;
	m.derived = malloc(sizeof(Flat));
	Flat *flat = (Flat*)m.derived;
	flat->color = *color;
	return m;
}

// ---------------------------------
// Phong
// ---------------------------------

Vector3 phongColorAt(Material *this, Vector3 *point, Vector3 *normal, Ray *ray, World *world)
{
	Phong *phong = (Phong*)this->derived;
	float vnDot =
		((-ray->direction.x * normal->x) +
		(-ray->direction.y * normal->y) +
		(-ray->direction.z * normal->z));
	char vnSign = vnDot < 0.0f ? -1 : (vnDot > 0.0f ? 1 : 0);
	Vector3 localNormal;
	localNormal.x = normal->x * vnSign;
	localNormal.y = normal->y * vnSign;
	localNormal.z = normal->z * vnSign;
	Vector3 color;
	color.x = phong->color.x * phong->ambient;
	color.y = phong->color.y * phong->ambient;
	color.z = phong->color.z * phong->ambient;
	if (phong->reflective && (ray->depth <= world->maxDepth))
	{
		Vector3 vnReflect;
		vnReflect.x = vnSign * localNormal.x * (2.0f * vnDot) - (-ray->direction.x);
		vnReflect.y = vnSign * localNormal.y * (2.0f * vnDot) - (-ray->direction.y);
		vnReflect.z = vnSign * localNormal.z * (2.0f * vnDot) - (-ray->direction.z);
		float vnRefLen = 1.0f / sqrtf(
			(vnReflect.x*vnReflect.x) +
			(vnReflect.y*vnReflect.y) +
			(vnReflect.z*vnReflect.z));
		vnReflect.x *= vnRefLen;
		vnReflect.y *= vnRefLen;
		vnReflect.z *= vnRefLen;
		Ray reflectRay;
		reflectRay.point.x = point->x + (vnReflect.x * EPSILON);
		reflectRay.point.y = point->y + (vnReflect.y * EPSILON);
		reflectRay.point.z = point->z + (vnReflect.z * EPSILON);
		reflectRay.direction = vnReflect;
		reflectRay.depth = ray->depth + 1;
		Intersection reflectTry = closestHit(&reflectRay, world->shapes);
		if (reflectTry.t != FLT_MAX)
		{
			Vector3 reflectColor =
				reflectTry.shape->material->colorAt(
				reflectTry.shape->material,
				&reflectTry.point,
				&reflectTry.normal,
				&reflectRay,
				world);
			color.x += reflectColor.x*phong->specular;
			color.y += reflectColor.y*phong->specular;
			color.z += reflectColor.z*phong->specular;
		}
		else
		{
			color.x += world->back_color.x*phong->specular;
			color.y += world->back_color.y*phong->specular;
			color.z += world->back_color.z*phong->specular;
		}
	}

	if (phong->refractive && (ray->depth <= world->maxDepth))
	{
		float iDot =
			-((-ray->direction.x*normal->x) +
			(-ray->direction.y*normal->y) +
			(-ray->direction.z*normal->z));
		float tDot = 1.0f - phong->refraction*phong->refraction * (1.0f - iDot*iDot);
		Vector3 vnRefract =
		{
			(phong->refraction * -ray->direction.x) + (phong->refraction * iDot - sqrtf(tDot)) * normal->x,
			(phong->refraction * -ray->direction.y) + (phong->refraction * iDot - sqrtf(tDot)) * normal->y,
			(phong->refraction * -ray->direction.z) + (phong->refraction * iDot - sqrtf(tDot)) * normal->z
		};
		float vnRefrLen = 1.0f / sqrtf(
			(vnRefract.x*vnRefract.x) +
			(vnRefract.y*vnRefract.y) +
			(vnRefract.z*vnRefract.z));
		vnRefract.x *= vnRefrLen;
		vnRefract.y *= vnRefrLen;
		vnRefract.z *= vnRefrLen;
		Ray refractRay;
		refractRay.point.x = point->x + (vnRefract.x * EPSILON);
		refractRay.point.y = point->y + (vnRefract.y * EPSILON);
		refractRay.point.z = point->z + (vnRefract.z * EPSILON);
		refractRay.direction = vnRefract;
		refractRay.depth = ray->depth + 1;
		Intersection refractTry = closestHit(&refractRay, world->shapes);
		if (refractTry.t != FLT_MAX)
		{
			Vector3 refractColor =
				refractTry.shape->material->colorAt(
				refractTry.shape->material,
				&refractTry.point,
				&refractTry.normal,
				&refractRay,
				world);
			color.x += refractColor.x*phong->specular;
			color.y += refractColor.y*phong->specular;
			color.z += refractColor.z*phong->specular;
		}
		else
		{
			color.x += world->back_color.x*phong->specular;
			color.y += world->back_color.y*phong->specular;
			color.z += world->back_color.z*phong->specular;
		}
	}

	for (unsigned int i = 0; world->lights[i].derived; i++)
	{
		Light *light = &world->lights[i];
		Vector3 ambAndDiff;
		ambAndDiff.x = phong->color.x * light->color.x;
		ambAndDiff.y = phong->color.y * light->color.y;
		ambAndDiff.z = phong->color.z * light->color.z;

		Vector3 sampledLight = { 0.0f, 0.0f, 0.0f };
		unsigned int n = light->samples;
		for (unsigned int j = 0; j < n; j++)
		{
			Vector3 lightDirection = light->getDirection(light, point);
			Ray shadowRay;
			shadowRay.point.x = point->x + (lightDirection.x * EPSILON);
			shadowRay.point.y = point->y + (lightDirection.y * EPSILON);
			shadowRay.point.z = point->z + (lightDirection.z * EPSILON);
			shadowRay.direction = lightDirection;
			float lnDot =
				(lightDirection.x * localNormal.x) +
				(lightDirection.y * localNormal.y) +
				(lightDirection.z * localNormal.z);
			char lnSign = lnDot < 0.0f ? -1 : (lnDot > 0.0f ? 1 : 0);
			Vector3 lnReflect;
			lnReflect.x = lnSign * localNormal.x * (2.0f * lnDot) - lightDirection.x;
			lnReflect.y = lnSign * localNormal.y * (2.0f * lnDot) - lightDirection.y;
			lnReflect.z = lnSign * localNormal.z * (2.0f * lnDot) - lightDirection.z;
			float lnRefLen = 1.0f / sqrtf(
				(lnReflect.x*lnReflect.x) +
				(lnReflect.y*lnReflect.y) +
				(lnReflect.z*lnReflect.z));
			lnReflect.x *= lnRefLen;
			lnReflect.y *= lnRefLen;
			lnReflect.z *= lnRefLen;
			float lnRefVDot =
				(lnReflect.x * -ray->direction.x) +
				(lnReflect.y * -ray->direction.y) +
				(lnReflect.z * -ray->direction.z);
			Intersection shadowTry = closestHit(&shadowRay, world->shapes);
			if (shadowTry.t == FLT_MAX ||
				!shadowTry.shape->material->castsShadow(shadowTry.shape->material))
			{
				sampledLight.x += ((ambAndDiff.x * phong->diffuse) * fmaxf(0.0f, lnDot)) +
					((light->color.x * phong->specularColor.x) * phong->specular *
					powf(fmaxf(0.0f, lnRefVDot), phong->shiny));
				sampledLight.y += (ambAndDiff.y * phong->diffuse) * fmaxf(0.0f, lnDot) +
					((light->color.y  * phong->specularColor.y) * phong->specular *
					powf(fmaxf(0.0f, lnRefVDot), phong->shiny));
				sampledLight.z += (ambAndDiff.z * phong->diffuse) * fmaxf(0.0f, lnDot) +
					((light->color.z * phong->specularColor.z) * phong->specular *
					powf(fmaxf(0.0f, lnRefVDot), phong->shiny));
			}
		}

		color.x += (sampledLight.x / n);
		color.y += (sampledLight.y / n);
		color.z += (sampledLight.z / n);
	}

	return color;
}

unsigned char phongCastsShadow(Material *this)
{
	Phong *phong = (Phong*)this->derived;
	return phong->shadows;
}

Material makePhong(Vector3 *color, Vector3 *specularColor,
	float ambient, float diffuse, float specular, float refraction,
	unsigned char shiny, unsigned char shadows, unsigned char reflective,
	unsigned char refractive)
{
	Material m;
	m.colorAt = &phongColorAt;
	m.castsShadow = &phongCastsShadow;
	m.derived = malloc(sizeof(Phong));
	Phong *phong = (Phong*)m.derived;
	phong->color = *color;
	phong->specularColor = *specularColor;
	phong->ambient = ambient;
	phong->diffuse = diffuse;
	phong->specular = specular;
	phong->refraction = refraction;
	phong->shiny = shiny;
	phong->shadows = shadows;
	phong->reflective = reflective;
	phong->refractive = refractive;
	return m;
}