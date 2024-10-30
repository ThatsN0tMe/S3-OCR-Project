#ifndef AFFLELOU_H_
#define AFFLELOU_H_

#include <SDL2/SDL.h>

extern char* filepath;
extern SDL_Surface* surface;

void applyPreprocess();
void doRotation();
void detectLines();
void initSDL();
void quitSDL();

#endif