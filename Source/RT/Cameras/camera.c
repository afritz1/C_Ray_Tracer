#include "camera.h"
#include <math.h>
#include "../Rays/ray.h"

Camera lookAt(Vector3 *eye, Vector3 *focus, Vector3 *up, float fovY, float aspect)
{
	float fovYToRad = fovY * (PI / 180.0f);
	float imgHalfHeight = tanf(fovYToRad / 2.0f);
	float imgHalfWidth = imgHalfHeight * aspect;

	Camera c;
	c.eye = *eye;
	c.aspect = aspect;

	c.forward.x = focus->x - eye->x;
	c.forward.y = focus->y - eye->y;
	c.forward.z = focus->z - eye->z;
	float fwdLen = 1.0f / sqrtf(
		(c.forward.x*c.forward.x) +
		(c.forward.y*c.forward.y) +
		(c.forward.z*c.forward.z));
	c.forward.x *= fwdLen;
	c.forward.y *= fwdLen;
	c.forward.z *= fwdLen;

	c.right.x = (c.forward.y * up->z) - (up->y * c.forward.z);
	c.right.y = (up->x * c.forward.z) - (c.forward.x * up->z);
	c.right.z = (c.forward.x * up->y) - (up->x * c.forward.y);
	float rgtLen = 1.0f / sqrtf(
		(c.right.x*c.right.x) +
		(c.right.y*c.right.y) +
		(c.right.z*c.right.z));
	c.right.x *= rgtLen;
	c.right.y *= rgtLen;
	c.right.z *= rgtLen;

	c.up.x = (c.right.y * c.forward.z) - (c.forward.y * c.right.z);
	c.up.y = (c.forward.x * c.right.z) - (c.right.x * c.forward.z);
	c.up.z = (c.right.x * c.forward.y) - (c.forward.x * c.right.y);
	float upLen = 1.0f / sqrtf(
		(c.up.x*c.up.x) +
		(c.up.y*c.up.y) +
		(c.up.z*c.up.z));
	c.up.x *= upLen;
	c.up.y *= upLen;
	c.up.z *= upLen;

	c.right.x *= imgHalfWidth;
	c.right.y *= imgHalfWidth;
	c.right.z *= imgHalfWidth;
	c.up.x *= imgHalfHeight;
	c.up.y *= imgHalfHeight;
	c.up.z *= imgHalfHeight;

	return c;
}

Ray imageRay(Camera *this, float x, float y)
{
	Ray r;
	r.point = this->eye;
	r.direction.x = 
		((this->eye.x + this->forward.x) + (this->up.x - this->right.x) +
		(this->right.x * x * 2.0f) + (-this->up.x * y * 2.0f) - this->eye.x);
	r.direction.y = 
		((this->eye.y + this->forward.y) + (this->up.y - this->right.y) +
		(this->right.y * x * 2.0f) + (-this->up.y * y * 2.0f) - this->eye.y);
	r.direction.z = 
		((this->eye.z + this->forward.z) + (this->up.z - this->right.z) +
		(this->right.z * x * 2.0f) + (-this->up.z * y * 2.0f) - this->eye.z);
	float imgDirLen = 1.0f / sqrtf(
		(r.direction.x*r.direction.x) +
		(r.direction.y*r.direction.y) +
		(r.direction.z*r.direction.z));
	r.direction.x *= imgDirLen;
	r.direction.y *= imgDirLen;
	r.direction.z *= imgDirLen;
	r.depth = INIT_DEPTH;
	return r;
}