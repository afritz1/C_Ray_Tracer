#include "ray.h"
#include <float.h>
#include "../Shapes/shape.h"

Intersection closestHit(Ray *this, Shape *shapes)
{
	Intersection nearestHit, currentAttempt;
	nearestHit.t = FLT_MAX;
	for (unsigned int i = 0; shapes[i].derived; i++)
	{
		currentAttempt = shapes[i].hit(&shapes[i], this);
		if (currentAttempt.t < nearestHit.t) nearestHit = currentAttempt;
	}

	return nearestHit;
}