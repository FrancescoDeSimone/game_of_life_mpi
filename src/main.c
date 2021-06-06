#include "../header/config.h"
#include "../header/gol.h"
#include "../header/mpi_utils.h"
#include "../header/render.h"
#include "../header/util.h"
#include <mpi.h>


void draw(unsigned char *matrix, const size_t *col, SDL_Renderer *rend,
          FPS *fps, TTF_Font *sans) {
  render_cell(matrix, *col, CELLS, rend);
  render_FPS(fps->current, rend, sans);
  SDL_RenderPresent(rend);
  SDL_RenderClear(rend);
}

int main() {
  int rank, np;
  Uint32 time = SDL_GetTicks();
  MPI_Init(NULL, NULL);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &np);

  unsigned char quit = 0;
  unsigned char matrix[DIM];
  SDL_Window *win;
  SDL_Renderer *rend;
  TTF_Font *Sans;
  FPS fps;
  SDL_Event event;
  const size_t col = sqrt(CELLS);

  if (rank == 0) {
    rand_gol(matrix, DIM);

    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
      die("error initializing SDL: %s\n", SDL_GetError());

    win = SDL_CreateWindow("Game of Life", SDL_WINDOWPOS_CENTERED,
                           SDL_WINDOWPOS_CENTERED, 1920, 1080,
                           SDL_WINDOW_FULLSCREEN);
    rend = init_rend(win);
    Sans = init_font("Sans.ttf", 24);
    fps = new_fps();
  }

  while (!quit) {
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
      case SDL_KEYDOWN:
        switch (event.key.keysym.sym) {
        case SDLK_q:
          quit = 1;
        }
        break;
      case SDL_QUIT:
        quit = 1;
        break;
      }
    }
    if (np == 1) {
      play_gol(matrix, col, 2); // avoid 0 division passing 2
      draw(matrix, &col, rend, &fps, Sans);
      update_fps(fps);
    } else {
      if (rank == 0) {
        MPI_Request rec;
        for (unsigned char i = 1; i < np; i++) {
          MPI_Isend(&quit, 1, MPI_UNSIGNED_CHAR, i, 0, MPI_COMM_WORLD, &rec);
          MPI_Isend(matrix + ((DIM / (np - 1)) * (i - 1)), (DIM / (np - 1)),
                    MPI_UNSIGNED_CHAR, i, 0, MPI_COMM_WORLD, &rec);
        }
        for (unsigned char i = 1; i < np; i++)
          MPI_Recv(matrix + ((DIM / (np - 1)) * (i - 1)), DIM / (np - 1),
                   MPI_UNSIGNED_CHAR, i, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        draw(matrix, &col, rend, &fps, Sans);
        update_fps(fps);
      } else {
        MPI_Request rec;
        MPI_Irecv(&quit, 1, MPI_UNSIGNED_CHAR, 0, 0, MPI_COMM_WORLD, &rec);
        MPI_Recv(matrix, DIM / (np - 1), MPI_UNSIGNED_CHAR, 0, 0,
                 MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        play_gol(matrix, col, np);
        MPI_Isend(matrix, (DIM / (np - 1)), MPI_UNSIGNED_CHAR, 0, 1,
                  MPI_COMM_WORLD, &rec);
      }
    }
  }

  if (rank == 0) {
    for (unsigned char i = 1; i < np; i++)
      MPI_Send(&quit, 1, MPI_UNSIGNED_CHAR, i, 0, MPI_COMM_WORLD);
    SDL_DestroyRenderer(rend);
    SDL_DestroyWindow(win);
    TTF_Quit();
    SDL_Quit();
  }
  MPI_Finalize();
}
