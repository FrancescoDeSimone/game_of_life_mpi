#pragma once
#include <SDL2/SDL.h>
#include "../header/gol.h"

typedef struct {
    SDL_Texture* gol_texture;
    void* pixel_buffer;
    int pitch;
} TextureRenderer;

TextureRenderer init_texture_renderer(SDL_Renderer* rend, size_t grid_width);
void render_gol_to_texture(TextureRenderer* tex_rend, unsigned char* gol_matrix, size_t grid_width);
void destroy_texture_renderer(TextureRenderer* tex_rend);
