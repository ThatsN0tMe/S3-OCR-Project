#ifndef PRETREATMENT_H_
#define PRETREATMENT_H_

#include <SDL2/SDL.h>

void save();
void grayscale();
void contrast();
void binarize();
void gaussian();
void median();
void brightness();
Uint32 get_major_color();
double variance();

void pretreatment(char* filepath);

#endif