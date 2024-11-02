#ifndef PRETREATMENT_H_
#define PRETREATMENT_H_

#include <SDL2/SDL.h>

void grayscale(SDL_Surface *surface);
void contrast(SDL_Surface *surface, float contrastFactor);
void binarize(SDL_Surface *surface, Uint8 threshold);
void gaussian(SDL_Surface *surface, int kernel_size);
void median(SDL_Surface *surface, int kernel_size);
Uint32 get_major_color(SDL_Surface *surface, int white);
double variance(SDL_Surface *surface);

void ApplyPretreatment(char *filepath);

#endif