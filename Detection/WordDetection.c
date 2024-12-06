#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "Detection.h"


int getNumWords(SDL_Surface* surface) {

    int numWords = 0,
        nextLine = 1;

    for (int y = 0; y < surface->h - 1; y += 10) {
        if (isWhiteLine(surface, 0, y, surface->w - 1, y)) {
            nextLine = 1;
        }
        else if (nextLine) {
            nextLine = 0;
            numWords++;
        }
    }

    return numWords;
}


void detectWords(SDL_Surface* surface, char* filepath) {

    int width = surface->w,
        height = surface->h,
        nextTopLine = 0,
        previousLineY = 0;

    for (int y = 0; y < height; y++) {
        if (!isWhiteLine(surface, 0, y, width - 1, y)) {
            if (nextTopLine) {
                drawLineOnSurface(surface, 0, previousLineY, width-1, previousLineY);
                nextTopLine = 0;
            }
        }
        else {
            if (!nextTopLine) {
                drawLineOnSurface(surface, 0, y, width-1, y);
                nextTopLine = 1;
            }
            previousLineY = y;
        }
    }
}