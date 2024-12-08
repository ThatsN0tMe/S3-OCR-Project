#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "Detection.h"
#include "../Functions.h"
#include "../Rotate/rotate.h"
#include "../Interface/Interface.h"


static int x1 = 0, yy1 = 0, x2 = 0, y2 = 0;
static char* path = NULL;
static SDL_Surface* surface = NULL;


void freeSurface() {
    if (surface != NULL) {
        SDL_FreeSurface(surface);
        surface = NULL;
    }
    x1 = 0;
    yy1 = 0;
    x2 = 0;
    y2 = 0;
}

void saveSurface() {
    if (path == NULL || surface == NULL) {
        puts("Cannot save image.");
        return;
    }

    save(surface, path);
    create_detection_window(path);
}


void gridDetection() {
    autoRotate(path, 1);
    surface = IMG_Load(path);

    int* coords = detectGrid(surface);
    x1 = coords[0];
    yy1 = coords[1];
    x2 = coords[2];
    y2 = coords[3];
    free(coords);

    if (x1 != x2 && yy1 != y2) {
        printf("Grid was successfully detected at coordinates : %d, %d  |  %d, %d\n", x1, yy1, x2, y2);
    }

    /*drawLineOnSurface(surface, x1, yy1, x2, yy1);
    drawLineOnSurface(surface, x2, yy1, x2, y2);
    drawLineOnSurface(surface, x1, yy1, x1, y2);
    drawLineOnSurface(surface, x1, y2, x2, y2);*/
    
    saveSurface();
}

void letterDetection() {
    if (x1 == x2 || yy1 == y2) {
        puts("Cannot detect letters, grid was not correctly detected before.");
        return;
    }
    detectLetters(surface, path, x1, x2, yy1, y2);
    saveSurface();
}

void wordDetection() {
    if (x1 == x2 || yy1 == y2) {
        puts("Cannot detect words, grid was not correctly detected before.");
        return;
    }

    SDL_Rect rect = {x1, yy1, x2 - x1, y2 - yy1};
    Uint32 color = SDL_MapRGB(surface->format, 255, 255, 255);
    SDL_FillRect(surface, &rect, color);

    detectWords(surface, path);
    saveSurface();
}


void detection(char* filepath) {

    if (filepath == NULL) {
        puts("Filepath is undefined");
        return;
    }
    path = filepath;

    surface = IMG_Load(filepath);
    if (surface == NULL) {
        printf("Error loading image: %s\n", IMG_GetError());
        return;
    }

    create_detection_window(filepath);
}