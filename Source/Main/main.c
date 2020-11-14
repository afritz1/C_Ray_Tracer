#include "main.h"
#include <math.h>
#include <omp.h>
#include <stdlib.h>
#include <time.h>
#include "../RT/Cameras/camera.h"
#include "../RT/Vectors/vector3.h"
#include "../RT/Worlds/world.h"

void handleInput(World *world)
{
	int x = 0, y = 0;
	size_t clicked = SDL_GetRelativeMouseState(&x, &y);
	// maybe multiply by the dot product with the plane perpendicular to (0,1,0).
	float mouseSpeed = sqrtf((float)(x*x*0.4f + y*y));

#pragma warning(push)
#pragma warning(disable:4800)
	unsigned char *keys = SDL_GetKeyState(NULL);
	char leftClick = clicked & SDL_BUTTON(SDL_BUTTON_LEFT);
	char rightClick = clicked & SDL_BUTTON(SDL_BUTTON_RIGHT);
	char forward = keys[SDLK_w] | keys[SDLK_UP];
	char backward = keys[SDLK_s] | keys[SDLK_DOWN];
	char left = keys[SDLK_a] | keys[SDLK_LEFT];
	char right = keys[SDLK_d] | keys[SDLK_RIGHT];
	char up = keys[SDLK_SPACE];
	char down = keys[SDLK_LSHIFT];
	char turnRight = (x > 0) && (rightClick || leftClick);
	char turnLeft = (x < 0) && (rightClick || leftClick);
	char turnUp = (y < 0) && (rightClick || leftClick);
	char turnDown = (y > 0) && (rightClick || leftClick);
	char any = forward | backward | left | right | up | down |
		turnRight | turnLeft | turnUp | turnDown;
#pragma warning(pop)

	if (!any) return;

	struct Camera camera = *world->camera;
	Vector3 globalUp = { 0.0f, 1.0f, 0.0f };
	Vector3 newEye = camera.eye;
	Vector3 newFwd = camera.forward;
	Vector3 newRgt = camera.right;
	Vector3 newUp = camera.up;
	float velocity = 0.15f;

	newEye.x +=
		(newFwd.x*velocity*forward) -
		(newFwd.x*velocity*backward) +
		(newRgt.x*velocity*right) -
		(newRgt.x*velocity*left) +
		(newUp.x*velocity*up) -
		(newUp.x*velocity*down);
	newEye.y +=
		(newFwd.y*velocity*forward) -
		(newFwd.y*velocity*backward) +
		(newRgt.y*velocity*right) -
		(newRgt.y*velocity*left) +
		(newUp.y*velocity*up) -
		(newUp.y*velocity*down);
	newEye.z +=
		(newFwd.z*velocity*forward) -
		(newFwd.z*velocity*backward) +
		(newRgt.z*velocity*right) -
		(newRgt.z*velocity*left) +
		(newUp.z*velocity*up) -
		(newUp.z*velocity*down);

	newFwd.x +=
		(newRgt.x*(velocity*0.05f*mouseSpeed)*turnRight) -
		(newRgt.x*(velocity*0.05f*mouseSpeed)*turnLeft) +
		(newUp.x*(velocity*0.05f*mouseSpeed)*turnUp) -
		(newUp.x*(velocity*0.05f*mouseSpeed)*turnDown);
	newFwd.y +=
		(newRgt.y*(velocity*0.05f*mouseSpeed)*turnRight) -
		(newRgt.y*(velocity*0.05f*mouseSpeed)*turnLeft) +
		(newUp.y*(velocity*0.05f*mouseSpeed)*turnUp) -
		(newUp.y*(velocity*0.05f*mouseSpeed)*turnDown);
	newFwd.z +=
		(newRgt.z*(velocity*0.05f*mouseSpeed)*turnRight) -
		(newRgt.z*(velocity*0.05f*mouseSpeed)*turnLeft) +
		(newUp.z*(velocity*0.05f*mouseSpeed)*turnUp) -
		(newUp.z*(velocity*0.05f*mouseSpeed)*turnDown);

	float newFwdLengthRecip = 1.0f / sqrtf(
		(newFwd.x*newFwd.x) + (newFwd.y*newFwd.y) + (newFwd.z*newFwd.z));
	newFwd.x *= newFwdLengthRecip;
	newFwd.y *= newFwdLengthRecip;
	newFwd.z *= newFwdLengthRecip;

	newRgt.x = (newFwd.y * globalUp.z) - (globalUp.y * newFwd.z);
	newRgt.y = (globalUp.x * newFwd.z) - (newFwd.x * globalUp.z);
	newRgt.z = (newFwd.x * globalUp.y) - (globalUp.x * newFwd.y);
	float newRgtLengthRecip = 1.0f / sqrtf(
		(newRgt.x*newRgt.x) + (newRgt.y*newRgt.y) + (newRgt.z*newRgt.z));
	newRgt.x *= newRgtLengthRecip * camera.aspect;
	newRgt.y *= newRgtLengthRecip * camera.aspect;
	newRgt.z *= newRgtLengthRecip * camera.aspect;

	newUp.x = (newRgt.y * newFwd.z) - (newFwd.y * newRgt.z);
	newUp.y = (newFwd.x * newRgt.z) - (newRgt.x * newFwd.z);
	newUp.z = (newRgt.x * newFwd.y) - (newFwd.x * newRgt.y);
	float newUpLengthRecip = 1.0f / sqrtf(
		(newUp.x*newUp.x) + (newUp.y*newUp.y) + (newUp.z*newUp.z));
	newUp.x *= newUpLengthRecip;
	newUp.y *= newUpLengthRecip;
	newUp.z *= newUpLengthRecip;

	if (any) {
		done_rendering = FALSE;
		camera.eye = newEye;
		camera.forward = newFwd;
		camera.right = newRgt;
		camera.up = newUp;
		*world->camera = camera;
	}
}

void setPixel(SDL_Surface *dst, unsigned int x, unsigned int y, unsigned int pixel)
{
	int bpp = dst->format->BytesPerPixel;
	unsigned char *p = (unsigned char*)dst->pixels + y * dst->pitch + x * bpp;
	switch (bpp) {
	case 1:
		*p = pixel;
		break;
	case 2:
		*(unsigned short*)p = pixel;
		break;
	case 3:
		if (SDL_BYTEORDER == SDL_BIG_ENDIAN) {
			p[0] = (pixel >> 16) & 0xFF;
			p[1] = (pixel >> 8) & 0xFF;
			p[2] = pixel & 0xFF;
		}
		else {
			p[0] = pixel & 0xFF;
			p[1] = (pixel >> 8) & 0xFF;
			p[2] = (pixel >> 16) & 0xFF;
		}
		break;
	case 4:
		*(unsigned int*)p = pixel;
		break;
	}
}

void render(SDL_Surface *dst, World *world)
{
	float screenWidthRecip = 1.0f / SCREEN_WIDTH;
	float screenHeightRecip = 1.0f / SCREEN_HEIGHT;
	float superSamplesRecip = 1.0f / SUPER_SAMPLES;
	float superSamplesSquaredRecip = 1.0f / (SUPER_SAMPLES * SUPER_SAMPLES);

	short x, y, k, l;
#pragma omp parallel for private(y, k, l)
	for (x = 0; x < SCREEN_WIDTH; x += MAX_PIXEL_SIZE)
	{
		for (y = 0; y < SCREEN_HEIGHT; y += MAX_PIXEL_SIZE)
		{
			Vector3 color;
			unsigned int colorR = 0, colorG = 0, colorB = 0;
			float left = (float)(x - (MAX_PIXEL_SIZE * 0.5f));
			float right = (float)(left + MAX_PIXEL_SIZE);
			float top = (float)(y - (MAX_PIXEL_SIZE * 0.5f));
			float bottom = (float)(top + MAX_PIXEL_SIZE);
			float radius = (float)(((right - left - top + bottom) * 0.25f) * superSamplesRecip);

			for (float i = (float)(left + radius); i < right; i += 2.0f * radius)
			{
				for (float j = (float)(top + radius); j < bottom; j += 2.0f * radius)
				{
					color = colorAt(world, i * screenWidthRecip, j * screenHeightRecip);
					colorR += (unsigned int)(color.x * 255);
					colorG += (unsigned int)(color.y * 255);
					colorB += (unsigned int)(color.z * 255);
				}
			}

			colorR = (unsigned int)(colorR * superSamplesSquaredRecip);
			colorG = (unsigned int)(colorG * superSamplesSquaredRecip);
			colorB = (unsigned int)(colorB * superSamplesSquaredRecip);

			for (k = 0; k < MAX_PIXEL_SIZE; k++)
				for (l = 0; l < MAX_PIXEL_SIZE; l++)
					setPixel(dst,
					x + k < SCREEN_WIDTH ? x + k : x,
					y + l < SCREEN_HEIGHT ? y + l : y,
					(colorR << 16) | (colorG << 8) | colorB);
		}
	}

	done_rendering = TRUE;
}

void tick(World *world)
{

}

#pragma warning(disable:4100)
int main(int argc, char *argv[])
{
	SDL_Surface *screen;
	SDL_Event sdl_event;
	unsigned char running = FALSE;

	SDL_Init(SDL_INIT_VIDEO);
	SDL_putenv("SDL_VIDEO_CENTERED=center");
	SDL_WM_SetCaption(SCREEN_TITLE, NULL);
	screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, SCREEN_FLAGS);
	srand((unsigned int)time(NULL));

	printf("C Ray Tracing! Now with void pointers!\n\n");
	printf("Screen resolution: [%d, %d]\n", SCREEN_WIDTH, SCREEN_HEIGHT);
	printf("Render resolution: [%d, %d]\n", RENDER_WIDTH * SUPER_SAMPLES, RENDER_HEIGHT * SUPER_SAMPLES);
	printf("Pixelsize: %d\n", MAX_PIXEL_SIZE);
	printf("Supersamples: %d\n", SUPER_SAMPLES);
	printf("Light samples: %d\n", LIGHT_SAMPLES);
	printf("Shape count: %d sphere(s), %d plane(s), %d cuboid(s)\n", 
		SPHERE_COUNT, PLANE_COUNT, CUBOID_COUNT);
	printf("Light count: %d point light(s), %d distant light(s), %d sphere light(s)\n", 
		POINT_LIGHT_COUNT, DIST_LIGHT_COUNT, SPHERE_LIGHT_COUNT);

	World world = makeWorld(RENDER_ASPECT);

	running = TRUE;
	done_rendering = FALSE;
	while (running)
	{
		size_t startTime = SDL_GetTicks();
		while (SDL_PollEvent(&sdl_event))
		{
			if ((sdl_event.type == SDL_QUIT) ||
				(sdl_event.type == SDL_KEYDOWN &&
				sdl_event.key.keysym.sym == SDLK_ESCAPE)) running = FALSE;
		}

		handleInput(&world);
		tick(&world);

		if (!done_rendering)
		{
			render(screen, &world);
		}

		SDL_Flip(screen);
		if ((SDL_GetTicks() - startTime) < SCREEN_DELAY)
			SDL_Delay(SCREEN_DELAY - (SDL_GetTicks() - startTime));
	}

	destructWorld(&world);
	SDL_Quit();
	exit(EXIT_SUCCESS);
}