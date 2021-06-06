#pragma once
#include "../header/gol.h"
#include <SDL2/SDL.h> 
#include <SDL2/SDL_ttf.h> 

#define update_fps(__fps__)\
({\
		__fps__.count++; \
		if(__fps__.lasttime < SDL_GetTicks() - 1000){ \
			__fps__.current = (__fps__).count; \
			__fps__.count = 0; \
			__fps__.lasttime = SDL_GetTicks();\
		} \
}) \


typedef struct fps_t{
	Uint32 lasttime;
	Uint32 current;
	Uint32 count;
} FPS;

FPS new_fps();
void render_cell(unsigned char*,size_t,unsigned int,SDL_Renderer*);
void render_FPS(int, SDL_Renderer*,TTF_Font*);

SDL_Renderer* init_rend(SDL_Window* win);
TTF_Font* init_font(char* path, unsigned int dim);
