#include "../header/gol.h"
#include "../header/config.h"
#include "../header/util.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define change_status(ar, col, i, j)                                           \
  ({                                                                           \
    size_t __k__ = ((i * col) + j);                                            \
    ar[__k__ >> 3] ^= (0x80 >> (__k__ & 7));                                   \
  })

void rand_gol(unsigned char *gol, unsigned int dim) {
  for (size_t i = 0; i < dim / sizeof(rand()); i++)
    memset((gol) + (i * sizeof(rand())), rand(), sizeof(rand()));
}

static inline unsigned char get_neighbor_status(unsigned char *gol,
                                                size_t dim_col,
                                                size_t row, size_t col) {
  if (col >= dim_col) {
      return 0;
  }
  return get_cell(gol, dim_col, row, col) ? 1 : 0;
}


void play_gol(unsigned char *gol, size_t grid_width, size_t local_rows) {
  size_t data_size_bytes = (local_rows * grid_width) >> 3;
  unsigned char *tmp = calloc(data_size_bytes, 1);
  if (!tmp)
    die("Memory error at play_gol function!");

  for (size_t i = 1; i <= local_rows; i++) {
    for (size_t j = 0; j < grid_width; j++) {

      unsigned char neighbor = 0;
      neighbor += get_neighbor_status(gol, grid_width, i - 1, j - 1);
      neighbor += get_neighbor_status(gol, grid_width, i - 1, j);
      neighbor += get_neighbor_status(gol, grid_width, i - 1, j + 1);
      neighbor += get_neighbor_status(gol, grid_width, i, j - 1);
      neighbor += get_neighbor_status(gol, grid_width, i, j + 1);
      neighbor += get_neighbor_status(gol, grid_width, i + 1, j - 1);
      neighbor += get_neighbor_status(gol, grid_width, i + 1, j);
      neighbor += get_neighbor_status(gol, grid_width, i + 1, j + 1);
      size_t tmp_i = i - 1;

      if (get_cell(gol, grid_width, i, j)) {
          if (neighbor == 2 || neighbor == 3) {
              change_status(tmp, grid_width, tmp_i, j);
          }
      } else {
          if (neighbor == 3) {
              change_status(tmp, grid_width, tmp_i, j);
          }
      }
    }
  }

  memcpy(gol + (grid_width / 8), tmp, data_size_bytes);

  free(tmp);
}
