#pragma once
#include <stddef.h>

typedef unsigned char* gol_matrix; 

#define get_cell(ar,col,i,j) \
	({ \
        size_t __k__ = ((i*col)+j); \
        ((ar[__k__>>3])&(0x80>>(__k__&7))); \
	}) \

void rand_gol(gol_matrix*,unsigned int);
gol_matrix new_gol(unsigned int dim);
void play_gol(gol_matrix, size_t dim);
