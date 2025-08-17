#include "../header/config.h"
#include "../header/gol.h"
#include "../header/mpi_utils.h"
#include "../header/render.h"
#include "../header/util.h"
#include <mpi.h>
#include <math.h>
#include <stdlib.h>
#include "../header/render_texture.h"

void draw(unsigned char *matrix, const size_t *col, SDL_Renderer *rend, TextureRenderer* tex_rend,
          FPS *fps, TTF_Font *sans) {
  SDL_SetRenderDrawColor(rend, 0, 0, 0, 255);
  SDL_RenderClear(rend);
  render_cell(matrix, *col, CELLS, rend);
  // render_gol_to_texture(tex_rend, matrix, *col);
  // SDL_RenderCopy(rend, tex_rend->gol_texture, NULL, NULL);
  render_FPS(fps->current, rend, sans);
  SDL_RenderPresent(rend);
}

int main() {
  int rank, np;
  MPI_Init(NULL, NULL);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &np);

  unsigned char quit = 0;
  unsigned char *matrix = NULL;
  SDL_Window *win = NULL;
  SDL_Renderer *rend = NULL;
  TTF_Font *Sans = NULL;
  FPS fps;
  SDL_Event event;
  const size_t col = sqrt(CELLS);

  TextureRenderer tex_rend;
  if (rank == 0) {
    matrix = malloc(DIM);
    if (!matrix) die("Failed to allocate matrix memory on rank 0");
    rand_gol(matrix, DIM);

    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
      die("error initializing SDL: %s\n", SDL_GetError());

    win = SDL_CreateWindow("Game of Life", SDL_WINDOWPOS_CENTERED,
                           SDL_WINDOWPOS_CENTERED, 1920, 1080, 0);
    rend = init_rend(win);
    Sans = init_font("Sans.ttf", 24);
    fps = new_fps();
    tex_rend = init_texture_renderer(rend, col);
  }

  unsigned char *local_matrix = NULL;
  if (np > 1 && rank != 0) {
      size_t rows_per_worker = col / (np - 1);
      size_t remainder_rows = col % (np - 1);
      size_t max_rows = rows_per_worker + remainder_rows;
      size_t row_size_bytes = col / 8;
      local_matrix = malloc((max_rows * row_size_bytes) + (2 * row_size_bytes));
      if (!local_matrix) die("Failed to allocate local_matrix on worker %d", rank);
  }

  while (!quit) {
    if (rank == 0) {
        while (SDL_PollEvent(&event)) {
          if (event.type == SDL_QUIT || (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_q)) {
              quit = 1;
          }
        }
    }

    MPI_Bcast(&quit, 1, MPI_UNSIGNED_CHAR, 0, MPI_COMM_WORLD);
    if (quit) {
        break;
    }

    if (np == 1) {
      play_gol(matrix, col, col);
      draw(matrix, &col, rend, &tex_rend, &fps, Sans);
      update_fps(fps);
    } else {
      if (rank == 0) {
        size_t row_size_bytes = col / 8;
        size_t rows_per_worker = col / (np - 1);
        size_t current_row_offset = 0;
        for (int i = 1; i < np - 1; i++) {
            size_t chunk_size_bytes = rows_per_worker * row_size_bytes;
            size_t offset_bytes = current_row_offset * row_size_bytes;
            MPI_Send(matrix + offset_bytes, chunk_size_bytes, MPI_UNSIGNED_CHAR, i, 1, MPI_COMM_WORLD);
            current_row_offset += rows_per_worker;
        size_t remaining_rows = col - current_row_offset;
        size_t remainder_size_bytes = remaining_rows * row_size_bytes;
        size_t offset_bytes = current_row_offset * row_size_bytes;
        MPI_Send(matrix + offset_bytes, remainder_size_bytes, MPI_UNSIGNED_CHAR, np - 1, 1, MPI_COMM_WORLD);
        current_row_offset = 0;
        for (int i = 1; i < np - 1; i++) {
            size_t chunk_size_bytes = rows_per_worker * row_size_bytes;
            size_t offset_bytes = current_row_offset * row_size_bytes;
            MPI_Recv(matrix + offset_bytes, chunk_size_bytes, MPI_UNSIGNED_CHAR, i, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            current_row_offset += rows_per_worker;
        }

        remaining_rows = col - current_row_offset;
        remainder_size_bytes = remaining_rows * row_size_bytes;
        offset_bytes = current_row_offset * row_size_bytes;
        MPI_Recv(matrix + offset_bytes, remainder_size_bytes, MPI_UNSIGNED_CHAR, np - 1, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        draw(matrix, &col, rend, &tex_rend, &fps, Sans);
        update_fps(fps);

      } else {
        size_t row_size_bytes = col / 8;
        size_t rows_per_worker = col / (np - 1);
        size_t my_rows = (rank == np - 1)
                         ? col - (rows_per_worker * (np - 2))
                         : rows_per_worker;
        size_t my_chunk_size = my_rows * row_size_bytes;
        MPI_Recv(local_matrix + row_size_bytes, my_chunk_size, MPI_UNSIGNED_CHAR, 0, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        int neighbor_up = (rank == 1) ? MPI_PROC_NULL : rank - 1;
        int neighbor_down = (rank == np - 1) ? MPI_PROC_NULL : rank + 1;

        MPI_Sendrecv(local_matrix + row_size_bytes, row_size_bytes, MPI_UNSIGNED_CHAR, neighbor_up, 0,
                     local_matrix, row_size_bytes, MPI_UNSIGNED_CHAR, neighbor_up, 0,
                     MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Sendrecv(local_matrix + my_rows * row_size_bytes, row_size_bytes, MPI_UNSIGNED_CHAR, neighbor_down, 0,
                     local_matrix + my_chunk_size + row_size_bytes, row_size_bytes, MPI_UNSIGNED_CHAR, neighbor_down, 0,
                     MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        play_gol(local_matrix, col, my_rows);

        MPI_Send(local_matrix + row_size_bytes, my_chunk_size, MPI_UNSIGNED_CHAR, 0, 1, MPI_COMM_WORLD);
      }
    }
  }
  // Cleanup
  if (rank == 0) {
    free(matrix);
    SDL_DestroyRenderer(rend);
    destroy_texture_renderer(&tex_rend);
    SDL_DestroyWindow(win);
    TTF_Quit();
    SDL_Quit();
  } else {
    if (local_matrix) free(local_matrix);
  }

  MPI_Finalize();
  return 0;
}
