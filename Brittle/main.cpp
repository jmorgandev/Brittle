
#include <memory>
#include <functional>

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

template <typename T>
using SDL_unique_ptr = std::unique_ptr<T, std::function<void(T *)>>;

struct Actor
{
	SDL_unique_ptr<SDL_Texture> texture;
	uint32_t x_position, y_position;
};

SDL_Rect index_into_texture(int col, int row, int w, int h, SDL_Surface * s)
{
	int s_w = s->w;
	int s_h = s->h;
	
	int x_off = col * w;
	int y_off = row * h;

	return { x_off, y_off, w, h };
}

int main(int argc, char **argv)
{
	SDL_unique_ptr<SDL_Window> window = {
		SDL_CreateWindow("Title", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_SHOWN),
		SDL_DestroyWindow
	};

	if (IMG_Init(IMG_INIT_PNG) == 0)
		throw "Fail";

	SDL_unique_ptr<SDL_Renderer> renderer = {
		SDL_CreateRenderer(window.get(), -1, SDL_RendererFlags::SDL_RENDERER_ACCELERATED),
		SDL_DestroyRenderer
	};

	SDL_unique_ptr<SDL_Surface> surface = {
		IMG_Load("C:/Users/jamie/Documents/Github/Brittle/Brittle/res/normal_character.png"),
		SDL_FreeSurface
	};

	SDL_unique_ptr<SDL_Texture> texture = {
		SDL_CreateTextureFromSurface(renderer.get(), surface.get()),
		SDL_DestroyTexture
	};

	const int char_pixel_w = 32;
	const int char_pixel_h = 32;
	const int row = 0;
	const int col = 0;

	Actor player;
	player.texture = std::move(texture);
	player.x_position = (800/2);
	player.y_position = (600/2);

	bool game_running = true;

	SDL_SetRenderDrawColor(renderer.get(), 255, 255, 255, 255);

	while (game_running)
	{
		SDL_Event e;
		while (SDL_PollEvent(&e))
		{
			if (e.type == SDL_QUIT)
			{
				game_running = false;
			}

			if (e.type == SDL_KEYDOWN)
			{
				if (e.key.keysym.sym == SDLK_w) player.y_position--;
				if (e.key.keysym.sym == SDLK_a) player.x_position--;
				if (e.key.keysym.sym == SDLK_s) player.y_position++;
				if (e.key.keysym.sym == SDLK_d) player.x_position++;
			}
		}

		
		uint64_t ticks = SDL_GetTicks64();
		uint64_t r = (ticks / 1000) % (surface->w / 32);
		SDL_Rect src_rect = index_into_texture(r, 10, 32, 32, surface.get());

		SDL_Rect dst_rect = {
			player.x_position - (128 / 2),
			player.y_position - (128 / 2),
			128, 128
		};

		SDL_RenderClear(renderer.get());
		SDL_RenderCopy(renderer.get(), player.texture.get(), &src_rect, &dst_rect);
		SDL_RenderPresent(renderer.get());
	}

	return 0;
}