#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "Detection.h"


void detectWords(SDL_Surface* surface, char* filepath) {

    int width = surface->w,
        height = surface->h;
    
    int numWords = 0,
        nextBottomLine = 0,
        previousLineY = 0;

    for (int y = 0; y < width; y++) {
        if (!isWhiteLine(surface, 0, y, width - 1, y)) {
            if (!nextBottomLine) {
                drawLineOnSurface(surface, 0, previousLineY, width-1, previousLineY);
                nextBottomLine = 1;
            }
        }
        else {
            if (nextBottomLine) {
                drawLineOnSurface(surface, 0, y, width-1, y);
                nextBottomLine = 0;
                numWords++;
            }
            previousLineY = y;
        }
    }
}