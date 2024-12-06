#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "Detection.h"
#include "../Rotate/rotate.h"


static int x1 = 0, yy1 = 0, x2 = 0, y2 = 0;
static SDL_Surface* surface = NULL;


void detection(char* filepath) {

    if (filepath == NULL) {
        puts("Filepath is undefined");
        return;
    }
    autoRotate(filepath, 1);

    surface = IMG_Load(filepath);
    if (surface == NULL) {
        printf("Error loading image: %s\n", IMG_GetError());
        return;
    }

    int* coords = detectGrid(surface);
    x1 = coords[0];
    yy1 = coords[1];
    x2 = coords[2];
    y2 = coords[3];
    free(coords);

    detectLetters(surface, filepath, x1, x2, yy1, y2);

    /*SDL_Rect rect = {x1, yy1, x2 - x1, y2 - yy1};
    Uint32 color = SDL_MapRGB(surface->format, 255, 255, 255);
    SDL_FillRect(surface, &rect, color);

    detectWords(surface, filepath);*/

    if (SDL_SaveBMP(surface, filepath) != 0) {
        printf("Image saving error: %s\n", SDL_GetError());
    }
    else {
        printf("Image processed successfully and saved as '%s'.\n", filepath);
    }

    SDL_FreeSurface(surface);
}