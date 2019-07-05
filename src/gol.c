#include "../header/gol.h"
#include <stdlib.h>
#include <string.h>

#define change_status(ar,col,i,j) \
	({ \
		size_t __k__ = ((i*col)+j); \
		ar[__k__>>3] ^= (0x80>>(__k__&7)); \
	}) \

gol_matrix new_rand_gol(gol_matrix gol,unsigned int dim){
	gol = malloc(dim);
	for(size_t i = 0; i<dim/sizeof(rand()); i++)
		memset(gol+(i*sizeof(rand())),rand(),sizeof(rand()));
	return gol;
}

static inline unsigned char 
get_neighbor_status(gol_matrix gol, size_t row, size_t col, size_t dim) {
	if (row<0 || row>=dim ) return 0;
	if (col<0 || col>=dim ) return 0;
	return (get_cell(gol,dim,row,col)) ? 1 : 0;
}

unsigned int play_gol(gol_matrix gol, size_t col){
	gol_matrix tmp = calloc(col,col);
	unsigned int alive_cell = 0;
	for(size_t i = 0; i< col; i++)
		for(size_t j = 0; j< col; j++){
			unsigned char neighbor = 0;
			neighbor += get_neighbor_status(gol, i - 1, j - 1, col);
			neighbor += get_neighbor_status(gol, i + 1, j + 1, col);
			neighbor += get_neighbor_status(gol, i + 1, j - 1, col);
			neighbor += get_neighbor_status(gol, i - 1, j + 1, col);
			neighbor += get_neighbor_status(gol, i + 1, j, col);
			neighbor += get_neighbor_status(gol, i - 1, j, col);
			neighbor += get_neighbor_status(gol, i, j + 1, col);
			neighbor += get_neighbor_status(gol, i, j - 1, col);
			if(get_cell(gol,col,i,j)){
				if(neighbor == 2 || neighbor == 3){
					change_status(tmp,col,i,j);	
					alive_cell++;
				}
			} else
				if(neighbor == 3){
					change_status(tmp,col,i,j);
					alive_cell++;
				}
		}

	for(size_t i = 0; i< col; i++)
		for(size_t j = 0; j< col; j++)
			if(get_cell(tmp,col,i,j) != get_cell(gol,col,i,j))
				change_status(gol,col,i,j);
	free(tmp);
	return alive_cell;
}
