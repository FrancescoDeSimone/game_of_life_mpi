#include "../header/render_texture.h"
#include "../header/util.h"

TextureRenderer init_texture_renderer(SDL_Renderer* rend, size_t grid_width) {
    TextureRenderer tex_rend;
    tex_rend.gol_texture = SDL_CreateTexture(rend,
                                             SDL_PIXELFORMAT_ARGB8888,
                                             SDL_TEXTUREACCESS_STREAMING,
                                             grid_width, grid_width);
    if (!tex_rend.gol_texture) {
        die("Failed to create SDL_Texture: %s", SDL_GetError());
    }
    tex_rend.pixel_buffer = NULL;
    tex_rend.pitch = 0;
    return tex_rend;
}

void render_gol_to_texture(TextureRenderer* tex_rend, unsigned char* gol_matrix, size_t grid_width) {
    if (SDL_LockTexture(tex_rend->gol_texture, NULL, &tex_rend->pixel_buffer, &tex_rend->pitch) != 0) {
        die("Failed to lock texture: %s", SDL_GetError());
    }

    // Cast the raw pixel buffer to a 32-bit integer array for easy color setting.
    // We use ARGB8888 format, which corresponds to a Uint32 on most systems.
    Uint32* pixels = (Uint32*)tex_rend->pixel_buffer;

    const Uint32 ALIVE_COLOR = 0xFF00FF00; // Green
    const Uint32 DEAD_COLOR = 0xFF000000;  // Black

    for (size_t i = 0; i < grid_width; i++) {
        for (size_t j = 0; j < grid_width; j++) {
            size_t pixel_index = i * (tex_rend->pitch / sizeof(Uint32)) + j;
            if (get_cell(gol_matrix, grid_width, i, j)) {
                pixels[pixel_index] = ALIVE_COLOR;
            } else {
                pixels[pixel_index] = DEAD_COLOR;
            }
        }
    }
    SDL_UnlockTexture(tex_rend->gol_texture);
}

void destroy_texture_renderer(TextureRenderer* tex_rend) {
    if (tex_rend && tex_rend->gol_texture) {
        SDL_DestroyTexture(tex_rend->gol_texture);
    }
}
