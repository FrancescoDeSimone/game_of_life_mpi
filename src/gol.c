#include "../header/gol.h"
#include "../header/config.h"
#include "../header/util.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

static void calculate_row(unsigned char* gol_with_ghosts, unsigned char* tmp_buffer, size_t grid_width, size_t local_row_idx) {
    for (size_t j = 0; j < grid_width; j++) {
        unsigned char neighbor = 0;
        neighbor += get_neighbor_status(gol_with_ghosts, grid_width, local_row_idx - 1, j - 1);
        neighbor += get_neighbor_status(gol_with_ghosts, grid_width, local_row_idx - 1, j);
        neighbor += get_neighbor_status(gol_with_ghosts, grid_width, local_row_idx - 1, j + 1);
        neighbor += get_neighbor_status(gol_with_ghosts, grid_width, local_row_idx, j - 1);
        neighbor += get_neighbor_status(gol_with_ghosts, grid_width, local_row_idx, j + 1);
        neighbor += get_neighbor_status(gol_with_ghosts, grid_width, local_row_idx + 1, j - 1);
        neighbor += get_neighbor_status(gol_with_ghosts, grid_width, local_row_idx + 1, j);
        neighbor += get_neighbor_status(gol_with_ghosts, grid_width, local_row_idx + 1, j + 1);

        size_t tmp_i = local_row_idx - 1;

        if (get_cell(gol_with_ghosts, grid_width, local_row_idx, j)) {
            if (neighbor == 2 || neighbor == 3) {
                change_status(tmp_buffer, grid_width, tmp_i, j);
            }
        } else {
            if (neighbor == 3) {
                change_status(tmp_buffer, grid_width, tmp_i, j);
            }
        }
    }
}


void play_gol_interior(unsigned char *gol_with_ghosts, unsigned char *tmp_buffer, size_t grid_width, size_t local_rows) {
    if (local_rows <= 2) {
        return;
    }
    for (size_t i = 2; i < local_rows; i++) {
        calculate_row(gol_with_ghosts, tmp_buffer, grid_width, i);
    }
}

void play_gol_boundaries(unsigned char *gol_with_ghosts, unsigned char *tmp_buffer, size_t grid_width, size_t local_rows) {
    if (local_rows < 1) {
        return;
    }
    calculate_row(gol_with_ghosts, tmp_buffer, grid_width, 1);
    if (local_rows > 1) {
        calculate_row(gol_with_ghosts, tmp_buffer, grid_width, local_rows);
    }
}


// Only used if single processor
void play_gol(unsigned char *gol, size_t grid_width, size_t local_rows) {
    size_t data_size_bytes = (local_rows * grid_width) >> 3;
    unsigned char *tmp = calloc(data_size_bytes, 1);
    if (!tmp)
        die("Memory error at play_gol function!");
    for (size_t i = 0; i < local_rows; i++) {
        for (size_t j = 0; j < grid_width; j++) {
            unsigned char neighbor = 0;
            for (long ni = i - 1; ni <= (long)i + 1; ni++) {
                for (long nj = j - 1; nj <= (long)j + 1; nj++) {
                    if (ni == i && nj == j) continue;
                    if (ni >= 0 && ni < (long)local_rows && nj >= 0 && nj < (long)grid_width) {
                        if (get_cell(gol, grid_width, ni, nj)) {
                            neighbor++;
                        }
                    }
                }
            }
            if (get_cell(gol, grid_width, i, j)) {
                if (neighbor == 2 || neighbor == 3) change_status(tmp, grid_width, i, j);
            } else {
                if (neighbor == 3) change_status(tmp, grid_width, i, j);
            }
        }
    }
    memcpy(gol, tmp, data_size_bytes);
    free(tmp);
}
