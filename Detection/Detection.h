#ifndef DETECTION_H_
#define DETECTION_H_

#include <SDL2/SDL.h>

void detectGrids(char* filepath);
void detectLines(char* filepath);
double** getBrightness(SDL_Surface* surface);
int** detectEdges(SDL_Surface* surface);
int* searchTopRight(SDL_Surface* surface, int x, int y, int sqPixels);
int* searchTopLeft(SDL_Surface* surface, int x, int y, int pixels);
int* searchBottomRight(SDL_Surface* surface, int x, int y, int pixels);
int* searchBottomLeft(SDL_Surface* surface, int x, int y, int pixels);

void drawPolarLine(SDL_Surface* surface, double rho, double theta, int h, int w);
void drawLineOnSurface(SDL_Surface *surface, int x1, int y1, int x2, int y2);
void putPixel(SDL_Surface *surface, int x, int y);

void detection(char* filepath);

#endif