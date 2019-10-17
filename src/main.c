#include <mpi.h>
#include <math.h>
#include "../header/gol.h" 
#include "../header/util.h" 
#include "../header/render.h" 
#include "../header/mpi_utils.h" 
#include <SDL2/SDL.h> 

int main(){

    unsigned int DIM = 32768;
	unsigned char matrix[32768];
	gol_matrix gol = matrix; 
	int rank, np;
	unsigned int cells = DIM*8;
    const size_t col = sqrt(cells);
	unsigned char close = 0;
	SDL_Window* win;
	SDL_Renderer* rend; 
	TTF_Font* Sans;
	FPS fps; 
	MPI_Init(NULL,NULL);
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
	MPI_Comm_size(MPI_COMM_WORLD,&np);
	
	if(rank == 0){
		rand_gol(&gol,DIM);
		if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
			die("error initializing SDL: %s\n", SDL_GetError()); 

		win = SDL_CreateWindow("Game of Life", SDL_WINDOWPOS_CENTERED, 
												SDL_WINDOWPOS_CENTERED, 
												1366, 768, SDL_WINDOW_FULLSCREEN); 
		rend = init_rend(win);
		Sans = init_font("Sans.ttf",24);
		fps = new_fps();
	}

	while (!close) { 
		if(rank == 0){
			SDL_Event event; 
			while (SDL_PollEvent(&event)) { 
				switch (event.type) { 
				case SDL_QUIT: 
					close = 255; 
					MPI_Bcast(&close,1,MPI_UNSIGNED_CHAR,0,MPI_COMM_WORLD);
					break; 
				}
			}

			MPI_Request rec;
			play_gol(matrix,256);
			for(unsigned char i = 1; i<np;i++)
				MPI_Isend(matrix+(8192*i),8192,MPI_UNSIGNED_CHAR,i,0,MPI_COMM_WORLD,&rec);
		}
		
		else{

			MPI_Request rec;
			MPI_Recv(matrix,8192,MPI_CHAR,0,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
			play_gol(matrix,256);
			MPI_Isend(matrix,8192,MPI_CHAR,0,1,MPI_COMM_WORLD,&rec);
		}
		
		if(rank==0)
		{
			for(unsigned char i = 1; i<np;i++)
				MPI_Recv(matrix+(8192*i),8192,MPI_UNSIGNED_CHAR,i,1,MPI_COMM_WORLD,MPI_STATUS_IGNORE);

			render_cell(matrix,col,cells,rend);
			render_FPS(fps.current,rend,Sans);
			SDL_RenderPresent(rend); 
			SDL_RenderClear(rend);
			update_fps(fps);
		}
	}

	if(!rank){
		SDL_DestroyRenderer(rend); 
		SDL_DestroyWindow(win); 
		TTF_Quit();
		SDL_Quit();
	}

	MPI_Finalize();
}
