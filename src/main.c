#include <mpi.h>
#include <math.h>
#include "../header/gol.h" 
#include "../header/util.h" 
#include "../header/render.h" 
#include "../header/mpi_utils.h" 
#include "../header/config.h" 
#include <SDL2/SDL.h> 

int main(){

	int rank, np;
	Uint32 time = SDL_GetTicks();
	MPI_Init(NULL,NULL);
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
	MPI_Comm_size(MPI_COMM_WORLD,&np);
	unsigned char matrix[DIM];
	size_t close = 0;
	SDL_Window* win;
	SDL_Renderer* rend; 
	TTF_Font* Sans;
	FPS fps; 
	unsigned int cells_mpi = (DIM/np)<<3;
    const size_t col_mpi = sqrt(cells_mpi);
    const size_t col = sqrt(CELLS);
	
	if(rank == 0){
		rand_gol(matrix,DIM);

		if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
			die("error initializing SDL: %s\n", SDL_GetError()); 

		win = SDL_CreateWindow("Game of Life", SDL_WINDOWPOS_CENTERED, 
												SDL_WINDOWPOS_CENTERED, 
												1920, 1080, SDL_WINDOW_FULLSCREEN); 
		rend = init_rend(win);
		Sans = init_font("Sans.ttf",24);
		fps = new_fps();
	}

	while (close < 10) { 
		if(rank == 0){
			MPI_Request rec;
			for(unsigned char i = 1; i<np;i++)
				MPI_Isend(matrix+((DIM/np)*i),(DIM/np),MPI_UNSIGNED_CHAR,i,0,MPI_COMM_WORLD,&rec);
			play_gol(matrix,col/np);

		}
		
		else{

			MPI_Request rec;
			MPI_Recv(matrix,DIM/np,MPI_UNSIGNED_CHAR,0,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
			play_gol(matrix,col/np);
			MPI_Isend(matrix,(DIM/np),MPI_UNSIGNED_CHAR,0,1,MPI_COMM_WORLD,&rec);
		}
		
		if(rank==0)
		{
			for(unsigned char i = 1; i<np;i++)
				MPI_Recv(matrix+((DIM/np)*i),DIM/np,MPI_UNSIGNED_CHAR,i,1,MPI_COMM_WORLD,MPI_STATUS_IGNORE);

			render_cell(matrix,col,CELLS,rend);
			render_FPS(fps.current,rend,Sans);
			SDL_RenderPresent(rend); 
			SDL_RenderClear(rend);
			update_fps(fps);
			if(time<SDL_GetTicks() - 1000){
				printf("%d\n",fps.current);
				close++;
				time = SDL_GetTicks();
			}
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
