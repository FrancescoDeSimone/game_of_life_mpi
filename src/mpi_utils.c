#include "../header/mpi_utils.h"
#include <stdlib.h> 
#include <math.h> 

int* divide_matrix(size_t col, int n_proc){
	int* division_arr = calloc(sizeof(int),n_proc);
	unsigned int dim = col * col;
	unsigned new_col = sqrt(dim/n_proc);
	for(int i = 0; i < n_proc; i++)
		division_arr[i] = new_col;
	return division_arr;
}
