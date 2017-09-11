/*
 * sdl.cpp
 *
 *  Created on: Sep 9, 2017
 *      Author: sheldon
 */

#include <unistd.h>
#include "SDL.h"
static const int width = 500;
static const int height = 500;

/* Moving Rectangle */
int main(int argc, char *argv[])
{
	SDL_Window *window;
	SDL_Renderer *renderer;
	SDL_Texture *texture;
	SDL_Event event;
	SDL_Rect r;

	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
				"Couldn't initialize SDL: %s", SDL_GetError());
		return 3;
	}

	window = SDL_CreateWindow("SDL_CreateTexture", SDL_WINDOWPOS_UNDEFINED,
			SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_RESIZABLE);

	r.w = 100;
	r.h = 50;

	renderer = SDL_CreateRenderer(window, -1, 0);

	texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888,
			SDL_TEXTUREACCESS_TARGET, width*2, height*2);
	printf("%d, %d\n", 0x7fc658289dc0 - 0x7fc65808ad80, 0x7fc658309b00 - 0x7fc658289dc0);
	printf("0x%x, 0x%x\n", SDL_PIXELFORMAT_YV12, SDL_PIXELFORMAT_ARGB8888);
	while (1) {
		SDL_PollEvent(&event);
		if (event.type == SDL_QUIT)
			break;
		r.x = rand() % 500;
		r.y = rand() % 500;
		printf("rect:[%d, %d, %d, %d]\n", r.x, r.y, r.x+r.w, r.y+r.h);
		SDL_SetRenderTarget(renderer, texture);
		SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0x00);
		SDL_RenderClear(renderer);
		SDL_RenderDrawRect(renderer, &r);
		SDL_SetRenderDrawColor(renderer, 0xFF, 0x00, 0x00, 0x00);
		SDL_RenderFillRect(renderer, &r);
		SDL_SetRenderTarget(renderer, NULL);
		SDL_RenderCopy(renderer, texture, NULL, NULL);
		SDL_RenderPresent(renderer);
		sleep(1);
	}
	SDL_DestroyRenderer(renderer);
	SDL_Quit();
	return 0;
}
