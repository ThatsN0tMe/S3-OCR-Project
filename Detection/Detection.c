#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "Detection.h"
#include "Cutting.h"

void detection(char* filepath) {
    detectLines(filepath); //rota auto
    detectGrids(filepath);
}