#ifndef MAIN_H
#define MAIN_H

#include "../c_rt01.h"
#include <SDL.h>

// ---------------------------------
// SDL window defines
// ---------------------------------

#define IS_RESIZABLE FALSE
#define IS_OPENGL FALSE
#define IS_FULLSCREEN FALSE
#define IS_HARDWARE_SURFACE TRUE
#define IS_DOUBLE_BUFFERED TRUE
#define SCREEN_FLAGS \
	((IS_RESIZABLE ? SDL_RESIZABLE : 0) | \
	(IS_OPENGL ? SDL_OPENGL : 0) | \
	(IS_FULLSCREEN ? SDL_FULLSCREEN : 0) | \
	(IS_HARDWARE_SURFACE ? SDL_HWSURFACE : SDL_SWSURFACE) | \
	(IS_DOUBLE_BUFFERED ? SDL_DOUBLEBUF : 0)) 

#define SCREEN_TITLE "C Ray Tracing! WASD/Space/LShift to move, mouse to look"
#define SCREEN_WIDTH ((short)1600)
#define SCREEN_HEIGHT ((short)900)
#define SCREEN_ASPECT ((float)(SCREEN_WIDTH) / (float)(SCREEN_HEIGHT))
#define SCREEN_FPS ((char)60)
#define SCREEN_DELAY (1000 / SCREEN_FPS)
#define SCREEN_BPP ((char)32)

#define MAX_PIXEL_SIZE 12
#define SUPER_SAMPLES 1

#define RENDER_WIDTH (SCREEN_WIDTH / MAX_PIXEL_SIZE)
#define RENDER_HEIGHT (SCREEN_HEIGHT / MAX_PIXEL_SIZE)
#define RENDER_ASPECT ((float)(RENDER_WIDTH) / (float)(RENDER_HEIGHT))

static char done_rendering = FALSE;

#endif