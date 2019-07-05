#pragma once
#include "../header/gol.h"
#include <SDL2/SDL.h> 
#include <SDL2/SDL_ttf.h> 

void render_cell(gol_matrix,size_t,unsigned int,SDL_Renderer*);
void render_cell2(gol_matrix,size_t,SDL_Renderer*);
void render_FPS(int, SDL_Renderer*,TTF_Font*);
