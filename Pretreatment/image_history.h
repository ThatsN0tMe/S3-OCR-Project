#ifndef IMAGE_HISTORY_H_
#define IMAGE_HISTORY_H_

#include <SDL2/SDL.h>

typedef struct SurfaceList {
    SDL_Surface* surface;
    struct SurfaceList* next;
} SurfaceList;

void push(SDL_Surface* surface);
SDL_Surface* pop();
int stack_is_empty();
void free_stack();

#endif