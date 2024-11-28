#ifndef DETECTION_H_
#define DETECTION_H_

#include <SDL2/SDL.h>

double** getBrightness(SDL_Surface* surface);
int** detectEdges(SDL_Surface* surface);

void searchCorners();
void pixelLimits(int* x, int* y);
int getPixelNum(int posX, int posY);
void resizeSides(int* x1, int* x2, int* y1, int* y2);
int* searchTopRight(int x, int y, int pixels);
int* searchTopLeft(int x, int y, int pixels);
int* searchBottomRight(int x, int y, int pixels);
int* searchBottomLeft(int x, int y, int pixels);

int isWhiteLine(SDL_Surface* surface, int x1, int y1, int x2, int y2);
int getNumLines(SDL_Surface* surface, int x1, int x2, int y1, int y2);
int getNumColumns(SDL_Surface* surface, int x1, int x2, int y1, int y2);
void detectLetters(SDL_Surface* surface, int x1, int x2, int y1, int y2);

void drawSquare(SDL_Surface* surface, int posX, int posY, int sqW, int sqH);
void drawPolarLine(SDL_Surface* surface, double rho, double theta, int h, int w);
void drawLineOnSurface(SDL_Surface *surface, int x1, int y1, int x2, int y2);
void putPixel(SDL_Surface *surface, int x, int y);

void detectGrids(char* filepath);
void detectLines(char* filepath);
void detection(char* filepath);

#endif