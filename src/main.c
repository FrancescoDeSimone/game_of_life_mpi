#include "../header/config.h"
#include "../header/gol.h"
#include "../header/render.h"
#include "../header/util.h"
#include <mpi.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

void draw(unsigned char *matrix, const size_t *col, SDL_Renderer *rend,
          FPS *fps, TTF_Font *sans) {
  SDL_SetRenderDrawColor(rend, 0, 0, 0, 255);
  SDL_RenderClear(rend);
  render_cell(matrix, *col, CELLS, rend);
  render_FPS(fps->current, rend, sans);
  SDL_RenderPresent(rend);
}

int main() {
  int rank, np;
  MPI_Init(NULL, NULL);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &np);

  unsigned char quit = 0;
  unsigned char paused = 0;
  unsigned char *matrix = NULL;
  SDL_Window *win = NULL;
  SDL_Renderer *rend = NULL;
  TTF_Font *Sans = NULL;
  FPS fps;
  SDL_Event event;
  const size_t col = sqrt(CELLS);

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
  }

  unsigned char *local_matrix = NULL;
  unsigned char *tmp_buffer = NULL; // Persistent temp buffer for workers
  if (np > 1 && rank != 0) {
      size_t rows_per_worker = col / (np - 1);
      size_t remainder_rows = col % (np - 1);
      size_t max_rows = rows_per_worker + remainder_rows;
      size_t row_size_bytes = col / 8;
      local_matrix = malloc((max_rows * row_size_bytes) + (2 * row_size_bytes));
      tmp_buffer = malloc(max_rows * row_size_bytes); // Allocate temp buffer once
      if (!local_matrix || !tmp_buffer) die("Failed to allocate buffers on worker %d", rank);
  }

  while (!quit) {
    if (rank == 0) {
        while (SDL_PollEvent(&event)) {
          if (event.type == SDL_QUIT ) {
              quit = 1;
          }
          if (event.type == SDL_KEYDOWN) {
              switch (event.key.keysym.sym) {
                  case SDLK_q:
                      quit = 1;
                      break;
                  case SDLK_SPACE:
                      paused = !paused;
                      break;
              }
          }
         if (event.type == SDL_MOUSEBUTTONDOWN) {
          int win_w, win_h;
          SDL_GetWindowSize(win, &win_w, &win_h);
          int cell_w = (win_w / col)+1;
          int cell_h = (win_h / col)+1;
            if (cell_w > 0 && cell_h > 0) {
              size_t clicked_col = event.button.x / cell_w;
              size_t clicked_row = event.button.y / cell_h;
              if (clicked_row < col && clicked_col < col) {
                change_status(matrix, col, clicked_row, clicked_col);
              }
            }
          }
        }
    }

    MPI_Bcast(&quit, 1, MPI_UNSIGNED_CHAR, 0, MPI_COMM_WORLD);
    if (quit) {
        break;
    }
    if(!paused) {
      if (np == 1) {
        play_gol(matrix, col, col);
      } else {
        if (rank == 0) {
          size_t row_size_bytes = col / 8;
          size_t rows_per_worker = col / (np - 1);
          size_t current_row_offset = 0;
          for (int i = 1; i < np - 1; i++) {
              size_t chunk_size_bytes = rows_per_worker * row_size_bytes;
              MPI_Send(matrix + (current_row_offset * row_size_bytes), chunk_size_bytes, MPI_UNSIGNED_CHAR, i, 1, MPI_COMM_WORLD);
              current_row_offset += rows_per_worker;
          }
          size_t remainder_size_bytes = (col - current_row_offset) * row_size_bytes;
          MPI_Send(matrix + (current_row_offset * row_size_bytes), remainder_size_bytes, MPI_UNSIGNED_CHAR, np - 1, 1, MPI_COMM_WORLD);
          current_row_offset = 0;
          for (int i = 1; i < np - 1; i++) {
              size_t chunk_size_bytes = rows_per_worker * row_size_bytes;
              MPI_Recv(matrix + (current_row_offset * row_size_bytes), chunk_size_bytes, MPI_UNSIGNED_CHAR, i, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
              current_row_offset += rows_per_worker;
          }
          remainder_size_bytes = (col - current_row_offset) * row_size_bytes;
          MPI_Recv(matrix + (current_row_offset * row_size_bytes), remainder_size_bytes, MPI_UNSIGNED_CHAR, np - 1, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        } else {
          size_t row_size_bytes = col / 8;
          size_t rows_per_worker = col / (np - 1);
          size_t my_rows = (rank == np - 1) ? col - (rows_per_worker * (np - 2)) : rows_per_worker;
          size_t my_chunk_size = my_rows * row_size_bytes;
          MPI_Recv(local_matrix + row_size_bytes, my_chunk_size, MPI_UNSIGNED_CHAR, 0, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
          MPI_Request requests[2];
          int neighbor_up = (rank == 1) ? MPI_PROC_NULL : rank - 1;
          int neighbor_down = (rank == np - 1) ? MPI_PROC_NULL : rank + 1;
          MPI_Irecv(local_matrix, row_size_bytes, MPI_UNSIGNED_CHAR, neighbor_up, 0, MPI_COMM_WORLD, &requests[0]);
          MPI_Irecv(local_matrix + my_chunk_size + row_size_bytes, row_size_bytes, MPI_UNSIGNED_CHAR, neighbor_down, 0, MPI_COMM_WORLD, &requests[1]);
          MPI_Send(local_matrix + row_size_bytes, row_size_bytes, MPI_UNSIGNED_CHAR, neighbor_up, 0, MPI_COMM_WORLD);
          MPI_Send(local_matrix + my_rows * row_size_bytes, row_size_bytes, MPI_UNSIGNED_CHAR, neighbor_down, 0, MPI_COMM_WORLD);

          memset(tmp_buffer, 0, my_chunk_size);
          play_gol_interior(local_matrix, tmp_buffer, col, my_rows);
          MPI_Waitall(2, requests, MPI_STATUSES_IGNORE);
          play_gol_boundaries(local_matrix, tmp_buffer, col, my_rows);
          MPI_Send(tmp_buffer, my_chunk_size, MPI_UNSIGNED_CHAR, 0, 1, MPI_COMM_WORLD);
        }
      }
    }
    if (rank == 0) {
      draw(matrix, &col, rend, &fps, Sans);
      update_fps(fps);
      MPI_Request rec;
      for (unsigned char i = 1; i < np; i++) {
          MPI_Isend(&quit, 1, MPI_UNSIGNED_CHAR, i, 0, MPI_COMM_WORLD, &rec);
      }
    } else {
      MPI_Recv(&quit, 1, MPI_UNSIGNED_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }
  }

  // Cleanup
  if (rank == 0) {
    free(matrix);
    SDL_DestroyRenderer(rend);
    SDL_DestroyWindow(win);
    TTF_Quit();
    SDL_Quit();
    MPI_Request rec;
    for (unsigned char i = 1; i < np; i++) {
        MPI_Isend(&quit, 1, MPI_UNSIGNED_CHAR, i, 0, MPI_COMM_WORLD, &rec);
    }
  } else {
    if (local_matrix) free(local_matrix);
    if (tmp_buffer) free(tmp_buffer);
    MPI_Recv(&quit, 1, MPI_UNSIGNED_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
  }

  MPI_Finalize();
  return 0;
}
