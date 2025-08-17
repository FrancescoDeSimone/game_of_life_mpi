#include "../header/render.h"

void render_cell(unsigned char* gol,size_t col, unsigned int cells, SDL_Renderer* rend){
    SDL_Rect* rect_alive = malloc(sizeof(SDL_Rect)*cells);
    if (!rect_alive) return;

    size_t alive_count = 0;
    int win_w, win_h;

    SDL_GetWindowSize(SDL_GL_GetCurrentWindow(), &win_w, &win_h);
    int cell_w = (win_w / col) + 1;
    int cell_h = (win_h / col) + 1;

    for(size_t i = 0; i < col; i++) {
        for(size_t j = 0; j < col; j++){
            if(get_cell(gol,col,i,j)){
                rect_alive[alive_count].x = j * cell_w;
                rect_alive[alive_count].y = i * cell_h;
                rect_alive[alive_count].w = cell_w;
                rect_alive[alive_count].h = cell_h;
                alive_count++;
            }
        }
    }

    SDL_SetRenderDrawColor(rend, 0, 255, 0, 255 );
    SDL_RenderFillRects(rend, rect_alive, alive_count);
    SDL_SetRenderDrawColor(rend, 0, 0, 0, 255 );
    SDL_RenderDrawRects(rend, rect_alive, alive_count);
    free(rect_alive);
}

FPS new_fps(){
	FPS fps;
	fps.lasttime = SDL_GetTicks();
	fps.count = 0;
	fps.current = 0;
	return fps;
}

void render_FPS(int fps, SDL_Renderer* rend, TTF_Font* Sans){
	SDL_Color White = {255, 255, 255, 0};
	char fps_string[12];
	sprintf(fps_string,"FPS: %d", fps);
	SDL_Surface* surfaceMessage = TTF_RenderText_Solid(Sans, fps_string, White);
    if (!surfaceMessage) return;
	SDL_Texture* Message = SDL_CreateTextureFromSurface(rend, surfaceMessage);
    if (!Message) {
        SDL_FreeSurface(surfaceMessage);
        return;
    }

	SDL_Rect Message_rect;
	Message_rect.x = 5;
	Message_rect.y = 5;
	Message_rect.w = 100;
	Message_rect.h = 25;
	SDL_RenderCopy(rend, Message, NULL, &Message_rect);
    SDL_FreeSurface(surfaceMessage);
    SDL_DestroyTexture(Message);
}


TTF_Font* init_font(char* path, unsigned int dim){
	if (TTF_Init() == -1) {
        // Handle error
        return NULL;
    }
	TTF_Font* Sans = TTF_OpenFont(path, dim);
	return Sans;
}

SDL_Renderer* init_rend(SDL_Window* win){
	SDL_Renderer* rend = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	SDL_SetRenderDrawColor(rend, 0, 0, 0, 255 );
	SDL_RenderClear(rend);
	return rend;
}
