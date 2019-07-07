#include <mpi.h>
#include <math.h>
#include "../header/gol.h" 
#include "../header/util.h" 
#include "../header/render.h" 
#include "../header/mpi_utils.h" 
#include <SDL2/SDL.h> 


SDL_Renderer* init_rend(SDL_Window* win){
	SDL_Renderer* rend = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED); 
	SDL_SetRenderDrawColor(rend, 0, 0, 0, 255 );
	SDL_RenderClear(rend);
	return rend;	
}

TTF_Font* init_font(char* path, unsigned int dim){
	TTF_Init();
	TTF_Font* Sans = TTF_OpenFont(path, dim); 
	return Sans;
}

int main(){

    unsigned int DIM = pow(2,15);
	int rank, np;
	unsigned int cells = DIM*8;
    const size_t col = sqrt(cells);

	MPI_Init(NULL,NULL);
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
	MPI_Comm_size(MPI_COMM_WORLD,&np);
	if(!rank){
		gol_matrix gol = new_gol(DIM); 
		rand_gol(&gol,DIM);
		if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
			die("error initializing SDL: %s\n", SDL_GetError()); 

		SDL_Window* win = SDL_CreateWindow("Game of Life", SDL_WINDOWPOS_CENTERED, 
												SDL_WINDOWPOS_CENTERED, 
												1366, 768, SDL_WINDOW_FULLSCREEN); 
		SDL_Renderer* rend = init_rend(win);
		TTF_Font* Sans = init_font("Sans.ttf",24);
		unsigned char close = 0;
		FPS fps = new_fps();
		int* arr_dim = divide_matrix(col,np);
		while (!close) { 
			SDL_Event event; 
			while (SDL_PollEvent(&event)) { 
				switch (event.type) { 
				case SDL_QUIT: 
					close = 1; 
					break; 
				}
			}
			cells = play_gol(gol,col);
			render_cell(gol,col,cells,rend);
			render_FPS(fps.current,rend,Sans);
			SDL_RenderPresent(rend); 
			SDL_RenderClear(rend);
			update_fps(fps);
			/* SDL_Delay(1000/60); */
		} 
		free(arr_dim);
		free(gol);
		SDL_DestroyRenderer(rend); 
		SDL_DestroyWindow(win); 
		TTF_Quit();
		SDL_Quit();
	}
	else{

	}
	MPI_Finalize();
}
