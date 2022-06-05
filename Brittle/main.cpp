
#include <memory>
#include <functional>

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

template <typename T>
using SDL_unique_ptr = std::unique_ptr<T, std::function<void(T *)>>;

struct SpriteSheet
{
	std::shared_ptr<SDL_Texture> texture;
	SDL_Point cell_size;

	SDL_Rect get_rect(int row, int col)
	{
		return { col * cell_size.x, row * cell_size.y, cell_size.x, cell_size.y };
	}
};

struct Sprite
{
	SpriteSheet sprite_sheet;
	SDL_Point offset;

	void render(SDL_Renderer * renderer, SDL_Point position, int scale)
	{
		SDL_Rect src = sprite_sheet.get_rect(offset.y, offset.x);
		SDL_Rect dst = {
			position.x - ((sprite_sheet.cell_size.x / 2) * scale),
			position.y - ((sprite_sheet.cell_size.y / 2) * scale),
			sprite_sheet.cell_size.x * scale,
			sprite_sheet.cell_size.y * scale
		};
		SDL_RenderCopy(renderer, sprite_sheet.texture.get(), &src, &dst);
	}
};
struct Actor
{
	Sprite sprite;
	SDL_Point position;
	int scale;

	void render(SDL_Renderer * renderer)
	{
		sprite.render(renderer, position, scale);
	}
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

	SpriteSheet character_sheet;
	character_sheet.texture = { SDL_CreateTextureFromSurface(renderer.get(), surface.get()),
								SDL_DestroyTexture };
	character_sheet.cell_size = { 32, 32 };

	Sprite player_sprite;
	player_sprite.sprite_sheet = character_sheet;
	player_sprite.offset = { 0, 10 };

	Actor player;
	player.sprite = player_sprite;
	player.position = { 800/2, 600/2 };
	player.scale = 3;

	Sprite enemy_sprite;
	enemy_sprite.sprite_sheet = character_sheet;
	enemy_sprite.offset = { 0, 9 };
	
	Actor enemy;
	enemy.sprite = enemy_sprite;
	enemy.position = { 800/2, 600/2 + (32 * 3) };
	enemy.scale = 3;

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
				if (e.key.keysym.sym == SDLK_w) player.position.y--;
				if (e.key.keysym.sym == SDLK_a) player.position.x--;
				if (e.key.keysym.sym == SDLK_s) player.position.y++;
				if (e.key.keysym.sym == SDLK_d) player.position.x++;
			}
		}

		
		uint64_t ticks = SDL_GetTicks64();
		uint64_t r = (ticks / 1000) % (surface->w / 32);
		
		player.sprite.offset.x = r;
		enemy.sprite.offset.x = r;

		SDL_RenderClear(renderer.get());
		player.render(renderer.get());
		enemy.render(renderer.get());
		SDL_RenderPresent(renderer.get());
	}

	return 0;
}