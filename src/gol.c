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
                                                size_t dim_row, size_t dim_col,
                                                size_t row, size_t col) {
  return !(row < 0 || row >= dim_row || col < 0 || col >= dim_col ||
           !get_cell(gol, dim_col, row, col));
}

void play_gol(unsigned char *gol, size_t size, int np) {
  size_t col = size / (np - 1);
  unsigned char *tmp = calloc((col * size) >> 3, 1);
  if (!tmp)
    die("Memory error at play_gol function!");

  for (size_t i = 0; i < size; i++)
    for (size_t j = 0; j < col; j++) {

      unsigned char neighbor = 0;
      neighbor += get_neighbor_status(gol, size, col, i - 1, j - 1);
      neighbor += get_neighbor_status(gol, size, col, i + 1, j + 1);
      neighbor += get_neighbor_status(gol, size, col, i + 1, j - 1);
      neighbor += get_neighbor_status(gol, size, col, i - 1, j + 1);
      neighbor += get_neighbor_status(gol, size, col, i + 1, j);
      neighbor += get_neighbor_status(gol, size, col, i - 1, j);
      neighbor += get_neighbor_status(gol, size, col, i, j + 1);
      neighbor += get_neighbor_status(gol, size, col, i, j - 1);

      if (neighbor == 3)
        change_status(tmp, col, i, j);
      else if (neighbor == 2 && get_cell(gol, col, i, j))
        change_status(tmp, col, i, j);
    }

  memcpy(gol, tmp, (col * col) >> 3);

  free(tmp);
}
