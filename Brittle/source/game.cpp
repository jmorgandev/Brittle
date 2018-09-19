#include "game.h"


#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

#include <cstdio>

#include "types.h"
#include "entity.h"

const uint SCREEN_WIDTH = 1280;
const uint SCREEN_HEIGHT = 720;

const uint INFO_WIDTH = 320;
const uint INFO_HEIGHT = SCREEN_HEIGHT;

const uint VIEW_WIDTH = SCREEN_WIDTH - INFO_WIDTH;
const uint VIEW_HEIGHT = SCREEN_HEIGHT;

const uint TILE_PX = 64;
const SDL_Point viewtiles = { VIEW_WIDTH / TILE_PX, VIEW_HEIGHT / TILE_PX };

static SDL_Rect viewport = { 0, 0, 1280 - 320, 720 };

static SDL_Rect infoport = { VIEW_WIDTH, 0, INFO_WIDTH, INFO_HEIGHT };

static SDL_Rect rrect = { TILE_PX, TILE_PX, TILE_PX, TILE_PX };

static Entity player = { nullptr, 0, 0, 0 };

static SDL_Texture* font_tex = nullptr;
static SDL_Rect glypth_source_rect;
static SDL_Rect glypth_dest_rect;

const uint MSG_X = VIEW_WIDTH;
const uint MSG_Y = 32;

static bool quit = false;

void Game::draw_message(const char* str) {
	uint i = 0;
	while (*str) {
		if (*str != ' ') {
			char c = *str - '!';
			glypth_source_rect.x = (c % 21) * glypth_source_rect.w;
			glypth_source_rect.y = (c / 21) * glypth_source_rect.h;

			glypth_dest_rect.x = MSG_X + (i * glypth_source_rect.w);
			glypth_dest_rect.y = MSG_Y;

			SDL_RenderCopy(context, font_tex, &glypth_source_rect, &glypth_dest_rect);
		}
		i++;
		str++;
	}
}

bool Game::init_graphics() {
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		printf("SDL error: %s", SDL_GetError());
		return false;
	}

	window = SDL_CreateWindow("Brittle", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	if (window == nullptr) {
		printf("SDL error: %s", SDL_GetError());
		return false;
	}

	context = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	if (context == nullptr) {
		printf("SDL error: %s", SDL_GetError());
		return false;
	}

	if (IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG) {
		printf("SDL error: %s", IMG_GetError());
		return false;
	}

	//load test image
	SDL_Surface* tmp = IMG_Load("res/test.png");
	if (!tmp) {
		printf("SDL error: %s", IMG_GetError());
		return false;
	}

	player.texture = SDL_CreateTextureFromSurface(context, tmp);
	if (!player.texture) {
		printf("SDL error: %s", SDL_GetError());
		return false;
	}

	SDL_FreeSurface(tmp);

	tmp = IMG_Load("res/consolas_24.tga");
	if (!tmp) {
		printf("SDL error: %s", IMG_GetError());
		return false;
	}
	font_tex = SDL_CreateTextureFromSurface(context, tmp);
	if (!player.texture) {
		printf("SDL error: %s", SDL_GetError());
		return false;
	}

	glypth_dest_rect.w = glypth_source_rect.w = (tmp->w / 21);
	glypth_dest_rect.h = glypth_source_rect.h = (tmp->h / 5) - 1;

	SDL_FreeSurface(tmp);

	return true;
}

void Game::process_events() {
	SDL_Event e;
	while (SDL_PollEvent(&e)) {
		switch (e.type) {
		case SDL_QUIT: quit = true; break;
		case SDL_KEYDOWN:
			if (e.key.keysym.sym == SDLK_ESCAPE) quit = true;

			// Movement code
			if (e.key.keysym.sym == SDLK_LEFT) player.px -= 1;
			else if (e.key.keysym.sym == SDLK_RIGHT) player.px += 1;
			else if (e.key.keysym.sym == SDLK_DOWN) player.py += 1;
			else if (e.key.keysym.sym == SDLK_UP) player.py -= 1;

			// Collision code
			if (player.px > viewtiles.x - 1) player.px = viewtiles.x - 1;
			else if (player.px < 0) player.px = 0;
			else if (player.py > viewtiles.y - 1) player.py = viewtiles.y - 1;
			else if (player.py < 0) player.py = 0;

			break;
		}
	}
}

void Game::draw_test() {
	SDL_SetRenderDrawColor(context, 0.8 * 0xFF, 0.5 * 0xFF, 0.2 * 0xFF, 0xFF);
	SDL_RenderClear(context);


	// render code
	rrect.x = player.px * TILE_PX;
	rrect.y = player.py * TILE_PX;

	SDL_RenderCopy(context, player.texture, nullptr, &rrect);

	SDL_SetRenderDrawColor(context, 20, 20, 20, 0xFF);
	SDL_RenderFillRect(context, &infoport);
	draw_message("the quick brown fox jumps over the lazy dog");

	SDL_RenderPresent(context);
	SDL_Delay(16);
}

void Game::free_graphics() {
	if (player.texture)
		SDL_DestroyTexture(player.texture);

	if (context)
		SDL_DestroyRenderer(context);

	if (window)
		SDL_DestroyWindow(window);

	IMG_Quit();
	SDL_Quit();
}

int main(int argc, char** argv) {
	Game brittle;
	
	if (brittle.init_graphics()) {
		while (!quit) {
			brittle.process_events();
			brittle.draw_test();
		}
	}

	brittle.free_graphics();

	return 0;
}