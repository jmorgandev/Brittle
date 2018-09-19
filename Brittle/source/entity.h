#ifndef ENTITY_H
#define ENTITY_H

#include <SDL/SDL_render.h>

struct Entity {
	SDL_Texture* texture;
	int px, py, pz;
};

#endif