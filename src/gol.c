#include "../header/gol.h"
#include "../header/util.h"
#include "../header/config.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define change_status(ar,col,i,j) \
	({ \
		size_t __k__ = ((i*col)+j); \
		ar[__k__>>3] ^= (0x80>>(__k__&7)); \
	}) \


void rand_gol(unsigned char* gol,unsigned int dim){
	for(size_t i = 0; i<dim/sizeof(rand()); i++)
		memset((gol)+(i*sizeof(rand())),rand(),sizeof(rand()));
}
 
static inline unsigned char 
get_neighbor_status(unsigned char* gol, size_t dim, size_t row, size_t col) {
	return !(row<0 || row>=dim || col<0 || col>=dim || !get_cell(gol,dim,row,col));  
}

void play_gol(unsigned char* gol, size_t col){
	unsigned char* tmp = calloc((col*col)>>3,1);
	if(!tmp)
		die("Memory error at play_gol function!");
	
	for(size_t i = 0; i< col; i++)
		for(size_t j = 0; j< col; j++){

			unsigned char neighbor = 0;
			neighbor += get_neighbor_status(gol, col, i - 1, j - 1);
			neighbor += get_neighbor_status(gol, col, i + 1, j + 1);
			neighbor += get_neighbor_status(gol, col, i + 1, j - 1);
			neighbor += get_neighbor_status(gol, col, i - 1, j + 1);
			neighbor += get_neighbor_status(gol, col, i + 1, j);
			neighbor += get_neighbor_status(gol, col, i - 1, j);
			neighbor += get_neighbor_status(gol, col, i, j + 1);
			neighbor += get_neighbor_status(gol, col, i, j - 1);

			if(neighbor == 3)
				change_status(tmp,col,i,j);	
			else if(neighbor == 2 && get_cell(gol,col,i,j))
				change_status(tmp,col,i,j);
		}

	memcpy(gol,tmp,(col*col)>>3);

	free(tmp);
}
