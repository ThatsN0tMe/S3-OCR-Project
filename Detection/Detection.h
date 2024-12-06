#ifndef DETECTION_H_
#define DETECTION_H_

#include <SDL2/SDL.h>

double** getBrightness(SDL_Surface* surface);
int** detectEdges(SDL_Surface* surface);

int getPixelNum(int posX, int posY);
void pixelLimits(int* x, int* y);
void resizeSides(int* x1, int* y1, int* x2, int* y2);
int* searchTopRight(int x, int y, int pixels);
int* searchTopLeft(int x, int y, int pixels);
int* searchBottomRight(int x, int y, int pixels);
int* searchBottomLeft(int x, int y, int pixels);

int isWhiteLine(SDL_Surface* surface, int x1, int y1, int x2, int y2);
int getNumLines(SDL_Surface* surface, int x1, int x2, int* y1, int y2);
int getNumColumns(SDL_Surface* surface, int x1, int x2, int y1, int y2);
void removeLines(SDL_Surface* surface, int x1, int x2, int y1, int y2);

int* polarToCartesian(double rho, double theta, int w, int h);
void drawWhiteRect(SDL_Surface* surface, int posX, int posY, int rectW, int rectH);
void drawPolarLine(SDL_Surface* surface, double rho, double theta, int w, int h);
void drawLineOnSurface(SDL_Surface *surface, int x1, int y1, int x2, int y2);
void putPixel(SDL_Surface *surface, int x, int y);

void detection(char* filepath);
int* detectGrid(SDL_Surface* surface);
void detectWords(SDL_Surface* surface, char* filepath);
int** detectLines(SDL_Surface* surface, int* size, int* threshold);
void detectLetters(SDL_Surface* surface, char* filepath, int x1, int x2, int y1, int y2);

void cut_and_save(char* input_file, char* output_file, int x1, int y1, int x2, int y2);

#endif