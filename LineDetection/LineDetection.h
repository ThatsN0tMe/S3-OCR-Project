#ifndef LINEDETECTION_H_
#define LINEDETECTION_H_

#include <SDL2/SDL.h>

double** getBrightness(SDL_Surface* surface);
int** detectEdges(SDL_Surface* surface);
void detectLines(char* filepath);
void drawLine(SDL_Renderer* renderer, double rho, double theta, int h, int w);
void close_program(SDL_Window *window, SDL_Renderer *renderer);
int process_events();

#endif