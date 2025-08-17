#pragma once
#include <stddef.h>

#define get_cell(ar,col,i,j) \
	({ \
        size_t __k__ = ((i*col)+j); \
        ((ar[__k__>>3])&(0x80>>(__k__&7))); \
	}) \

#define change_status(ar, col, i, j)                                           \
  ({                                                                           \
    size_t __k__ = ((i * col) + j);                                            \
    ar[__k__ >> 3] ^= (0x80 >> (__k__ & 7));                                   \
  })

void rand_gol(unsigned char*, unsigned int);
void play_gol(unsigned char*, size_t, size_t);

void play_gol_interior(unsigned char *gol_with_ghosts, unsigned char *tmp_buffer, size_t grid_width, size_t local_rows);
void play_gol_boundaries(unsigned char *gol_with_ghosts, unsigned char *tmp_buffer, size_t grid_width, size_t local_rows);
