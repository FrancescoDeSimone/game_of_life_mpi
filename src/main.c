#include <mpi.h>
#include <math.h>
#include "../header/gol.h" 
#include "../header/util.h" 
#include "../header/render.h" 
#include <SDL2/SDL.h> 

int main(){

    unsigned int DIM = pow(2,15);
	int rank, np;
	unsigned int cells = DIM*8;
    const size_t col = sqrt(cells);
	gol_matrix gol = NULL;
	gol = new_rand_gol(gol,DIM);

	MPI_Init(NULL,NULL);
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
	MPI_Comm_size(MPI_COMM_WORLD,&np);
	if(!rank){
		if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
			die("error initializing SDL: %s\n", SDL_GetError()); 

		SDL_Window* win = SDL_CreateWindow("Game of Life", SDL_WINDOWPOS_CENTERED, 
												SDL_WINDOWPOS_CENTERED, 
												1366, 768, SDL_WINDOW_FULLSCREEN); 

		SDL_Renderer* rend = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED); 
		SDL_SetRenderDrawColor(rend, 0, 0, 0, 255 );
		SDL_RenderClear(rend);
		unsigned char close = 0;
		Uint32 fps_lasttime = SDL_GetTicks();
		Uint32 fps_frames = 0;
		Uint32 fps = 0;
		TTF_Init();
		TTF_Font* Sans = TTF_OpenFont("Sans.ttf", 24); 
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
			render_FPS(fps,rend,Sans);
			SDL_RenderPresent(rend); 
			SDL_RenderClear(rend);
			fps_frames++;
			if(fps_lasttime < SDL_GetTicks() - 1000){
				fps = fps_frames;
				fps_frames = 0;
				fps_lasttime = SDL_GetTicks();
			}
//			SDL_Delay(1000/60);
		} 
		SDL_DestroyRenderer(rend); 
		SDL_DestroyWindow(win); 
		TTF_Quit();
		SDL_Quit();
	}
	else{

	}
	free(gol);
	MPI_Finalize();
}
