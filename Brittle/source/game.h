#ifndef GAME_H
#define GAME_H

struct SDL_Window;
struct SDL_Renderer;

class Game {
public:
	bool init_graphics();
	
	void process_events();

	void draw_test();

	void free_graphics();
private:
	SDL_Window* window;
	SDL_Renderer* context;

	void draw_message(const char* str);
};

#endif